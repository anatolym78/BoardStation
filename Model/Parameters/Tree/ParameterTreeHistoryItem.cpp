#include "ParameterTreeHistoryItem.h"

ParameterTreeHistoryItem::ParameterTreeHistoryItem(const QString &label, ParameterTreeItem *parent)
    : ParameterTreeItem(label, parent)
{
}

void ParameterTreeHistoryItem::addValue(const QVariant &value, const QDateTime &timestamp)
{
    m_values.append(value);
    m_timestamps.append(timestamp);
}

QVariant ParameterTreeHistoryItem::lastValue() const
{
    return m_values.last();
}

const QList<QVariant>& ParameterTreeHistoryItem::values() const
{
    return m_values;
}

const QList<QDateTime>& ParameterTreeHistoryItem::timestamps() const
{
    return m_timestamps;
}
