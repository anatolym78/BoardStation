#include "BoardParameterSingle.h"

BoardParameterSingle::BoardParameterSingle(QObject *parent)
    : QObject(parent)
    , m_timestamp(QDateTime::currentDateTime())
{
}

BoardParameterSingle::BoardParameterSingle(const QString &label, const QString &unit, QObject *parent)
    : QObject(parent)
    , m_label(label)
    , m_unit(unit)
    , m_timestamp(QDateTime::currentDateTime())
{
}

BoardParameterSingle::BoardParameterSingle(const QString &label, const QVariant &value, const QString &unit, QObject *parent)
    : QObject(parent)
    , m_label(label)
    , m_unit(unit)
    , m_value(value)
    , m_timestamp(QDateTime::currentDateTime())
{
}

BoardParameterSingle::BoardParameterSingle(const QString &label, const QVariant &value, const QDateTime &timestamp, const QString &unit, QObject *parent)
    : QObject(parent)
    , m_label(label)
    , m_unit(unit)
    , m_value(value)
    , m_timestamp(timestamp)
{
}

void BoardParameterSingle::setLabel(const QString &label)
{
    if (m_label != label) 
    {
        m_label = label;
        emit labelChanged();
    }
}

void BoardParameterSingle::setUnit(const QString &unit)
{
    if (m_unit != unit) 
    {
        m_unit = unit;
        emit unitChanged();
    }
}

void BoardParameterSingle::setValue(const QVariant &value)
{
    if (m_value != value) 
    {
        m_value = value;
        emit valueChanged();
    }
}

void BoardParameterSingle::setTimestamp(const QDateTime &timestamp)
{
    if (m_timestamp != timestamp) 
    {
        m_timestamp = timestamp;
        emit timestampChanged();
    }
}

BoardParameterValue* BoardParameterSingle::toParameterValue(QObject *parent) const
{
    return new BoardParameterValue(m_value, m_timestamp, parent);
}

bool BoardParameterSingle::operator==(const BoardParameterSingle &other) const
{
    return m_label == other.m_label && 
           m_unit == other.m_unit && 
           m_value == other.m_value && 
           m_timestamp == other.m_timestamp;
}

bool BoardParameterSingle::operator!=(const BoardParameterSingle &other) const
{
    return !(*this == other);
}
