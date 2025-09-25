#include "ChartViewModel.h"
#include "../Model/Parameters/BoardParameterSingle.h"
#include <QDebug>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QDateTime>

ChartViewModel::ChartViewModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_parametersStorage(nullptr)
{
    //qDebug() << "ChartViewModel: Created empty model";
}

ChartViewModel::ChartViewModel(BoardParameterHistoryStorage *parametersStorage, QObject *parent)
    : QAbstractListModel(parent)
    , m_parametersStorage(parametersStorage)
{
    setParametersStorage(parametersStorage);
    //qDebug() << "ChartViewModel: Created with parameters storage";
}

void ChartViewModel::setParametersStorage(BoardParameterHistoryStorage* storage)
{
    m_parametersStorage = storage;
    if (m_parametersStorage)
    {
        connect(m_parametersStorage,
                &BoardParameterHistoryStorage::newParameterAdded,
                this, &ChartViewModel::onNewParameterAdded);
    }
}

int ChartViewModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_chartViews.size();
}

QVariant ChartViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_chartViews.size())
        return QVariant();

    const ChartData &chartData = m_chartViews.at(index.row());

    switch (role) {
    case ChartViewRole:
        return QVariant::fromValue(chartData.chartView);
    case ChartTitleRole:
        return chartData.title;
    case ChartIndexRole:
        return index.row();
    case HasDataRole:
        return !chartData.seriesMap.isEmpty();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ChartViewModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ChartViewRole] = "chartView";
    roles[ChartTitleRole] = "chartTitle";
    roles[ChartIndexRole] = "chartIndex";
    roles[HasDataRole] = "hasData";
    return roles;
}

QtCharts::QChartView* ChartViewModel::addChart(const QString &title)
{
    if (hasChart(title))
    {
        //qDebug() << "ChartViewModel: Chart with title" << title << "already exists";
        return getChartView(title);
    }

    beginInsertRows(QModelIndex(), m_chartViews.size(), m_chartViews.size());
    
    ChartData chartData;
    chartData.title = title;
    chartData.chartView = new QtCharts::QChartView();
    chartData.xAxis = new QtCharts::QValueAxis();
    chartData.yAxis = new QtCharts::QValueAxis();
    
    setupChart(&chartData);
    
    m_chartViews.append(chartData);
    endInsertRows();
    
    //qDebug() << "ChartViewModel: Added chart" << title;
    return chartData.chartView;
}

void ChartViewModel::removeChart(int index)
{
    if (index < 0 || index >= m_chartViews.size())
    {
        qWarning() << "ChartViewModel: Invalid index" << index;
        return;
    }
    
    beginRemoveRows(QModelIndex(), index, index);
    ChartData chartData = m_chartViews.takeAt(index);
    
    if (chartData.chartView)
    {
        chartData.chartView->deleteLater();
    }
    
    endRemoveRows();
    
    //qDebug() << "ChartViewModel: Removed chart at index" << index;
}

void ChartViewModel::clearCharts()
{
    if (m_chartViews.isEmpty())
        return;
        
    beginResetModel();
    
    for (const ChartData &chartData : m_chartViews)
    {
        if (chartData.chartView)
        {
            chartData.chartView->deleteLater();
        }
    }
    
    m_chartViews.clear();
    endResetModel();
    
    //qDebug() << "ChartViewModel: Cleared all charts";
}

void ChartViewModel::addDataPoint(const QString &chartTitle, const QString &parameterLabel, 
                                  double x, double y, const QDateTime &timestamp, const QVariant &value)
{
    ChartData* chartData = findChartData(chartTitle);
    if (!chartData)
    {
        //qWarning() << "ChartViewModel: Chart not found:" << chartTitle;
        return;
    }
    
    QtCharts::QLineSeries* series = getOrCreateSeries(chartData, parameterLabel);
    if (!series)
    {
        //qWarning() << "ChartViewModel: Failed to create series for parameter:" << parameterLabel;
        return;
    }
    
    // Устанавливаем время начала если это первая точка
    if (chartData->startTime.isNull())
    {
        chartData->startTime = timestamp;
    }
    
    // Добавляем точку в серию
    series->append(x, y);
    
    // Ограничиваем количество точек для производительности
    if (series->count() > 100)
    {
        series->removePoints(0, 1);
    }
    
    // Обновляем оси
    chartData->xAxis->setMin(x - 50);
    chartData->xAxis->setMax(x + 10);
    
    emit chartDataAdded(chartTitle, parameterLabel);
    
    //qDebug() << "ChartViewModel: Added point to chart" << chartTitle << "parameter" << parameterLabel;
}

void ChartViewModel::addDataPoint(const QString &chartTitle, const QString &parameterLabel, 
                                  double x, double y)
{
    addDataPoint(chartTitle, parameterLabel, x, y, QDateTime::currentDateTime(), QVariant());
}

QtCharts::QChartView* ChartViewModel::getChartView(int index) const
{
    if (index >= 0 && index < m_chartViews.size())
    {
        return m_chartViews.at(index).chartView;
    }
    return nullptr;
}

QtCharts::QChartView* ChartViewModel::getChartView(const QString &title) const
{
    for (const ChartData &chartData : m_chartViews)
    {
        if (chartData.title == title)
        {
            return chartData.chartView;
        }
    }
    return nullptr;
}

QStringList ChartViewModel::chartTitles() const
{
    QStringList titles;
    for (const ChartData &chartData : m_chartViews)
    {
        titles.append(chartData.title);
    }
    return titles;
}

bool ChartViewModel::hasChart(const QString &title) const
{
    return getChartView(title) != nullptr;
}

bool ChartViewModel::hasChart(int index) const
{
    return index >= 0 && index < m_chartViews.size();
}

void ChartViewModel::onNewParameterAdded(BoardParameterSingle* parameter)
{
    auto label = parameter->label();
    
    if (label.isEmpty())
    {
        return;
    }
    
    // Добавляем данные во все существующие графики
    for (ChartData &chartData : m_chartViews)
    {
        auto timeValue = (qreal)parameter->timestamp().toMSecsSinceEpoch() / 1000.0;
        auto dataValue = parameter->value().toDouble();
        addDataPoint(chartData.title, label, timeValue, dataValue, parameter->timestamp(), parameter->value());
    }
    
    //qDebug() << "ChartViewModel: Parameter added:" << label;
}

ChartViewModel::ChartData* ChartViewModel::findChartData(const QString &title)
{
    for (ChartData &chartData : m_chartViews)
    {
        if (chartData.title == title)
        {
            return &chartData;
        }
    }
    return nullptr;
}

ChartViewModel::ChartData* ChartViewModel::findChartData(int index)
{
    if (index >= 0 && index < m_chartViews.size())
    {
        return &m_chartViews[index];
    }
    return nullptr;
}

QtCharts::QLineSeries* ChartViewModel::getOrCreateSeries(ChartData* chartData, const QString &parameterLabel)
{
    if (!chartData)
        return nullptr;
    
    // Проверяем, есть ли уже серия для этого параметра
    if (chartData->seriesMap.contains(parameterLabel))
    {
        return chartData->seriesMap[parameterLabel];
    }
    
    // Создаем новую серию
    QtCharts::QLineSeries* series = new QtCharts::QLineSeries();
    series->setName(parameterLabel);
    
    // Устанавливаем цвет серии
    static QList<QColor> colors = {Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::magenta, Qt::cyan};
    int colorIndex = chartData->seriesMap.size() % colors.size();
    series->setColor(colors[colorIndex]);
    
    // Добавляем серию в график
    chartData->chartView->chart()->addSeries(series);
    series->attachAxis(chartData->xAxis);
    series->attachAxis(chartData->yAxis);
    
    // Сохраняем ссылку на серию
    chartData->seriesMap[parameterLabel] = series;
    
    //qDebug() << "ChartViewModel: Created series for parameter:" << parameterLabel;
    return series;
}

void ChartViewModel::setupChart(ChartData* chartData)
{
    if (!chartData || !chartData->chartView)
        return;
    
    QtCharts::QChart* chart = new QtCharts::QChart();
    chart->setTitle(chartData->title);
    
    // Настраиваем оси
    chartData->xAxis->setTitleText("Время (сек)");
    chartData->xAxis->setMin(0);
    chartData->xAxis->setMax(50);
    
    chartData->yAxis->setTitleText("Значение");
    chartData->yAxis->setMin(-750);
    chartData->yAxis->setMax(750);
    
    chart->addAxis(chartData->xAxis, Qt::AlignBottom);
    chart->addAxis(chartData->yAxis, Qt::AlignLeft);
    
    chartData->chartView->setChart(chart);
    chartData->chartView->setRenderHint(QPainter::Antialiasing);
}
