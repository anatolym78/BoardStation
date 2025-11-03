#include "BoardParameterTreeItem.h"
#include "Model/Parameters/BoardParameterSingle.h"


void BoardParameterTreeItem::append(BoardParameterSingle* parameter)
{
	if (parameter == nullptr) return;

	auto label = parameter->label();

	if (label.isEmpty()) return;

	//if (m_childItems.contains(label))
	//{

	//}
	//else
	//{
	//    m_childItems.insert(label, new BoardParameterTreeItem(label));
	//}
	return;
}

BoardParameterTreeItem::BoardParameterTreeItem(QString label, BoardParameterTreeItem* parentItem, BoardParameterSingle* parameter, const QVariant& itemValue)
	: m_label(label)
	, m_parentItem(parentItem)
	, m_parameter(parameter)
	, m_itemValue(itemValue)
{
}

BoardParameterTreeItem::~BoardParameterTreeItem()
{
	qDeleteAll(m_childItems);
}

void BoardParameterTreeItem::appendChild(BoardParameterTreeItem* child)
{
	m_childItems.append(child);
}

BoardParameterTreeItem* BoardParameterTreeItem::child(int row)
{
	if (row < 0 || row >= m_childItems.size()) return nullptr;

	return m_childItems.at(row);
}

int BoardParameterTreeItem::childCount() const
{
	return 0;

	return m_childItems.count();
}
//
//const QList<BoardParameterTreeItem*>& BoardParameterTreeItem::children() const { return m_childItems; }
//QList<BoardParameterTreeItem*>& BoardParameterTreeItem::children() { return m_childItems; }
//
//void BoardParameterTreeItem::clearChildren()
//{
//    m_childItems.clear();
//}
//
//BoardParameterSingle* BoardParameterTreeItem::parameter() const
//{
//    return m_parameter;
//}
//
//QVariant BoardParameterTreeItem::itemValue() const
//{
//    return m_itemValue;
//}
//
//void BoardParameterTreeItem::setParameter(BoardParameterSingle* p)
//{
//    m_parameter = p;
//}
//
//QString BoardParameterTreeItem::label() const
//{
//    if (m_parameter)
//    {
//        auto parts = m_parameter->label().split('.');
//        return parts.last();
//    }
//    return m_label;
//}
//
//QString BoardParameterTreeItem::fullPath() const
//{
//    if (m_parameter)
//    {
//        return m_parameter->label();
//    }
//
//    if (m_parentItem && m_parentItem->parameter() && m_itemValue.isValid())
//    {
//        return QString("%1%2").arg(m_parentItem->parameter()->label()).arg(m_label);
//    }
//
//    QString result = m_label;
//    BoardParameterTreeItem* current = m_parentItem;
//    while(current && current->m_parentItem) // stop before root
//    {
//        result = current->m_label + "." + result;
//        current = current->m_parentItem;
//    }
//    return result;
//}
//
//BoardParameterTreeItem* BoardParameterTreeItem::parentItem()
//{
//    return m_parentItem;
//}
//
int BoardParameterTreeItem::row() const
{
	if (parent())
		return parent()->m_childItems.indexOf(const_cast<BoardParameterTreeItem*>(this));
	
	return 0;
}
