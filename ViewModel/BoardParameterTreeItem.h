#ifndef BOARDPARAMETERTREEITEM_H
#define BOARDPARAMETERTREEITEM_H

#include <QVariant>
#include <QList>
#include <QMap>

class BoardParameterSingle;

class BoardParameterTreeItem
{
public:
    void append(BoardParameterSingle* parameter);
    explicit BoardParameterTreeItem(QString label, BoardParameterTreeItem* parentItem = nullptr, BoardParameterSingle* parameter = nullptr, const QVariant& itemValue = QVariant());
    ~BoardParameterTreeItem();

    void appendChild(BoardParameterTreeItem* child);
    BoardParameterTreeItem* child(int row);
    int childCount() const;
    //const QList<BoardParameterTreeItem*>& children() const;
    //QList<BoardParameterTreeItem*>& children();
    //void clearChildren();
    //BoardParameterSingle* parameter() const;
    //QVariant itemValue() const;
    //void setParameter(BoardParameterSingle* p);
    //QString label() const;
    //QString fullPath() const;
    //BoardParameterTreeItem* parentItem();
    int row() const;

    bool isRoot() const { return m_parentItem == nullptr; }
    bool isLeaf() const { return m_childItems.count() == 0; }
    BoardParameterTreeItem* parent() const { return m_parentItem; }

private:
    QString m_label;
    QVariant m_itemValue;
    QList<BoardParameterTreeItem*> m_childItems;
    BoardParameterTreeItem* m_parentItem = nullptr;
    BoardParameterSingle* m_parameter;
};

#endif // BOARDPARAMETERTREEITEM_H
