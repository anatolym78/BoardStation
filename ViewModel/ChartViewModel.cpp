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
    case ChartLabelRole:
        return chartData.label;
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
    roles[ChartLabelRole] = "chartLabel";
    roles[ChartIndexRole] = "chartIndex";
    roles[HasDataRole] = "hasData";
    return roles;
}

void ChartViewModel::toggleChart(const QString &label, const QColor &color)
{
    auto index = findChartIndex(label);

    if(index == -1) return;

    if(hasChart(label))
    {
        removeChart(index);
    }
    else
    {
        addChart(label, color);
    }
}

void ChartViewModel::addChart(const QString &label, const QColor& color)
{
    // Проверяем, существует ли параметр в истории
    if (!parameterExistsInHistory(label))
    {
        qWarning() << "ChartViewModel: Parameter" << label << "does not exist in history";
        return;
    }
    
    if (hasChart(label))
    {
        //qDebug() << "ChartViewModel: Chart with label" << label << "already exists";
        return;
    }

    beginInsertRows(QModelIndex(), m_chartViews.size(), m_chartViews.size());
    
    ChartData chartData;
    chartData.label = label;
    chartData.chartView = new QtCharts::QChartView();
    chartData.xAxis = new QtCharts::QValueAxis();
    chartData.yAxis = new QtCharts::QValueAxis();
    
    setupChart(&chartData);
    
    m_chartViews.append(chartData);
    endInsertRows();
    
    //qDebug() << "ChartViewModel: Added chart" << label;
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

void ChartViewModel::addDataPoint(const QString &chartLabel, const QString &parameterLabel, 
                                  double x, double y, const QDateTime &timestamp, const QVariant &value)
{
    ChartData* chartData = findChartData(chartLabel);
    if (!chartData)
    {
        //qWarning() << "ChartViewModel: Chart not found:" << chartLabel;
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
    
    emit chartDataAdded(chartLabel, parameterLabel);
    
    //qDebug() << "ChartViewModel: Added point to chart" << chartLabel << "parameter" << parameterLabel;
}

void ChartViewModel::addDataPoint(const QString &chartLabel, const QString &parameterLabel, 
                                  double x, double y)
{
    addDataPoint(chartLabel, parameterLabel, x, y, QDateTime::currentDateTime(), QVariant());
}

QtCharts::QChartView* ChartViewModel::getChartView(int index) const
{
    if (index >= 0 && index < m_chartViews.size())
    {
        return m_chartViews.at(index).chartView;
    }
    return nullptr;
}

QtCharts::QChartView* ChartViewModel::getChartView(const QString &label) const
{
    for (const ChartData &chartData : m_chartViews)
    {
        if (chartData.label == label)
        {
            return chartData.chartView;
        }
    }
    return nullptr;
}

QStringList ChartViewModel::chartLabels() const
{
    QStringList labels;
    for (const ChartData &chartData : m_chartViews)
    {
        labels.append(chartData.label);
    }
    return labels;
}

bool ChartViewModel::hasChart(const QString &label) const
{
    return getChartView(label) != nullptr;
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
        addDataPoint(chartData.label, label, timeValue, dataValue, parameter->timestamp(), parameter->value());
    }
    
    //qDebug() << "ChartViewModel: Parameter added:" << label;
}

int ChartViewModel::findChartIndex(const QString &label) const
{
    for (auto i=0;i<chartCount();i++)
    {
        if (m_chartViews[i].label == label)
        {
            return i;
        }
    }

    return -1;
}

ChartViewModel::ChartData* ChartViewModel::findChartData(const QString &label)
{
    for (ChartData &chartData : m_chartViews)
    {
        if (chartData.label == label)
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
    chart->setTitle(chartData->label);
    
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

bool ChartViewModel::parameterExistsInHistory(const QString &label) const
{
    if (!m_parametersStorage)
    {
        return false;
    }
    
    // Проверяем, есть ли параметр в хранилище истории
    BoardParameterHistory* history = m_parametersStorage->getParameterHistory(label);
    return history != nullptr;
}

QVariantList ChartViewModel::getSeriesData(const QString &chartLabel, const QString &parameterLabel) const
{
    QVariantList result;
    
    ChartData* chartData = const_cast<ChartViewModel*>(this)->findChartData(chartLabel);
    if (!chartData)
    {
        return result;
    }
    
    QtCharts::QLineSeries* series = chartData->seriesMap.value(parameterLabel);
    if (!series)
    {
        return result;
    }
    
    // Преобразуем точки серии в QVariantList
    for (int i = 0; i < series->count(); ++i)
    {
        QPointF point = series->at(i);
        QVariantMap pointData;
        pointData["x"] = point.x();
        pointData["y"] = point.y();
        result.append(pointData);
    }
    
    return result;
}

QStringList ChartViewModel::getParameterLabels(const QString &chartLabel) const
{
    QStringList result;
    
    ChartData* chartData = const_cast<ChartViewModel*>(this)->findChartData(chartLabel);
    if (!chartData)
    {
        return result;
    }
    
    return chartData->seriesMap.keys();
}

int ChartViewModel::getSeriesPointCount(const QString &chartLabel, const QString &parameterLabel) const
{
    ChartData* chartData = const_cast<ChartViewModel*>(this)->findChartData(chartLabel);
    if (!chartData)
    {
        return 0;
    }
    
    QtCharts::QLineSeries* series = chartData->seriesMap.value(parameterLabel);
    if (!series)
    {
        return 0;
    }
    
    return series->count();
}
