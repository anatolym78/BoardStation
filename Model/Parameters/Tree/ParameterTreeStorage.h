#ifndef PARAMETERTREESTORAGE_H
#define PARAMETERTREESTORAGE_H

#include "ParameterTreeItem.h"

class ParameterTreeHistoryItem;

class ParameterTreeStorage : public ParameterTreeItem
{
    Q_OBJECT
public:
    explicit ParameterTreeStorage(QObject *parent = nullptr);
    ItemType type() const override { return ItemType::Root; }

public slots:
    void appendSnapshot(ParameterTreeStorage* snapshot);
    void setSnapshot(ParameterTreeStorage* snapshot);

signals:
    void parameterAdded(ParameterTreeItem* newItem);
    void valueAdded(ParameterTreeHistoryItem* updatedItem);

private:
    void appendNode(ParameterTreeItem* localParent, ParameterTreeItem* incomingNode);
    void setNode(ParameterTreeItem* localParent, ParameterTreeItem* incomingNode);
};

#endif // PARAMETERTREESTORAGE_H
