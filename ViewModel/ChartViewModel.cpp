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

bool ChartViewModel::hasLabel(const QString &label) const
{

}

int ChartViewModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_series.count();

    return m_charts.size();
}

QVariant ChartViewModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return {};

    if(index.row() >= m_series.count()) return {};

    if(m_series.isEmpty()) return {};

    return m_series[index.row()].last();

    // if (!index.isValid() || index.row() >= m_charts.size())
    //     return QVariant();


    // QtCharts::QChart* chart = m_charts.at(index.row());

    // switch (role) {
    // case ChartRole:
    //     return QVariant::fromValue(chart);
    // case ChartLabelRole:
    //     return chart->title();
    // case ChartIndexRole:
    //     return index.row();
    // case HasDataRole:
    //     return !chart->series().isEmpty();
    // default:
    //     return QVariant();
    // }
}

QHash<int, QByteArray> ChartViewModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ChartRole] = "chart";
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

    beginInsertRows(QModelIndex(), m_charts.size(), m_charts.size());
    m_series.append(QList<QString>() << label);
    endInsertRows();

    return;

    beginInsertRows(QModelIndex(), m_charts.size(), m_charts.size());
    
    QtCharts::QChart* chart = new QtCharts::QChart();
    setupChart(chart, label);
    
    m_charts.append(chart);
    endInsertRows();
    
    //qDebug() << "ChartViewModel: Added chart" << label;
}

void ChartViewModel::removeChart(int index)
{
    if (index < 0 || index >= m_charts.size())
    {
        qWarning() << "ChartViewModel: Invalid index" << index;
        return;
    }
    
    beginRemoveRows(QModelIndex(), index, index);

    m_series.removeAt(index);

    endRemoveRows();

    return;
    beginRemoveRows(QModelIndex(), index, index);
    QtCharts::QChart* chart = m_charts.takeAt(index);
    
    if (chart)
    {
        chart->deleteLater();
    }
    
    endRemoveRows();
    
    //qDebug() << "ChartViewModel: Removed chart at index" << index;
}

void ChartViewModel::clearCharts()
{
    if (m_charts.isEmpty())
        return;
        
    beginResetModel();
    
    for (QtCharts::QChart* chart : m_charts)
    {
        if (chart)
        {
            chart->deleteLater();
        }
    }
    
    m_charts.clear();
    endResetModel();
    
    //qDebug() << "ChartViewModel: Cleared all charts";
}

void ChartViewModel::addDataPoint(const QString &chartLabel, const QString &parameterLabel, 
                                  double x, double y, const QDateTime &timestamp, const QVariant &value)
{
    QtCharts::QChart* chart = findChart(chartLabel);
    if (!chart)
    {
        //qWarning() << "ChartViewModel: Chart not found:" << chartLabel;
        return;
    }
    
    QtCharts::QLineSeries* series = getOrCreateSeries(chart, parameterLabel);
    if (!series)
    {
        //qWarning() << "ChartViewModel: Failed to create series for parameter:" << parameterLabel;
        return;
    }
    
    // Добавляем точку в серию
    series->append(x, y);
    
    // Ограничиваем количество точек для производительности
    if (series->count() > 100)
    {
        series->removePoints(0, 1);
    }
    
    // Обновляем оси
    QtCharts::QValueAxis* xAxis = qobject_cast<QtCharts::QValueAxis*>(chart->axes(Qt::Horizontal).first());
    if (xAxis)
    {
        xAxis->setMin(x - 50);
        xAxis->setMax(x + 10);
    }
    
    emit chartDataAdded(chartLabel, parameterLabel);
    
    //qDebug() << "ChartViewModel: Added point to chart" << chartLabel << "parameter" << parameterLabel;
}

void ChartViewModel::addDataPoint(const QString &chartLabel, const QString &parameterLabel, 
                                  double x, double y)
{
    addDataPoint(chartLabel, parameterLabel, x, y, QDateTime::currentDateTime(), QVariant());
}

QtCharts::QChart* ChartViewModel::getChart(int index) const
{
    if (index >= 0 && index < m_charts.size())
    {
        return m_charts.at(index);
    }
    return nullptr;
}

QtCharts::QChart* ChartViewModel::getChart(const QString &label) const
{
    for (QtCharts::QChart* chart : m_charts)
    {
        if (chart->title() == label)
        {
            return chart;
        }
    }
    return nullptr;
}

QStringList ChartViewModel::chartLabels() const
{
    QStringList labels;
    for (QtCharts::QChart* chart : m_charts)
    {
        labels.append(chart->title());
    }
    return labels;
}

bool ChartViewModel::hasChart(const QString &label) const
{
    for(auto s : m_series)
    {
        if(s.contains(label)) return true;
    }

    return false;

    return getChart(label) != nullptr;
}

bool ChartViewModel::hasChart(int index) const
{
    return index >= 0 && index < m_charts.size();
}

void ChartViewModel::onNewParameterAdded(BoardParameterSingle* parameter)
{
    auto label = parameter->label();
    
    if (label.isEmpty())
    {
        return;
    }

    if(!hasChart(label)) return;

    auto timeValue = (qreal)parameter->timestamp().toMSecsSinceEpoch() / 1000.0;
    auto dataValue = parameter->value().toDouble();

    emit newParameterValueAdded(label, dataValue, timeValue);

    return;
    
    // // Добавляем данные во все существующие графики
    // for (QtCharts::QChart* chart : m_charts)
    // {
    //     auto timeValue = (qreal)parameter->timestamp().toMSecsSinceEpoch() / 1000.0;
    //     auto dataValue = parameter->value().toDouble();
    //     addDataPoint(chart->title(), label, timeValue, dataValue, parameter->timestamp(), parameter->value());
    // }
    
    //qDebug() << "ChartViewModel: Parameter added:" << label;
}

int ChartViewModel::findChartIndex(const QString &label) const
{
    for (auto i=0;i<chartCount();i++)
    {
        if (m_charts[i]->title() == label)
        {
            return i;
        }
    }

    return -1;
}

QtCharts::QChart* ChartViewModel::findChart(const QString &label)
{
    for (QtCharts::QChart* chart : m_charts)
    {
        if (chart->title() == label)
        {
            return chart;
        }
    }
    return nullptr;
}

QtCharts::QLineSeries* ChartViewModel::getOrCreateSeries(QtCharts::QChart* chart, const QString &parameterLabel)
{
    if (!chart)
        return nullptr;
    
    // Проверяем, есть ли уже серия для этого параметра
    for (QtCharts::QAbstractSeries* series : chart->series())
    {
        if (series->name() == parameterLabel)
        {
            return qobject_cast<QtCharts::QLineSeries*>(series);
        }
    }
    
    // Создаем новую серию
    QtCharts::QLineSeries* series = new QtCharts::QLineSeries();
    series->setName(parameterLabel);
    
    // Устанавливаем цвет серии
    static QList<QColor> colors = {Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::magenta, Qt::cyan};
    int colorIndex = chart->series().size() % colors.size();
    series->setColor(colors[colorIndex]);
    
    // Добавляем серию в график
    chart->addSeries(series);
    
    // Получаем оси из графика
    QList<QtCharts::QAbstractAxis*> axes = chart->axes();
    QtCharts::QValueAxis* xAxis = nullptr;
    QtCharts::QValueAxis* yAxis = nullptr;
    
    for (QtCharts::QAbstractAxis* axis : axes)
    {
        if (axis->orientation() == Qt::Horizontal)
            xAxis = qobject_cast<QtCharts::QValueAxis*>(axis);
        else if (axis->orientation() == Qt::Vertical)
            yAxis = qobject_cast<QtCharts::QValueAxis*>(axis);
    }
    
    if (xAxis && yAxis)
    {
        series->attachAxis(xAxis);
        series->attachAxis(yAxis);
    }
    
    //qDebug() << "ChartViewModel: Created series for parameter:" << parameterLabel;
    return series;
}

void ChartViewModel::setupChart(QtCharts::QChart* chart, const QString &label)
{
    if (!chart)
        return;
    
    chart->setTitle(label);
    
    // Создаем и настраиваем оси
    QtCharts::QValueAxis* xAxis = new QtCharts::QValueAxis();
    xAxis->setTitleText("Время (сек)");
    xAxis->setMin(0);
    xAxis->setMax(50);
    
    QtCharts::QValueAxis* yAxis = new QtCharts::QValueAxis();
    yAxis->setTitleText("Значение");
    yAxis->setMin(-750);
    yAxis->setMax(750);
    
    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);
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
    
    QtCharts::QChart* chart = const_cast<ChartViewModel*>(this)->findChart(chartLabel);
    if (!chart)
    {
        return result;
    }
    
    // Ищем серию по имени
    QtCharts::QLineSeries* series = nullptr;
    for (QtCharts::QAbstractSeries* s : chart->series())
    {
        if (s->name() == parameterLabel)
        {
            series = qobject_cast<QtCharts::QLineSeries*>(s);
            break;
        }
    }
    
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
    
    QtCharts::QChart* chart = const_cast<ChartViewModel*>(this)->findChart(chartLabel);
    if (!chart)
    {
        return result;
    }
    
    for (QtCharts::QAbstractSeries* series : chart->series())
    {
        result.append(series->name());
    }
    
    return result;
}

int ChartViewModel::getSeriesPointCount(const QString &chartLabel, const QString &parameterLabel) const
{
    QtCharts::QChart* chart = const_cast<ChartViewModel*>(this)->findChart(chartLabel);
    if (!chart)
    {
        return 0;
    }
    
    // Ищем серию по имени
    for (QtCharts::QAbstractSeries* s : chart->series())
    {
        if (s->name() == parameterLabel)
        {
            QtCharts::QLineSeries* series = qobject_cast<QtCharts::QLineSeries*>(s);
            if (series)
            {
                return series->count();
            }
        }
    }
    
    return 0;
}

