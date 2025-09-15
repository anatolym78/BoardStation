#include "ChartsListModel.h"
#include <QDebug>

ChartsListModel::ChartsListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_parametersStorage(nullptr)
{
    //qDebug() << "ChartsListModel: Created empty model";
}

ChartsListModel::ChartsListModel(BoardParametersStorage *parametersStorage, QObject *parent)
    : QAbstractListModel(parent)
    , m_parametersStorage(parametersStorage)
{
    setParametersStorage(parametersStorage);

	//qDebug() << "ChartsListModel: Created with parameters storage";
}

void ChartsListModel::setParametersStorage(BoardParametersStorage* storage)
{
	m_parametersStorage = storage;
	if (m_parametersStorage)
	{
		connect(m_parametersStorage, 
            &BoardParametersStorage::parameterAdded,
			this, &ChartsListModel::onParameterAdded);

		connect(m_parametersStorage, 
            &BoardParametersStorage::parameterUpdated,
			this, &ChartsListModel::onParameterUpdated);
	}
}

void ChartsListModel::onParameterAdded(const QString& label)
{
	// Создаем серию для нового параметра
	addSeries(label);

	// Передаем сигнал всем моделям серий
	for (ChartSeriesModel* seriesModel : m_chartsModels)
	{
		if (seriesModel)
		{
			seriesModel->handleParameterAdded(label);
		}
	}
}

void ChartsListModel::onParameterUpdated(const QString& label)
{
	// Получаем серию для обновленного параметра
	ChartSeriesModel* seriesModel = getSeriesModel(label);
	if (seriesModel && m_parametersStorage)
	{
		BoardParameter* param = m_parametersStorage->getParameter(label);
		if (param && param->hasValues())
		{
			BoardParameterValue* lastValue = param->lastValue();
			if (lastValue)
			{
				auto timeValue = (qreal)lastValue->timestamp().toMSecsSinceEpoch() / 1000.0;
                auto dataValue = lastValue->value().toDouble();
				seriesModel->addPoint(label, timeValue, dataValue, lastValue->timestamp(), lastValue->value());
			}
		}
	}
}

ChartSeriesModel* ChartsListModel::addSeries(const QString &parameterLabel)
{
    if (hasSeries(parameterLabel))
    {
        return getSeriesModel(parameterLabel);
    }

    beginInsertRows(QModelIndex(), m_chartsModels.size(), m_chartsModels.size());
    auto seriesModel = new ChartSeriesModel(this);
    seriesModel->setParametersStorage(m_parametersStorage);
    seriesModel->addSeries(parameterLabel);
    m_chartsModels.append(seriesModel);
    endInsertRows();

    return seriesModel;
}

void ChartsListModel::addSeries(const QStringList &parameterLabels)
{
    for (const QString &label : parameterLabels)
    {
        addSeries(label);
    }
}

void ChartsListModel::removeSeries(const QString &parameterLabel)
{
    for (int i = 0; i < m_chartsModels.size(); ++i)
    {
        ChartSeriesModel *seriesModel = m_chartsModels.at(i);
        if (seriesModel && seriesModel->hasSeries(parameterLabel))
        {
            removeSeries(i);
            return;
        }
    }
    //qDebug() << "ChartsListModel: Series for parameter" << parameterLabel << "not found";
}

void ChartsListModel::removeSeries(int index)
{
    if (index < 0 || index >= m_chartsModels.size()) 
    {
        qWarning() << "ChartsListModel: Invalid index" << index;
        return;
    }
    
    beginRemoveRows(QModelIndex(), index, index);
    ChartSeriesModel *seriesModel = m_chartsModels.takeAt(index);
    if (seriesModel)
    {
        seriesModel->deleteLater();
    }
    endRemoveRows();
    
    //qDebug() << "ChartsListModel: Removed series at index" << index;
}

void ChartsListModel::clearSeries()
{
    if (m_chartsModels.isEmpty())
        return;
        
    beginResetModel();
    qDeleteAll(m_chartsModels);
    m_chartsModels.clear();
    endResetModel();
    
    //qDebug() << "ChartsListModel: Cleared all series";
}


ChartSeriesModel* ChartsListModel::getSeriesModel(const QString &parameterLabel) const
{
    for (ChartSeriesModel *seriesModel : m_chartsModels)
    {
        if (seriesModel && seriesModel->hasSeries(parameterLabel))
        {
            return seriesModel;
        }
    }
    return nullptr;
}

ChartSeriesModel* ChartsListModel::getSeriesModel(int index) const
{
    if (index >= 0 && index < m_chartsModels.size())
    {
        return m_chartsModels.at(index);
    }
    return nullptr;
}

QStringList ChartsListModel::parameterLabels() const
{
    QStringList allLabels;
    for (ChartSeriesModel *seriesModel : m_chartsModels)
    {
        if (seriesModel)
        {
            allLabels.append(seriesModel->parameterLabels());
        }
    }
    return allLabels;
}

bool ChartsListModel::hasSeries(const QString &parameterLabel) const
{
    return getSeriesModel(parameterLabel) != nullptr;
}

bool ChartsListModel::hasSeries(int index) const
{
    return index >= 0 && index < m_chartsModels.size();
}

QVariantList ChartsListModel::getSeriesData(const QString &parameterLabel) const
{
    QVariantList result;
    
    ChartSeriesModel *seriesModel = getSeriesModel(parameterLabel);
    if (!seriesModel) 
    {
        qWarning() << "ChartsListModel: Series for parameter" << parameterLabel << "not found";
        return result;
    }
    
    // Получаем данные серии
    ChartPointsModel *pointsModel = seriesModel->getPointsModel(parameterLabel);
    if (pointsModel)
    {
        QVariantMap seriesData;
        seriesData["parameterLabel"] = parameterLabel;
        seriesData["pointCount"] = pointsModel->pointCount();
        seriesData["hasPoints"] = pointsModel->hasPoints();
        
        // Добавляем точки
        QVariantList points;
        for (int i = 0; i < pointsModel->pointCount(); ++i) 
        {
            QVariantMap point;
            point["x"] = pointsModel->data(pointsModel->index(i, 0), ChartPointsModel::XRole);
            point["y"] = pointsModel->data(pointsModel->index(i, 0), ChartPointsModel::YRole);
            points.append(point);
        }
        seriesData["points"] = points;
        
        result.append(seriesData);
    }
    
    return result;
}


int ChartsListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_chartsModels.size();
}

QVariant ChartsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_chartsModels.size())
        return QVariant();

    ChartSeriesModel *seriesModel = m_chartsModels.at(index.row());
    if (!seriesModel)
        return QVariant();

    switch (role)
	{
    case SeriesModelRole:
        return QVariant::fromValue(seriesModel);
    case HasSeriesRole:
        return seriesModel->seriesCount() > 0;
    case Name:
        return "Chart";
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ChartsListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[SeriesModelRole] = "seriesModel";
    roles[HasSeriesRole] = "hasSeries";
    roles[Name] = "name";
    return roles;
}
