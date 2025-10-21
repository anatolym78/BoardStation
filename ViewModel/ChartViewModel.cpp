#include "ChartViewModel.h"
#include "../Model/Parameters/BoardParameterSingle.h"
#include <QDebug>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QDateTime>

ChartViewModel::ChartViewModel(QObject *parent) : QAbstractTableModel(parent)
{
}

void ChartViewModel::setPlayer(DataPlayer* dataPlayer)
{
	m_dataPlayer = dataPlayer;

	if (m_stopConnection)
	{
		QObject::disconnect(m_stopConnection);
	}

	clearCharts();

	m_stopConnection = connect(m_dataPlayer, &DataPlayer::stopped,
		this, &ChartViewModel::clearCharts);
}


int ChartViewModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0;

	auto countSeries = m_series.count();

	if (countSeries == 0) return 0;

	if (countSeries % m_columnCount == 0)
	{
		return countSeries / m_columnCount;
	}
	else
	{
		return countSeries / m_columnCount + 1;
	}
}

int ChartViewModel::columnCount(const QModelIndex& parent /* = QModelIndex() */) const
{
	return m_columnCount;
}

int ChartViewModel::cellToIndex(int cellRow, int cellCol) const
{
	return cellRow * columnCount(QModelIndex()) + cellCol;
}

int ChartViewModel::cellToIndex(const QModelIndex& modelIndex) const
{
	return cellToIndex(modelIndex.row(), modelIndex.column());
}

QModelIndex ChartViewModel::indexToCell(int index) const
{
	auto column = index % columnCount(QModelIndex());
	auto row = index / columnCount(QModelIndex());

	return this->createIndex(row, column);
}

QVariant ChartViewModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid())
		return {};

	auto cellIndex = cellToIndex(index);
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
	if(role == LabelsRole)

	return m_series[cellIndex].last();
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

	beginInsertRows(QModelIndex(), 0, rowCount(QModelIndex()));// QModelIndex(), m_series.size(), m_series.size());
	m_series.append(QList<QString>() << label);
	resetDepths();
	endInsertRows();

	emit parameterAdded(label, color);
}

void ChartViewModel::removeChart(int index)
{
	if (index < 0 || index >= m_series.count()) return;

	auto cell = indexToCell(index);

	if (!cell.isValid()) return;
	
	beginResetModel();
	m_series.removeAt(index);
	endResetModel();
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

bool ChartViewModel::parameterExistsInHistory(const QString& label) const
{
	return m_dataPlayer->storage()->containsParameter(label);
}
void ChartViewModel::mergeCharts(int movedIndex, int targetIndex)
{
	return; // not realized

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

void ChartViewModel::clearCharts()
{
	if (m_series.isEmpty())
		return;
		
	beginResetModel();
	m_series.clear();
	m_depths.clear();
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

QHash<int, QByteArray> ChartViewModel::roleNames() const
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
	return roles;
}
