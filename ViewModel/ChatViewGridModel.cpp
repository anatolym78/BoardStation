#include "ChatViewGridModel.h"
#include "../Model/Parameters/BoardParameterSingle.h"
#include <QDebug>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QDateTime>

ChatViewGridModel::ChatViewGridModel(QObject *parent) : QAbstractListModel(parent)
{
}

void ChatViewGridModel::setPlayer(DataPlayer* dataPlayer)
{
	m_dataPlayer = dataPlayer;

	if (m_playConnection)
	{
		QObject::disconnect(m_playConnection);
	}

	//m_playConnection = connect(dataPlayer, &DataPlayer::parameterPlayed)
}

void ChatViewGridModel::setStorage(BoardParameterHistoryStorage* pStorage)
{
	m_pStorage = pStorage;
}

void ChatViewGridModel::toggleParameter(const QString &label, const QColor &color)
{
	if(hasSeries(label))
	{
		removeSeries(label);
	}
	else
	{
		addSeries(label, color);
	}
}

void ChatViewGridModel::addSeries(const QString &label, const QColor& color)
{
	if (!parameterExistsInHistory(label))
	{
		qWarning() << "ChatViewGridModel: Parameter" << label << "does not exist in history";
		return;
	}
	
	if (hasSeries(label))
	{
		//qDebug() << "ChatViewGridModel: Chart with label" << label << "already exists";
		return;
	}

	beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
	m_charts.append(ChartInfo{ QList<QString>() << label, color, false });
	endInsertRows();

	emit parameterAdded(m_charts.count() - 1, label, color);
}

void ChatViewGridModel::removeSeries(const QString &label)
{
	for(auto i=0;i<m_charts.count();i++)
	{
		if(m_charts[i].series.contains(label))
		{
			m_charts[i].series.removeAll(label);

			if(m_charts[i].series.isEmpty())
			{
				beginRemoveRows(QModelIndex(), i, i);
				m_charts.removeAt(i);
				endRemoveRows();

				return;
			}
		}
	}
}

void ChatViewGridModel::addSeriesToChart(int chartIndex, const QString& label, const QColor& color, QtCharts::QLineSeries* series, QtCharts::QDateTimeAxis* timeAxis, QtCharts::QValueAxis* valueAxis)
{
	if (chartIndex < 0 || chartIndex >= m_charts.count()) return;

	//if (!isSeriesCreated(label)) return;

	series->setColor(color);
	m_charts[chartIndex].seriesMap[label] = QPointer<QtCharts::QLineSeries>(series);
	m_charts[chartIndex].timeAxis = timeAxis;
	m_charts[chartIndex].valueAxis = valueAxis;

	initializeSeries(chartIndex, label);
}

void ChatViewGridModel::initializeSeries(int index, const QString& label)
{
	auto seriesMap = m_charts[index].seriesMap;

	if (!seriesMap.contains(label)) return;

	auto timeAxis = m_charts[index].timeAxis;
	auto valueAxis = m_charts[index].valueAxis;


	auto playerPos = player()->currentPosition();
	auto startPos = playerPos.addMSecs(-minuteIntervalMsec());
	auto endPos = playerPos.addMSecs(minuteIntervalMsec());
	auto parameters = storage()->getParametersInTimeRange(startPos, endPos, label);
	if (parameters.isEmpty()) return;

	auto beginTime = playerPos;
	auto endTime = playerPos.addMSecs(minuteIntervalMsec());

	auto leftTime = parameters.first();
	auto rightTime = parameters.last();


	auto series = seriesMap[label];

	auto minValue = std::numeric_limits<double>::max();
	auto maxValue = std::numeric_limits<double>::min();
	auto minTime = playerPos;
	auto maxTime = playerPos;
	for (auto p : parameters)
	{
		bool ok;
		auto v = p->value().toDouble(&ok);

		auto value = p->value().toReal();

		series->append(p->timestamp().toMSecsSinceEpoch(), value);

		if (value < minValue)
		{
			minValue = value;
		}

		if (value > maxValue)
		{
			maxValue = value;	
		}
	}
	timeAxis->setMin(startPos);
	timeAxis->setMax(endPos);

	valueAxis->setMax(maxValue);
	valueAxis->setMin(minValue);

}

bool ChatViewGridModel::isSeriesCreated(const QString& label) const
{
	for (const auto& chart : m_charts)
	{
		if (chart.seriesMap.contains(label)) return true;
	}

	return false;
}

int ChatViewGridModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0;

	return m_charts.count();
}

QVariant ChatViewGridModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid())
		return {};

	auto cellIndex = index.row();// cellToIndex(index);

	if(cellIndex >= m_charts.count()) 
		return {};

	if(m_charts.isEmpty()) 
		return {};

	if (role == LabelsRole)
	{
		return m_charts[cellIndex].series;
	}

	if (role == LabelRole)
	{
		return m_charts[cellIndex].series.last();
	}

	if (role == SelectionRole)
	{
		return m_charts[cellIndex].isSelected;
	}

	if (role == HoverRole)
	{
		return m_hoverIndex == cellIndex;
	}

	if (role == ColorRole)
	{
		return QVariant::fromValue(m_charts[cellIndex].color);
	}

	if (role == seriesMapRole)
	{
		return QVariant::fromValue(m_charts[cellIndex].seriesMap);
	}


	return m_charts[cellIndex].series.last();
}

bool ChatViewGridModel::parameterExistsInHistory(const QString& label) const
{
	return m_dataPlayer->storage()->containsParameter(label);
}

void ChatViewGridModel::mergeCharts(int movedIndex, int targetIndex)
{
	return; // not realized

	if (movedIndex < 0 || movedIndex >= m_charts.size())
	{
		qWarning() << "ChatViewGridModel: Invalid moved index" << movedIndex;
		return;
	}

	if (targetIndex < 0 || targetIndex >= m_charts.size())
	{
		qWarning() << "ChatViewGridModel: Invalid target index" << targetIndex;
		return;
	}

	beginRemoveRows(QModelIndex(), movedIndex, movedIndex);

	auto seriesLabels = m_charts[movedIndex].series;
	m_charts[targetIndex].series.append(seriesLabels);
	m_charts.removeAt(movedIndex);


	endRemoveRows();
}

void ChatViewGridModel::splitSeries(int chartIndex)
{
	return; // not realized

	if(chartIndex >= 0 && chartIndex < m_charts.count())
	{
		auto series = m_charts[chartIndex].series;

		m_charts.removeAt(chartIndex);
		for(auto s : series)
		{
			m_charts.append(ChartInfo{ QStringList() << s, Qt::darkGray, false });
		}
	}

	beginInsertRows(QModelIndex(), m_charts.size(), m_charts.size());
	endInsertRows();
}

void ChatViewGridModel::clearCharts()
{
	return; // !!!
	if (m_charts.isEmpty())
		return;
		
	beginResetModel();
	m_charts.clear();
	endResetModel();
}

QStringList ChatViewGridModel::getChartSeriesLabels(int chartIndex) const
{
	if(chartIndex < 0 || chartIndex >= rowCount()) return QStringList();

	return m_charts[chartIndex].series;
}

QStringList ChatViewGridModel::chartLabels() const
{
	QStringList labels;
	for(auto& chart : m_charts)
	{
		labels.append(chart.series);
	}

	return labels;
}

bool ChatViewGridModel::hasSeries(const QString &label) const
{
	for(auto& chart : m_charts)
	{
		if(chart.series.contains(label)) return true;
	}

	return false;
}

bool ChatViewGridModel::selectElement(int index, bool keepSelection)
{
	if (keepSelection == false)
	{
		clearSelection();
	}

	if (index >= 0 && index < m_charts.count())
	{
		m_charts[index].isSelected =  !m_charts[index].isSelected;
	}


	emit isCanMergeChartsChanged();

	updateAllCells();

	return true;
}

void ChatViewGridModel::clearSelection()
{
	for (auto& chart : m_charts)
	{
		chart.isSelected = false;
	}

	emit isCanMergeChartsChanged();

	updateAllCells();
}

bool ChatViewGridModel::hoverElement(int index)
{
	m_hoverIndex = index;
	updateAllCells();
	return true;
}

void ChatViewGridModel::clearHover()
{
	m_hoverIndex = -1;

	updateAllCells();
}

bool ChatViewGridModel::isCanMergeCharts() const
{
	auto countSelectedCharts = 0;
	for (auto& chart : m_charts)
	{
		countSelectedCharts += (int)(chart.isSelected == true);
	}

	return countSelectedCharts > 1;
}

void ChatViewGridModel::updateAllCells()
{
	for (auto i = 0; i < m_charts.count(); i++)
	{
		emit dataChanged(this->createIndex(i, 0), this->createIndex(i, 0));
	}
}

int ChatViewGridModel::findChartIndex(const QString &label) const
{ 
	for (auto i=0;i<m_charts.count();i++)
	{
		if (m_charts[i].series.contains(label))
		{
			return i;
		}
	}

	return -1;
}

QHash<int, QByteArray> ChatViewGridModel::roleNames() const
{
	QHash<int, QByteArray> roles;

	roles[LabelsRole] = "labels";
	roles[LabelRole] = "label";
	roles[ChartIndexRole] = "chartIndex";
	roles[DepthRole] = "depth";
	roles[SelectionRole] = "selection";
	roles[HoverRole] = "hover";
	roles[ColorRole] = "parameterColor";
	roles[seriesMapRole] = "seriesMap";

	return roles;
}
