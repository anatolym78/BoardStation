#include "BoardParameterTreeItem.h"
#include "Model/Parameters/BoardParameterSingle.h"

#include <QObject>

BoardParameterTreeItem::BoardParameterTreeItem()
{
	m_itemLabel = "Root";// tr("Root");
}

BoardParameterTreeItem::BoardParameterTreeItem(BoardParameterTreeItem* parentItem, BoardParameterSingle* parameter) :
	m_parentItem(parentItem)
	, m_parameter(parameter)
{
	if (parentItem == nullptr || parameter == nullptr)
	{
		return;
	}

	m_level = parentItem->level() + 1; // если это последний уровень вложенности, то он равен 0

	// Если дошли до последнего вложенного параметра
	if (isLastLevel()) // value layer
	{
		// 
		if (parameter->isArrayValue())
		{
			for (auto valueIndex = 0; valueIndex < parameter->countValues(); valueIndex++)
			{
				m_childItems.append(new BoardParameterTreeItem(this, parameter, valueIndex));
			}
		}
		else
		{
			m_itemValue = parameter->valueToString();
		}
	}	
	else 
	{
		// если это не последний уровень вложенности, то рекурсивно идем дальше
		m_childItems.append(new BoardParameterTreeItem(this, parameter));
	}
		
	m_itemLabel = parameter->label(m_level);
}

bool BoardParameterTreeItem::isLastLevel() const
{
	return m_parameter->countLabelParts() == m_level + 1;
}

BoardParameterTreeItem::BoardParameterTreeItem(BoardParameterTreeItem* parentItem, BoardParameterSingle* parameter, int valueIndex) :
	m_parentItem(parentItem)
	, m_parameter(parameter)
{
	m_level = parentItem->level() + 1;

	m_itemLabel = QString("[%1]").arg(valueIndex);
	m_itemValue = parameter->value(valueIndex).toString();
	m_indexLabel = true;
}

void BoardParameterTreeItem::addParameter(BoardParameterSingle* parameter)
{
	if (parameter == nullptr) return;

	// добавлять параметры можно только в корень
	if (!isRoot()) return;

	if (!parameter->isValid()) return;

	if (hasParameter(parameter)) return;

	m_childItems.append(new BoardParameterTreeItem(this, parameter));
}

BoardParameterTreeItem* BoardParameterTreeItem::updateParameter(BoardParameterSingle* parameter)
{
	if (!hasParameter(parameter)) return nullptr;

	auto parameterItem = findItem(parameter);

	if (parameterItem == nullptr) return nullptr;

	// двигаемся вниз до узла содержащего значение
	do
	{
		if (parameterItem->childCount() == 0) break;

		parameterItem = parameterItem->child(0);
	} 
	while (true);
	

	if (parameter->isArrayValue())
	{

	}
	else
	{
		parameterItem->m_itemValue = parameter->valueToString();

		return parameterItem;
	}

	return nullptr;
}

bool BoardParameterTreeItem::hasParameter(BoardParameterSingle* parameter)
{
	if (parameter == nullptr) return false;

	auto parameterLabel = parameter->label();

	if (parameterLabel.isEmpty()) return false;

	for (auto item : m_childItems)
	{
		auto itemParameter = item->parameter();

		if(itemParameter == nullptr) continue;

		if (itemParameter->label() == parameterLabel) return true;
	}

	return false;
}


BoardParameterTreeItem* BoardParameterTreeItem::findItem(BoardParameterSingle* parameter)
{
	if (parameter == nullptr) return nullptr;

	for (auto item : m_childItems)
	{
		if (item->parameter()->label() == parameter->label()) return item;
	}

	return nullptr;
}

BoardParameterTreeItem::~BoardParameterTreeItem()
{
	qDeleteAll(m_childItems);
}

BoardParameterTreeItem* BoardParameterTreeItem::child(int row)
{
	if (row < 0 || row >= m_childItems.size()) return nullptr;

	return m_childItems.at(row);
}

int BoardParameterTreeItem::childCount() const
{
	return m_childItems.count();
}

int BoardParameterTreeItem::row() const
{
	if (parent())
		return parent()->m_childItems.indexOf(const_cast<BoardParameterTreeItem*>(this));
	
	return 0;
}

QString BoardParameterTreeItem::label() const
{
	return m_itemLabel;
}

QVariant BoardParameterTreeItem::value() const
{
	return m_itemValue;
}
