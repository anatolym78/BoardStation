#ifndef SIMPLETREEITEM_H
#define SIMPLETREEITEM_H

#include <QVariant>
#include <QList>

class SimpleTreeItem
{
public:
    explicit SimpleTreeItem(const QList<QVariant> &data, SimpleTreeItem *parentItem = nullptr);
    ~SimpleTreeItem();

    void appendChild(SimpleTreeItem *child);

    SimpleTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    SimpleTreeItem *parentItem();

private:
    QList<SimpleTreeItem*> m_childItems;
    QList<QVariant> m_itemData;
    SimpleTreeItem *m_parentItem;
};

#endif // SIMPLETREEITEM_H
