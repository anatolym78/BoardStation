#include "ParameterTreeItem.h"

ParameterTreeItem::ParameterTreeItem(const QString &label, ParameterTreeItem *parent)
    : QObject(parent),
      m_label(label),
      m_parentItem(parent)
{
}

ParameterTreeItem::~ParameterTreeItem()
{
    // Дочерние элементы будут удалены автоматически механизмом владения QObject
}

void ParameterTreeItem::appendChild(ParameterTreeItem *child)
{
    m_childItems.append(child);
}

ParameterTreeItem *ParameterTreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
    {
        return nullptr;
    }
    return m_childItems.at(row);
}

ParameterTreeItem* ParameterTreeItem::findChildByLabel(const QString& label, bool recursive)
{
    for (ParameterTreeItem* item : m_childItems)
    {
        if (item->label() == label)
        {
            return item;
        }
        if (recursive)
        {
            ParameterTreeItem* found = item->findChildByLabel(label, true);
            if (found)
            {
                return found;
            }
        }
    }
    return nullptr;
}

int ParameterTreeItem::childCount() const
{
    return m_childItems.count();
}

ParameterTreeItem *ParameterTreeItem::parentItem() const
{
    return m_parentItem;
}

int ParameterTreeItem::level() const
{
	auto level = 0;
    auto parentItem = this;
    while(parentItem = parentItem->parentItem())
    {        
        level++;
    } 

    return level;
}

QString ParameterTreeItem::label() const
{
    return m_label;
}

int ParameterTreeItem::row() const
{
    if (m_parentItem)
    {
        return m_parentItem->m_childItems.indexOf(const_cast<ParameterTreeItem*>(this));
    }
    return 0;
}

QList<ParameterTreeItem*> ParameterTreeItem::children() const
{
    return m_childItems;
}
