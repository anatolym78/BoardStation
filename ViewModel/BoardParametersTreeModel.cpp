#include "BoardParametersTreeModel.h"
#include "BoardParameterTreeItem.h"
#include "./Model/Parameters/Tree/ParameterTreeHistoryItem.h"

#include <random>

BoardParametersTreeModel::BoardParametersTreeModel(QObject* parent)
	: QAbstractItemModel(parent)
{
	makeRandomColors();

	m_rootItem = new ParameterTreeStorage(this);

	connect(m_rootItem, &ParameterTreeStorage::parameterAdded, this, &BoardParametersTreeModel::onParameterAdded);
	connect(m_rootItem, &ParameterTreeStorage::valueAdded, this, &BoardParametersTreeModel::onValueAdded);
	connect(m_rootItem, &ParameterTreeStorage::valueChanged, this, &BoardParametersTreeModel::onValueChanged);
}

void BoardParametersTreeModel::setSnapshot(ParameterTreeStorage* storage, bool isBackPlaying)
{
	m_rootItem->setSnapshot(storage);
}

void BoardParametersTreeModel::onParameterAdded(ParameterTreeItem* newItem)
{
	this->beginResetModel();
	this->endResetModel();
}

void BoardParametersTreeModel::onValueAdded(ParameterTreeHistoryItem* updatedItem)
{
}

void BoardParametersTreeModel::onValueChanged(ParameterTreeHistoryItem* history)
{
	QModelIndex foundedIndex;
	if (findIndexRecursive(history, QModelIndex(), foundedIndex))
	{
		// foundedIndex - это индекс для колонки 0. 
		// Нам нужно создать индекс для колонки 1, где отображается значение.
		QModelIndex valueIndex = index(foundedIndex.row(), 1, foundedIndex.parent());

		// Испускаем сигнал только для ячейки со значением и только для роли ValueRole.
		// Это эффективнее, чем обновлять всю строку.
		emit dataChanged(valueIndex, valueIndex, { ValueRole });
	}
}
bool BoardParametersTreeModel::findIndexRecursive(ParameterTreeItem* item, QModelIndex parentIndex, QModelIndex& foundedIndex)
{
	if (static_cast<ParameterTreeItem*>(parentIndex.internalPointer()) == item)
	{
		foundedIndex = QModelIndex(parentIndex);

		return true;
	}
	for (auto i = 0; i < this->rowCount(parentIndex); i++)
	{
		auto nestedParentIndex = this->index(i, 0, parentIndex);

		if (findIndexRecursive(item, nestedParentIndex, foundedIndex))
		{
			return true;
		}
	}

	return false;
}

int BoardParametersTreeModel::columnCount(const QModelIndex& parent) const
{
	return 2;
}

int BoardParametersTreeModel::rowCount(const QModelIndex& parent) const
{
	ParameterTreeItem* parentItem;
	if (!parent.isValid())
	{
		parentItem = m_rootItem;
	}
	else
	{
		parentItem = static_cast<ParameterTreeItem*>(parent.internalPointer());
	}

	if (parentItem == nullptr)
		return 0;

	return parentItem->childCount();
}

QModelIndex BoardParametersTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	ParameterTreeItem* parentItem;
	if (!parent.isValid())
	{
		parentItem = m_rootItem;
	}
	else
	{
		parentItem = static_cast<ParameterTreeItem*>(parent.internalPointer());
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

	auto childItem = static_cast<ParameterTreeItem*>(index.internalPointer());

	if (!childItem)
		return QModelIndex();

	auto parentItem = childItem->parentItem();

	if (!parentItem)
		return QModelIndex();
	if (!parentItem->parentItem())
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

QVariant BoardParametersTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	auto treeItem = static_cast<ParameterTreeItem*>(index.internalPointer());
	if (treeItem)
	{
		// ========== КОД ДЛЯ QT WIDGETS (QTreeView) ==========
		// Для работы с Qt Widgets: оставить этот блок активным
		// Для работы только с QML: закомментировать этот блок
		if (role == Qt::DisplayRole)
		{
			if (index.column() == 0)
			{
				return treeItem->label();
			}
			if (index.column() == 1)
			{
				if (treeItem->type() == ParameterTreeItem::ItemType::History)
				{
					auto leafItem = static_cast<ParameterTreeHistoryItem*>(index.internalPointer());
					return leafItem->values().last();
				}
				return QVariant();
			}
		}
		// =====================================================
		
		// ========== КОД ДЛЯ QML ==========
		// Для работы с QML: оставить этот блок активным
		// Для работы только с Qt Widgets: закомментировать этот блок
		switch (static_cast<ParameterRole>(role))
		{
		case ParameterRole::LabelRole:
			return treeItem->label();
		case ParameterRole::ValueRole:
			if (treeItem->type() == ParameterTreeItem::ItemType::History)
			{
				auto leafItem = static_cast<ParameterTreeHistoryItem*>(index.internalPointer());
				
				return leafItem->values().last();
			}
			else
			{
				return QVariant();
			}
		default:
			break;
		}
		// ==================================
	}

	return QVariant();
}

bool BoardParametersTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	return false;

	if (!index.isValid()) return false;

	if (role == (int)ParameterRole::ChartVisibilityRole)
	{
		m_chartVisibilities[index.row()] = value.toBool();
		emit dataChanged(index, index, { role });
		return true;
	}

	return false;
}

QVariant BoardParametersTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// Для Qt Widgets: возвращаем заголовки колонок
	// Для QML: этот метод не используется
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section < 2)
		return m_horizontalHeaders[section];

	return QAbstractItemModel::headerData(section, orientation, role);
}

bool BoardParametersTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	// Для Qt Widgets: устанавливаем заголовки колонок
	// Для QML: этот метод не используется
	if (orientation == Qt::Horizontal && role == Qt::EditRole && section < 2)
	{
		m_horizontalHeaders[section] = value;
		emit headerDataChanged(orientation, section, section);
		return true;
	}
	return QAbstractItemModel::setHeaderData(section, orientation, value, role);
}

QHash<int, QByteArray> BoardParametersTreeModel::roleNames() const
{
	QHash<int, QByteArray> rolesHash;
	rolesHash[(int)ParameterRole::LabelRole] = "label";
	rolesHash[(int)ParameterRole::ValueRole] = "value";
	return rolesHash;
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

BoardParametersTreeModel::~BoardParametersTreeModel()
{
}
