#ifndef PARAMETERTREEHISTORYITEM_H
#define PARAMETERTREEHISTORYITEM_H

#include "ParameterTreeItem.h"
#include <QVariant>
#include <QDateTime>
#include <QList>

class ParameterTreeHistoryItem : public ParameterTreeItem
{
    Q_OBJECT

public:
    explicit ParameterTreeHistoryItem(const QString &label, ParameterTreeItem *parent = nullptr);
    ItemType type() const override { return ItemType::History; }

    void addValue(const QVariant &value, const QDateTime &timestamp);
    QVariant lastValue() const;

    const QList<QVariant>& values() const;
    const QList<QDateTime>& timestamps() const;

private:
    QList<QVariant> m_values;
    QList<QDateTime> m_timestamps;
};

#endif // PARAMETERTREEHISTORYITEM_H
