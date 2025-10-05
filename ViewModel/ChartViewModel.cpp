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

    return;

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

    return m_series.count();
}

QVariant ChartViewModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return {};

    if(index.row() >= m_series.count()) return {};

    if(m_series.isEmpty()) return {};

    if(role == DepthRole)
    {
        return m_depths[index.row()];
    }

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
    roles[DepthRole] = "depth";
    return roles;
}

bool ChartViewModel::toggleParameter(const QString &label, const QColor &color)
{
    if(hasSeries(label))
    {
        removeLabel(label);

        return false;
    }
    else
    {
        addChart(label, color);

        return true;
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
    
    if (hasSeries(label))
    {
        //qDebug() << "ChartViewModel: Chart with label" << label << "already exists";
        return;
    }

    beginInsertRows(QModelIndex(), m_series.size(), m_series.size());
    m_series.append(QList<QString>() << label);
    resetDepths();
    endInsertRows();

    emit parameterAdded(label, color);
}

void ChartViewModel::removeChart(int index)
{
    if (index < 0 || index >= m_series.size())
    {
        qWarning() << "ChartViewModel: Invalid index" << index;
        return;
    }
    
    beginRemoveRows(QModelIndex(), index, index);

    m_series.removeAt(index);

    endRemoveRows();
}

void ChartViewModel::removeLabel(const QString &label)
{
    for(auto i=0;i<m_series.count();i++)
    {
        if(m_series[i].contains(label))
        {
            m_series[i].removeAll(label);

            if(m_series[i].isEmpty())
            {
                removeChart(i);

                return;
            }
        }
    }
}

void ChartViewModel::mergeCharts(int movedIndex, int targetIndex)
{
    if (movedIndex < 0 || movedIndex >= m_series.size())
    {
        qWarning() << "ChartViewModel: Invalid moved index" << movedIndex;
        return;
    }

    if (targetIndex < 0 || targetIndex >= m_series.size())
    {
        qWarning() << "ChartViewModel: Invalid target index" << targetIndex;
        return;
    }

    beginRemoveRows(QModelIndex(), movedIndex, movedIndex);

    auto seriesLabels = m_series[movedIndex];
    m_series[targetIndex].append(seriesLabels);
    m_series.removeAt(movedIndex);

    resetDepths();

    endRemoveRows();
}

void ChartViewModel::splitSeries(int chartIndex)
{
    if(chartIndex >= 0 && chartIndex < m_series.count())
    {
        auto series = m_series[chartIndex];

        m_series.removeAt(chartIndex);
        for(auto s : series)
        {
            m_series.append(QStringList()<<s);
        }
    }

    beginInsertRows(QModelIndex(), m_series.size(), m_series.size());
    resetDepths();
    endInsertRows();
}

void ChartViewModel::clearCharts()
{
    if (m_series.isEmpty())
        return;
        
    beginResetModel();
    m_series.clear();
    endResetModel();
}

QStringList ChartViewModel::getChartSeriesLabels(int chartIndex) const
{
    if(chartIndex < 0 || chartIndex >= rowCount()) return QStringList();

    return m_series[chartIndex];
}

void ChartViewModel::reorderChartsBeforeDrag(int dragIndex)
{
    resetDepths();

    m_depths[dragIndex] = 0;

    auto topLeft = this->index(0, 0);
    auto bottomRight = this->index(rowCount() - 1, 0);

    emit dataChanged(topLeft, bottomRight);
}

void ChartViewModel::resetDepths()
{
    m_depths.clear();

    for(auto i=0;i<m_series.count();i++)
    {
        m_depths.append(i + 1);
    }
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
    for(auto s : m_series)
    {
        labels.append(s);
    }

    return labels;
}

bool ChartViewModel::hasSeries(const QString &label) const
{
    for(auto s : m_series)
    {
        if(s.contains(label)) return true;
    }

    return false;
}

void ChartViewModel::onNewParameterAdded(BoardParameterSingle* parameter)
{
    auto label = parameter->label();
    
    if (label.isEmpty())
    {
        return;
    }

    if(!hasSeries(label)) return;

    auto timeValue = (qreal)parameter->timestamp().toMSecsSinceEpoch() / 1000.0;
    auto dataValue = parameter->value().toDouble();

    emit newParameterValueAdded(label, dataValue, timeValue);
}

int ChartViewModel::findChartIndex(const QString &label) const
{ 
    for (auto i=0;i<countSeries();i++)
    {
        if (m_series[i].contains(label))
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

