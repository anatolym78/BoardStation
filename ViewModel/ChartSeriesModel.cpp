#include "ChartSeriesModel.h"
#include <QDebug>

ChartSeriesModel::ChartSeriesModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_parametersStorage(nullptr)
{
    //qDebug() << "ChartSeriesModel: Created empty model";
    createColorMap();
}

ChartSeriesModel::ChartSeriesModel(const QStringList &parameterLabels, QObject *parent)
    : QAbstractListModel(parent)
    , m_parametersStorage(nullptr)
{
    addSeries(parameterLabels);
    createColorMap();
    //qDebug() << "ChartSeriesModel: Created with" << parameterLabels.size() << "series";
}

void ChartSeriesModel::createColorMap()
{
	m_colorMap["Altitude"] = Qt::red;
	m_colorMap["Longitude"] = Qt::green;
    m_colorMap["Latitude"] = Qt::blue;
    m_colorMap["Speed"] = Qt::black;
}

void ChartSeriesModel::setParametersStorage(BoardParameterHistoryStorage *storage)
{
    m_parametersStorage = storage;
    if (m_parametersStorage)
    {
        connect(m_parametersStorage, &BoardParameterHistoryStorage::parameterAdded, 
                this, &ChartSeriesModel::onParameterAdded);
		connect(m_parametersStorage, &BoardParameterHistoryStorage::parameterUpdated,
			this, &ChartSeriesModel::onParameterUpdated);
    }
}

int ChartSeriesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_seriesModels.size();
}

QVariant ChartSeriesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_seriesModels.size())
        return QVariant();

    ChartPointsModel *pointsModel = m_seriesModels.at(index.row());
    if (!pointsModel)
        return QVariant();

    switch (role) {
    case ParameterLabelRole:
        return pointsModel->parameterLabel();
    case PointsModelRole:
        return QVariant::fromValue(pointsModel);
    case PointCountRole:
        return pointsModel->pointCount();
    case HasPointsRole:
        return pointsModel->hasPoints();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ChartSeriesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ParameterLabelRole] = "parameterLabel";
    roles[PointsModelRole] = "pointsModel";
    roles[PointCountRole] = "pointCount";
    roles[HasPointsRole] = "hasPoints";
    return roles;
}

void ChartSeriesModel::addSeries(const QString &parameterLabel)
{
    if (hasSeries(parameterLabel)) 
    {
        //qDebug() << "ChartSeriesModel: Series for parameter" << parameterLabel << "already exists";
        return;
    }

    beginInsertRows(QModelIndex(), m_seriesModels.size(), m_seriesModels.size());
    ChartPointsModel *pointsModel = new ChartPointsModel(parameterLabel, this);
    if(m_colorMap.contains(parameterLabel))
    {
        pointsModel->setColor(m_colorMap[parameterLabel]);
    }
    m_seriesModels.append(pointsModel);
    m_parameterLabels.append(parameterLabel);
    endInsertRows();
}

void ChartSeriesModel::addSeries(const QStringList &parameterLabels)
{
    for (const QString &label : parameterLabels) 
    {
        addSeries(label);
    }
}

void ChartSeriesModel::removeSeries(const QString &parameterLabel)
{
    int index = m_parameterLabels.indexOf(parameterLabel);
    if (index >= 0) {
        removeSeries(index);
    } else {
        //qDebug() << "ChartSeriesModel: Series for parameter" << parameterLabel << "not found";
    }
}

void ChartSeriesModel::removeSeries(int index)
{
    if (index < 0 || index >= m_seriesModels.size()) {
        qWarning() << "ChartSeriesModel: Invalid index" << index;
        return;
    }
    
    beginRemoveRows(QModelIndex(), index, index);
    ChartPointsModel *pointsModel = m_seriesModels.takeAt(index);
    QString removedLabel = m_parameterLabels.takeAt(index);
    if (pointsModel) {
        pointsModel->deleteLater();
    }
    endRemoveRows();
    
    //qDebug() << "ChartSeriesModel: Removed series for parameter" << removedLabel;
}

void ChartSeriesModel::clearSeries()
{
    if (m_seriesModels.isEmpty())
        return;
        
    beginResetModel();
    qDeleteAll(m_seriesModels);
    m_seriesModels.clear();
    m_parameterLabels.clear();
    endResetModel();
    
    //qDebug() << "ChartSeriesModel: Cleared all series";
}

void ChartSeriesModel::addPoint(const QString &parameterLabel, const QPointF &point)
{
    ChartPointsModel *pointsModel = getPointsModel(parameterLabel);
    if (pointsModel) 
    {
        pointsModel->addPoint(point);
    }
	else
    {
        //qWarning() << "ChartSeriesModel: Points model not found for parameter" << parameterLabel;
    }
}

void ChartSeriesModel::addPoint(const QString &parameterLabel, double x, double y)
{
    addPoint(parameterLabel, QPointF(x, y));
}

void ChartSeriesModel::addPoint(const QString &parameterLabel, double x, double y, const QDateTime &timestamp, const QVariant &value)
{
    ChartPointsModel *pointsModel = getPointsModel(parameterLabel);
    if (pointsModel) 
    {
        pointsModel->addPoint(x, y, timestamp, value);
    }
	else 
    {
        //qWarning() << "ChartSeriesModel: Points model not found for parameter" << parameterLabel;
    }
}

ChartPointsModel* ChartSeriesModel::getPointsModel(const QString &parameterLabel) const
{
    int index = m_parameterLabels.indexOf(parameterLabel);
    if (index >= 0 && index < m_seriesModels.size()) {
        return m_seriesModels.at(index);
    }
    return nullptr;
}

ChartPointsModel* ChartSeriesModel::getPointsModel(int index) const
{
    if (index >= 0 && index < m_seriesModels.size()) {
        return m_seriesModels.at(index);
    }
    return nullptr;
}

QStringList ChartSeriesModel::parameterLabels() const
{
    return m_parameterLabels;
}

bool ChartSeriesModel::hasSeries(const QString &parameterLabel) const
{
    return m_parameterLabels.contains(parameterLabel);
}

bool ChartSeriesModel::hasSeries(int index) const
{
    return index >= 0 && index < m_seriesModels.size();
}

void ChartSeriesModel::handleParameterAdded(const QString &label)
{
    // Если у нас есть серия для этого параметра, добавляем новую точку
    if (hasSeries(label) && m_parametersStorage) 
    {
        BoardParameterHistory *param = m_parametersStorage->getParameterHistory(label);
        if (param && param->hasValues())
        {
            BoardParameterValue *lastValue = param->lastValue();
            if (lastValue) 
            {
                double timeValue = lastValue->timestamp().toMSecsSinceEpoch() / 1000.0;
                double dataValue = lastValue->value().toDouble();
                addPoint(label, timeValue, dataValue, lastValue->timestamp(), lastValue->value());
            }
        }
    }
    
    //qDebug() << "ChartSeriesModel: Parameter added:" << label;
}

void ChartSeriesModel::onParameterAdded(const QString &label)
{
    handleParameterAdded(label);

    //emit parameterValueAdded(label);
}

void ChartSeriesModel::onParameterUpdated(const QString& label)
{
    emit parameterValueAdded(label);
}

void ChartSeriesModel::copySeriesFrom(ChartSeriesModel* sourceModel)
{
    if (!sourceModel) 
    {
        qWarning() << "ChartSeriesModel: Source model is null";
        return;
    }
    
    // Получаем все метки параметров из исходной модели
    QStringList sourceLabels = sourceModel->parameterLabels();
    
    for (const QString &label : sourceLabels) 
    {
        // Добавляем серию если её ещё нет
        if (!hasSeries(label)) 
        {
            addSeries(label);
        }
        
        // Получаем модели точек
        ChartPointsModel *sourcePointsModel = sourceModel->getPointsModel(label);
        ChartPointsModel *targetPointsModel = getPointsModel(label);
        
        if (sourcePointsModel && targetPointsModel) 
        {
            // Копируем все точки из исходной модели
            QList<QPointF> sourcePoints = sourcePointsModel->points();
            QList<QDateTime> sourceTimestamps = sourcePointsModel->timestamps();
            QList<QVariant> sourceValues = sourcePointsModel->values();
            
            for (int i = 0; i < sourcePoints.size(); ++i) 
            {
                QPointF point = sourcePoints.at(i);
                QDateTime timestamp = (i < sourceTimestamps.size()) ? sourceTimestamps.at(i) : QDateTime::currentDateTime();
                QVariant value = (i < sourceValues.size()) ? sourceValues.at(i) : QVariant();
                
                targetPointsModel->addPoint(point.x(), point.y(), timestamp, value);
            }
        }
    }
    
    //qDebug() << "ChartSeriesModel: Copied" << sourceLabels.size() << "series from source model";
}