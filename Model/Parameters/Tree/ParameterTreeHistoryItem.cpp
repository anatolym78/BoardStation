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

void ParameterTreeHistoryItem::setValues(const QList<QVariant>& values, const QList<QDateTime>& timestamps)
{
    m_values = values;
    m_timestamps = timestamps;
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

const QDateTime& ParameterTreeHistoryItem::lastTimestamp() const
{
    if (m_timestamps.isEmpty()) return QDateTime();

    return m_timestamps.last();
}

QString ParameterTreeHistoryItem::control() const
{
    return m_control;
}

void ParameterTreeHistoryItem::setControl(const QString &control)
{
    m_control = control;
}

QVariant ParameterTreeHistoryItem::min() const
{
    return m_min;
}

void ParameterTreeHistoryItem::setMin(const QVariant &min)
{
    m_min = min;
}

QVariant ParameterTreeHistoryItem::max() const
{
    return m_max;
}

void ParameterTreeHistoryItem::setMax(const QVariant &max)
{
    m_max = max;
}