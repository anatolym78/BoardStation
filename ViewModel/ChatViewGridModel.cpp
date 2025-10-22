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

	if (m_stopConnection)
	{
		QObject::disconnect(m_stopConnection);
	}

	clearCharts();

	m_stopConnection = connect(m_dataPlayer, &DataPlayer::stopped,
		this, &ChatViewGridModel::clearCharts);
}


int ChatViewGridModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0;

	return m_series.count();
}

QVariant ChatViewGridModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid())
		return {};

	auto cellIndex = index.row();// cellToIndex(index);
	if (role == IsExists)
	{
		return cellIndex < m_series.count();
	}

	if(cellIndex >= m_series.count()) 
		return {};

	if(m_series.isEmpty()) 
		return {};

	if(role == DepthRole)
	{
		return m_depths[cellIndex];
	}
	if (role == LabelsRole)
	{
		return m_series[cellIndex];
	}

	if (role == LabelRole)
	{
		return m_series[cellIndex].last();
	}
	if (role == LabelsRole)
	{
		return m_series[cellIndex];
	}

	if (role == SelectionRole)
	{
		return m_selectedIndices[cellIndex];
	}

	if (role == HoverRole)
	{
		return m_hoverIndex == cellIndex;
	}

	return m_series[cellIndex].last();
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
	//beginInsertRows(QModelIndex(), m_elements.count(), m_elements.count());
	//m_elements.append(0);

	//endInsertRows();

	//return;

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

	beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));// QModelIndex(), m_series.size(), m_series.size());
	m_series.append(QList<QString>() << label);
	m_selectedIndices.append(false);
	resetDepths();
	endInsertRows();

	emit parameterAdded(label, color);
}

void ChatViewGridModel::removeChart(int index)
{
	if (index < 0 || index >= m_series.count()) return;

	//auto cell = indexToCell(index);

	//if (!cell.isValid()) return;
	
	beginResetModel();
	m_series.removeAt(index);
	m_selectedIndices.removeAt(index);
	endResetModel();
}

void ChatViewGridModel::removeLabel(const QString &label)
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

bool ChatViewGridModel::parameterExistsInHistory(const QString& label) const
{
	return m_dataPlayer->storage()->containsParameter(label);
}
void ChatViewGridModel::mergeCharts(int movedIndex, int targetIndex)
{
	return; // not realized

	if (movedIndex < 0 || movedIndex >= m_series.size())
	{
		qWarning() << "ChatViewGridModel: Invalid moved index" << movedIndex;
		return;
	}

	if (targetIndex < 0 || targetIndex >= m_series.size())
	{
		qWarning() << "ChatViewGridModel: Invalid target index" << targetIndex;
		return;
	}

	beginRemoveRows(QModelIndex(), movedIndex, movedIndex);

	auto seriesLabels = m_series[movedIndex];
	m_series[targetIndex].append(seriesLabels);
	m_series.removeAt(movedIndex);

	resetDepths();

	endRemoveRows();
}

void ChatViewGridModel::splitSeries(int chartIndex)
{
	return; // not realized

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

void ChatViewGridModel::clearCharts()
{
	if (m_series.isEmpty())
		return;
		
	beginResetModel();
	m_series.clear();
	m_depths.clear();
	m_selectedIndices.clear();
	endResetModel();
}

QStringList ChatViewGridModel::getChartSeriesLabels(int chartIndex) const
{
	if(chartIndex < 0 || chartIndex >= rowCount()) return QStringList();

	return m_series[chartIndex];
}

void ChatViewGridModel::reorderChartsBeforeDrag(int dragIndex)
{
	resetDepths();

	m_depths[dragIndex] = 0;

	auto topLeft = this->index(0, 0);
	auto bottomRight = this->index(rowCount() - 1, 0);

	emit dataChanged(topLeft, bottomRight);
}

void ChatViewGridModel::resetDepths()
{
	m_depths.clear();

	for(auto i=0;i<m_series.count();i++)
	{
		m_depths.append(i + 1);
	}
}


QStringList ChatViewGridModel::chartLabels() const
{
	QStringList labels;
	for(auto s : m_series)
	{
		labels.append(s);
	}

	return labels;
}

bool ChatViewGridModel::hasSeries(const QString &label) const
{
	for(auto s : m_series)
	{
		if(s.contains(label)) return true;
	}

	return false;
}

bool ChatViewGridModel::selectElement(int index, bool keepSelection)
{
	if (keepSelection == false)
	{
		clearSelection();
	}

	if (index >= 0 && index < m_selectedIndices.count())
	{
		m_selectedIndices[index] =  !m_selectedIndices[index];
	}

	updateAllCells();

	return true;
}

void ChatViewGridModel::clearSelection()
{
	for (auto& selection : m_selectedIndices)
	{
		selection = false;
	}

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

void ChatViewGridModel::updateAllCells()
{
	for (auto i = 0; i < m_series.count(); i++)
	{
		emit dataChanged(this->createIndex(i, 0), this->createIndex(i, 0));
	}
}

int ChatViewGridModel::findChartIndex(const QString &label) const
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

QHash<int, QByteArray> ChatViewGridModel::roleNames() const
{
	QHash<int, QByteArray> roles;

	roles[ChartRole] = "chart";
	roles[ChartLabelRole] = "chartLabel";
	roles[LabelsRole] = "labels";
	roles[LabelRole] = "label";
	roles[ChartIndexRole] = "chartIndex";
	roles[HasDataRole] = "hasData";
	roles[DepthRole] = "depth";
	roles[IsExists] = "exists";
	roles[SelectionRole] = "selection";
	roles[HoverRole] = "hover";

	return roles;
}
