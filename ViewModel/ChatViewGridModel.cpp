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
}

void ChatViewGridModel::addSeriesToChart(int chartIndex, const QString& label, QtCharts::QLineSeries* series)
{
	if (chartIndex >= 0 && chartIndex < m_charts.count() && series) 
	{
		m_charts[chartIndex].seriesMap[label] = QPointer<QtCharts::QLineSeries>(series);
		// Опционально: подключить сигналы, если нужно (например, series->pointsReplaced.connect(...))
	}
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


	return m_charts[cellIndex].series.last();
}

bool ChatViewGridModel::toggleParameter(const QString &label, const QColor &color)
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

void ChatViewGridModel::addChart(const QString &label, const QColor& color)
{
	// Проверяем, существует ли параметр в истории
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

	emit parameterAdded(label, color);
}

void ChatViewGridModel::removeChart(int index)
{
	if (index < 0 || index >= m_charts.count()) return;
	
	beginRemoveRows(QModelIndex(), index, index);
	m_charts.removeAt(index);
	endRemoveRows();
}

void ChatViewGridModel::removeLabel(const QString &label)
{
	for(auto i=0;i<m_charts.count();i++)
	{
		if(m_charts[i].series.contains(label))
		{
			m_charts[i].series.removeAll(label);

			if(m_charts[i].series.isEmpty())
			{
				removeChart(i);

				return;
			}
		}
	}
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

void ChatViewGridModel::addSeriesToChart(int chartIndex, const QString& label, QtCharts::QLineSeries* series)
{

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

	return roles;
}
