#include "BoardParametersTreeModel.h"
#include "BoardParameterTreeItem.h"

#include <random>

BoardParametersTreeModel::BoardParametersTreeModel(QObject* parent)
	: QAbstractItemModel(parent)
{
	m_rootItem = new BoardParameterTreeItem("root");
	makeRandomColors();

	//m_topLevelItems.insert("param", new BoardParameterTreeItem("label"));

	//m_parameters.insert("ok", nullptr);
}

void BoardParametersTreeModel::onNewParameterAdded(BoardParameterSingle* parameter)
{
	if (m_rootItem->childCount() > 0) return; // заглушка

	this->beginInsertRows(QModelIndex(), m_rootItem->childCount(), m_rootItem->childCount());
	m_rootItem->append(parameter);
	this->endInsertRows();
	return;
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

int BoardParametersTreeModel::columnCount(const QModelIndex& parent) const
{
	return 2;
}

int BoardParametersTreeModel::rowCount(const QModelIndex& parent) const
{
	if (parent.column() > 0)
		return 0;

	auto parentItem = static_cast<BoardParameterTreeItem*>(parent.internalPointer());

	if (parentItem == nullptr) return 0;

	return parentItem->childCount();
}

QModelIndex BoardParametersTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	return QModelIndex();

	if (!this->hasIndex(row, column, parent)) return QModelIndex();

	auto parentItem = static_cast<BoardParameterTreeItem*>(parent.internalPointer());

	if (parentItem == nullptr) return QModelIndex();

	auto childItem = parentItem->child(row);

	if (childItem) 
		return createIndex(row, column, childItem);

	return QModelIndex();
}



QModelIndex BoardParametersTreeModel::parent(const QModelIndex& index) const
{
	return QModelIndex();

	if (!index.isValid())
		return QModelIndex();


	auto childItem = static_cast<BoardParameterTreeItem*>(index.internalPointer());
	auto parentItem = childItem->parent();

	if (parentItem == nullptr) return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

QVariant BoardParametersTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) return QVariant();

	switch (static_cast<ParameterRole>(role))
	{
	case ParameterRole::LabelRole:
		return "tralala";
	case ParameterRole::ValueRole:
		return "123";
	default:
		return QVariant();
	}		

	auto treeItem = static_cast<BoardParameterSingle*>(index.internalPointer());
	if (treeItem)
	{
		switch (static_cast<ParameterRole>(role))
		{
		case ParameterRole::LabelRole:
			return "tralala";
		case ParameterRole::ValueRole:
			return "123";
		}
	}

	return QVariant();

	//BoardParameterTreeItem* item = static_cast<BoardParameterTreeItem*>(index.internalPointer());
	//BoardParameterSingle* parameter = item->parameter();

	//switch (static_cast<ParameterRole>(role))
	//{
	//case ParameterRole::LabelRole:
	//    return item->label();
	//case ParameterRole::ValueRole:
	//{
	//    if (item->itemValue().isValid())
	//    {
	//        return item->itemValue();
	//    }
	//    if (!parameter) return {};
	//    bool ok;
	//    auto doubleValue = parameter->value().toDouble(&ok);
	//    if (!ok) return parameter->value();
	//    return QString::number(doubleValue, 'g', 3);
	//}
	//case ParameterRole::UnitRole:
	//    return parameter ? QVariant::fromValue(parameter->unit()) : QVariant();
	//case ParameterRole::TimeRole:
	//    return parameter ? QVariant::fromValue(parameter->timestamp()) : QVariant();
	//case ParameterRole::ChartVisibilityRole:
	//    return QVariant::fromValue(m_chartVisibilities.value(index.row())); // This is simplistic and might need adjustment for tree
	//case ParameterRole::ColorRole:
	//    return QVariant::fromValue(m_colors.value(index.row())); // This is simplistic and might need adjustment for tree
	//case ParameterRole::FullPathRole:
	//    return item->fullPath();
	//}

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
	//rolesHash[(int)ParameterRole::UnitRole] = "unit";
	//rolesHash[(int)ParameterRole::TimeRole] = "timestamp";
	//rolesHash[(int)ParameterRole::ChartVisibilityRole] = "chartVisibility";
	//rolesHash[(int)ParameterRole::ColorRole] = "parameterColor";
	//rolesHash[(int)ParameterRole::FullPathRole] = "fullPath";
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
	m_rootItem = new BoardParameterTreeItem("root");
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
