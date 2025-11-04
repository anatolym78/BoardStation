#include "BoardParametersTreeModel.h"
#include "BoardParameterTreeItem.h"

#include <random>

BoardParametersTreeModel::BoardParametersTreeModel(QObject* parent)
	: QAbstractItemModel(parent)
{
	m_rootItem = new BoardParameterTreeItem();
	makeRandomColors();
}

void BoardParametersTreeModel::setPlayer(DataPlayer* dataPlayer)
{
	m_dataPlayer = dataPlayer;

	if (m_playConnection)
	{
		QObject::disconnect(m_playConnection);
	}

	if (m_stopConnection)
	{
		QObject::disconnect(m_stopConnection);
	}

	clearParameters();

	m_playConnection = connect(m_dataPlayer, &DataPlayer::parameterPlayed,
		this, &BoardParametersTreeModel::onNewParameterAdded);
}

void BoardParametersTreeModel::onNewParameterAdded(BoardParameterSingle* parameter)
{
	if (parameter == nullptr) return;

	if (m_rootItem->hasParameter(parameter))
	{
		m_rootItem->updateParameter(parameter);

		emit dataChanged(index(0, 0), index(1, 0));
	}
	else
	{
		this->beginInsertRows(QModelIndex(), m_rootItem->childCount(), m_rootItem->childCount());
		m_rootItem->addParameter(parameter);
		this->endInsertRows();
	}
}

int BoardParametersTreeModel::columnCount(const QModelIndex& parent) const
{
	return 2;
}

int BoardParametersTreeModel::rowCount(const QModelIndex& parent) const
{
	BoardParameterTreeItem* parentItem;
	if (!parent.isValid())
	{
		parentItem = m_rootItem;
	}
	else
	{
		parentItem = static_cast<BoardParameterTreeItem*>(parent.internalPointer());
	}

	if (parentItem == nullptr) 
		return 0;

	auto rowCount = parentItem->childCount();

	return rowCount;
}

QModelIndex BoardParametersTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	BoardParameterTreeItem* parentItem;
	if (!parent.isValid())
	{
		parentItem = m_rootItem;
	}
	else
	{
		parentItem = static_cast<BoardParameterTreeItem*>(parent.internalPointer());
	}

	if (parentItem == nullptr)
	{
		return QModelIndex();
	}

	auto childItem = parentItem->child(row);

	if (childItem)
	{
		return createIndex(row, column, childItem);
	}

	return QModelIndex();
}



QModelIndex BoardParametersTreeModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	auto childItem = static_cast<BoardParameterTreeItem*>(index.internalPointer());

	if (!childItem) 
		return QModelIndex();

	auto parentItem = childItem->parent();

	if (!parentItem)
		return QModelIndex();
	if (!parentItem->parent()) 
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

QVariant BoardParametersTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	auto treeItem = static_cast<BoardParameterTreeItem*>(index.internalPointer());
	if (treeItem)
	{
		switch (static_cast<ParameterRole>(role))
		{
		case ParameterRole::LabelRole:
			return treeItem->label();
		case ParameterRole::ValueRole:
			return treeItem->value();
		default:
			break;
		}
	}

	return QVariant();
}

bool BoardParametersTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	return false;

	if (!index.isValid()) return false;

	if (role == (int)ParameterRole::ChartVisibilityRole)
	{
		// This is simplistic and might need adjustment for tree
		m_chartVisibilities[index.row()] = value.toBool();
		emit dataChanged(index, index, { role });
		return true;
	}

	return false;
}

QHash<int, QByteArray> BoardParametersTreeModel::roleNames() const
{
	QHash<int, QByteArray> rolesHash;
	rolesHash[(int)ParameterRole::LabelRole] = "label";
	rolesHash[(int)ParameterRole::ValueRole] = "value";
	return rolesHash;
}

int BoardParametersTreeModel::getCountParameters() const
{
	return m_rootItem->childCount();
}

void BoardParametersTreeModel::clearParameters()
{
	onParametersCleared();
}

void BoardParametersTreeModel::onParametersCleared()
{
	beginResetModel();
	delete m_rootItem;
	m_rootItem = new BoardParameterTreeItem();
	m_chartVisibilities.clear();
	endResetModel();
	emit countParametersChanged();
}

void BoardParametersTreeModel::makeRandomColors()
{
	int hue = 0;
	int hueStep = 60 + 5;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib_int(192, 255);
	for (auto i = 0; i < 200; i++)
	{
		auto sat = distrib_int(gen);
		auto val = distrib_int(gen);
		m_colors.append(QColor::fromHsv(hue, sat, val, 255));
		hue = (hue + hueStep) % 360;
	}
}

QString BoardParametersTreeModel::indexToLabel(int row) const
{
	//auto keys = m_parameters.keys();
	//for (auto i = 0; i < keys.count(); i++)
	//{
	//	if (i == row)
	//	{
	//		return keys[i];
	//	}
	//}

	return QString();
}

BoardParametersTreeModel::~BoardParametersTreeModel()
{
	delete m_rootItem;
}
