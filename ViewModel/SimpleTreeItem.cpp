#include "SimpleTreeItem.h"

SimpleTreeItem::SimpleTreeItem(const QList<QVariant> &data, SimpleTreeItem *parent)
    : m_itemData(data), m_parentItem(parent)
{}

SimpleTreeItem::~SimpleTreeItem()
{
    qDeleteAll(m_childItems);
}

void SimpleTreeItem::appendChild(SimpleTreeItem *item)
{
    m_childItems.append(item);
}

SimpleTreeItem *SimpleTreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int SimpleTreeItem::childCount() const
{
    return m_childItems.count();
}

int SimpleTreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant SimpleTreeItem::data(int column) const
{
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

SimpleTreeItem *SimpleTreeItem::parentItem()
{
    return m_parentItem;
}

int SimpleTreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<SimpleTreeItem*>(this));
    return 0;
}
