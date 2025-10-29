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

	m_playConnection = connect(dataPlayer, &DataPlayer::parameterPlayed, 
		this, &ChatViewGridModel::onParameterPlayed);
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
	auto chartIndex = findChartIndex(label);
	if (chartIndex == -1) return;

	// remove sereis from qml
	emit parameterNeedToRemove(chartIndex, label);

	// remove series from cpp model
	m_charts[chartIndex].seriesMap.remove(label);

	// remove empty chart
	if (m_charts[chartIndex].seriesMap.isEmpty())
	{
		this->beginRemoveRows(QModelIndex(), chartIndex, chartIndex);
		m_charts.removeAt(chartIndex);
		this->endRemoveRows();
	}
	else
	{
		updateValueAxisRange(chartIndex);
	}
}

void ChatViewGridModel::addSeriesToChart(int chartIndex, const QString& label, const QColor& color, QtCharts::QLineSeries* series, QtCharts::QDateTimeAxis* timeAxis, QtCharts::QValueAxis* valueAxis)
{
	if (chartIndex < 0 || chartIndex >= m_charts.count()) return;

	series->setColor(color);
	m_charts[chartIndex].seriesMap[label] = QPointer<QtCharts::QLineSeries>(series);
	m_charts[chartIndex].timeAxis = timeAxis;
	m_charts[chartIndex].valueAxis = valueAxis;

	fillSeries(label, color, true);
}

void ChatViewGridModel::moveSeriesToChart(int targetChartIndex, const QString& label, QtCharts::QLineSeries* series)
{
	auto color = labelColor(label);

	// add series to target chart
	m_charts[targetChartIndex].seriesMap[label] = QPointer<QtCharts::QLineSeries>(series);

	fillSeries(label, color, false);

	// remove moved series from chart
	for (auto index = 0; index < m_charts.count(); index++)
	{
		if(index == targetChartIndex) continue;

		auto& chart = m_charts[index];
		if (chart.seriesMap.contains(label))
		{
			chart.seriesMap.remove(label);

			break;
		}
	}

	removeEmptyCharts();
}

int ChatViewGridModel::countEmptyCharts() const
{
	return std::count_if(m_charts.begin(), m_charts.end(), [](ChartInfo chartInfo)
		{
			return chartInfo.seriesMap.isEmpty();
		});
}

int ChatViewGridModel::firstEmptyChart() const
{
	for (auto i = 0; i < m_charts.count(); i++)
	{
		if (m_charts[i].seriesMap.isEmpty()) return i;
	}

	return -1;
}

void ChatViewGridModel::removeEmptyCharts()
{
	while (countEmptyCharts() > 0)
	{
		int indexToRemove = firstEmptyChart();
		this->beginRemoveRows(QModelIndex(), indexToRemove, indexToRemove);
		m_charts.removeAt(indexToRemove);
		this->endRemoveRows();
	}
}

void ChatViewGridModel::fillSeries(const QString& label, QColor color, bool isInitialFill)
{
	auto chartIndex = findChartIndex(label);
	if (chartIndex == -1) return;

	auto seriesMap = m_charts[chartIndex].seriesMap;
	auto timeAxis = m_charts[chartIndex].timeAxis;
	auto valueAxis = m_charts[chartIndex].valueAxis;

	// extract parameters in [-1, 1] minute range
	auto playerTime = player()->currentPosition();
	auto startPos = playerTime.addMSecs(-minuteIntervalMsec());
	auto endPos = playerTime.addMSecs(minuteIntervalMsec());
	auto parameters = storage()->getParametersInTimeRange(startPos, endPos, label);

	if (parameters.isEmpty()) return;

	// fill sereis with extracted parameters
	auto series = seriesMap[label];
	for (auto p : parameters)
	{
		bool ok;
		auto value = p->value().toDouble(&ok);

		if (ok)
		{
			series->append(p->timestamp().toMSecsSinceEpoch(), value);
		}
	}

	// set color
	series->setColor(color);

	// calc time range
	if (isInitialFill)
	{
		QDateTime firstTime = playerTime;	// ��������� ����� ��� �����
		QDateTime firstParameterTime = parameters.first()->timestamp();
		QDateTime lastParameterTime = parameters.last()->timestamp();
		for (auto p : parameters)
		{
			// ���� �� ������ ������ ��� ������, �� ��� ��������� �����
			if (p->timestamp().msecsTo(playerTime) < minuteIntervalMsec()/2)
			{
				firstTime = p->timestamp();

				break;
			}

			// ���� �� ����� ����� ������ ������, �� ��� ��������� �����
			if (p->timestamp().msecsTo(lastParameterTime) >= minuteIntervalMsec())
			{
				firstTime = p->timestamp();

				break;
			}
		}

		timeAxis->setMin(firstTime);
		timeAxis->setMax(firstTime.addMSecs(minuteIntervalMsec()));
	}

	updateValueAxisRange(chartIndex);
}

QColor ChatViewGridModel::labelColor(QString label)
{
	auto chartIndex = findChartIndex(label);

	if (chartIndex == -1) return QColor().black();

	return m_charts[chartIndex].seriesMap[label]->color();
}

void ChatViewGridModel::updateValueAxisRange(int chartIndex)
{
	if (chartIndex < 0 || chartIndex >= m_charts.count())
	{
		return;
	}

	auto& chart = m_charts[chartIndex];
	auto valueAxis = chart.valueAxis;

	if (valueAxis == nullptr)
	{
		return;
	}

	if (chart.seriesMap.isEmpty())
	{
		valueAxis->setMin(0);
		valueAxis->setMax(10);
		return;
	}

	double minValue = std::numeric_limits<double>::max();
	double maxValue = std::numeric_limits<double>::min();
	bool hasPoints = false;

	for (auto series : chart.seriesMap)
	{
		if (series)
		{
			for (const auto& point : series->points())
			{
				minValue = std::min(minValue, point.y());
				maxValue = std::max(maxValue, point.y());
				hasPoints = true;
			}
		}
	}

	if (hasPoints)
	{
		auto range = maxValue - minValue;
		if (qFuzzyCompare(range, 0.0))
		{
			valueAxis->setMin(minValue - 1);
			valueAxis->setMax(maxValue + 1);
		}
		else
		{
			valueAxis->setMin(minValue - range * 0.1);
			valueAxis->setMax(maxValue + range * 0.1);
		}
	}
	else
	{
		valueAxis->setMin(0);
		valueAxis->setMax(10);
	}
}

void ChatViewGridModel::onParameterPlayed(BoardParameterSingle* parameter, bool isBackPlaying)
{
	if (parameter == nullptr) return;

	auto parameterLabel = parameter->label();
	auto parameterTime = parameter->timestamp();
	auto parameterValue = parameter->value().toDouble();

	auto chartIndex = findChartIndex(parameterLabel);

	if (chartIndex == -1) return;

	auto timeAxis = m_charts[chartIndex].timeAxis;
	auto valueAxis = m_charts[chartIndex].valueAxis;

	if (timeAxis == nullptr || valueAxis == nullptr) return;

	auto series = m_charts[chartIndex].seriesMap[parameterLabel];

	auto countPoints = series->count();

	if (countPoints == 0)
	{
		series->append(QPointF(parameterTime.toMSecsSinceEpoch(), parameterValue));

		return;
	}

	auto seriesLeftTime = series->at(0).x();
	auto seriesRightTime = series->at(countPoints - 1).x();

	auto timeRangeChaned = false;
	if (isBackPlaying)
	{
		if (parameterTime.toMSecsSinceEpoch() < seriesLeftTime)
		{
			series->insert(0, QPointF(parameterTime.toMSecsSinceEpoch(), parameterValue));
			seriesLeftTime = parameterTime.toMSecsSinceEpoch();
			timeRangeChaned = true;
		}

		// remove right points
		bool intervalReached = false;
		while (!intervalReached)
		{
			auto countSeries = series->count();
			if (countSeries == 0) break;

			auto pointTime = series->at(countSeries - 1).x();
			if (pointTime - seriesLeftTime > minuteIntervalMsec())
			{
				series->remove(countSeries - 1);
			}
			else
			{
				intervalReached = true;
			}
		}
	}
	else
	{
		if (parameterTime.toMSecsSinceEpoch() > seriesRightTime)
		{
			series->append(QPointF(parameterTime.toMSecsSinceEpoch(), parameterValue));
			seriesRightTime = parameterTime.toMSecsSinceEpoch();

			timeRangeChaned = true;
		}

		// remove left points
		bool intervalReached = false;
		while (!intervalReached)
		{
			if (series->count() == 0) break;

			auto pointTime = series->at(0).x();
			if (seriesRightTime - pointTime > minuteIntervalMsec())
			{
				series->remove(0);
			}
			else
			{
				intervalReached = true;
			}
		}
	}

	if (timeRangeChaned && series->count() > 0)
	{
		auto beginTime = series->at(0).x();
		auto endTime = series->at(series->count() - 1).x();
		timeAxis->setMin(QDateTime::fromMSecsSinceEpoch(beginTime));
		timeAxis->setMax(QDateTime::fromMSecsSinceEpoch(endTime));

		auto minValue = valueAxis->min();
		auto maxValue = valueAxis->max();

		valueAxis->setMin(std::min(minValue, parameterValue));
		valueAxis->setMax(std::max(maxValue, parameterValue));
	}
}

void ChatViewGridModel::mergeSelectedCharts()
{
	auto indices = selectedIndices();

	auto targetChartIndex = indices.first();
	auto indicesToRemove = QList(indices.begin() + 1, indices.end());

	QStringList labelsToMove;
	for (auto index : indicesToRemove)
	{
		auto chart = m_charts[index];
		labelsToMove.append(chart.seriesMap.keys());
	}

	emit parametersNeedToMove(targetChartIndex, labelsToMove);

	clearSelection();
}

QList<int> ChatViewGridModel::selectedIndices() const
{
	QList<int> indices;
	for (auto index = 0; index < m_charts.count(); index++)
	{
		if (m_charts[index].isSelected)
		{
			indices.append(index);
		}
	}

	return indices;
}

int ChatViewGridModel::countSelectedIndices() const
{
	return std::count_if(m_charts.begin(), m_charts.end(), [](ChartInfo chart) { return chart.isSelected; });
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
		if(chart.seriesMap.contains(label)) return true;
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
		if (m_charts[i].seriesMap.contains(label))
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
