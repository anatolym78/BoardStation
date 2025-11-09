#ifndef PARAMETERTREEARRAYITEM_H
#define PARAMETERTREEARRAYITEM_H

#include "ParameterTreeItem.h"

class ParameterTreeArrayItem : public ParameterTreeItem
{
    Q_OBJECT
public:
    explicit ParameterTreeArrayItem(const QString &label, ParameterTreeItem *parent = nullptr);
    ItemType type() const override { return ItemType::Array; }
};

#endif // PARAMETERTREEARRAYITEM_H

