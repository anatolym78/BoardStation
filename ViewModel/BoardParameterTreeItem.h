#ifndef BOARDPARAMETERTREEITEM_H
#define BOARDPARAMETERTREEITEM_H

#include <QVariant>
#include <QList>

class BoardParameterSingle;

class BoardParameterTreeItem
{
public:
    explicit BoardParameterTreeItem(QString label, BoardParameterTreeItem* parentItem = nullptr, BoardParameterSingle* parameter = nullptr, const QVariant& itemValue = QVariant());
    ~BoardParameterTreeItem();

    void appendChild(BoardParameterTreeItem* child);
    BoardParameterTreeItem* child(int row);
    int childCount() const;
    const QList<BoardParameterTreeItem*>& children() const;
    QList<BoardParameterTreeItem*>& children();
    void clearChildren();
    BoardParameterSingle* parameter() const;
    QVariant itemValue() const;
    void setParameter(BoardParameterSingle* p);
    QString label() const;
    QString fullPath() const;
    BoardParameterTreeItem* parentItem();
    int row() const;

private:
    QString m_label;
    QList<BoardParameterTreeItem*> m_childItems;
    BoardParameterTreeItem* m_parentItem;
    BoardParameterSingle* m_parameter;
    QVariant m_itemValue;
};

#endif // BOARDPARAMETERTREEITEM_H
