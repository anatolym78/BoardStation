#include "BoardParameter.h"
#include <QDebug>

BoardParameter::BoardParameter(QObject *parent)
    : QObject(parent)
{
}

BoardParameter::BoardParameter(const QString &label, const QString &unit, QObject *parent)
    : QObject(parent)
    , m_label(label)
    , m_unit(unit)
{
}

BoardParameter::BoardParameter(const QString &label, const QVariant &value, const QString &unit, QObject *parent)
    : QObject(parent)
    , m_label(label)
    , m_unit(unit)
{
    addValue(value);
}

BoardParameter::BoardParameter(const QString &label, const QVariant &value, const QDateTime &timestamp, const QString &unit, QObject *parent)
    : QObject(parent)
    , m_label(label)
    , m_unit(unit)
{
    addValue(value, timestamp);
}

BoardParameter::~BoardParameter()
{
    qDeleteAll(m_values);
    qDebug() << "BoardParameter: Destructor called for" << m_label;
}

QVariant BoardParameter::lastValueData() const
{
    if (m_values.isEmpty()) {
        return QVariant();
    }
    return m_values.last()->value();
}

QDateTime BoardParameter::lastTimestamp() const
{
    if (m_values.isEmpty()) {
        return QDateTime();
    }
    return m_values.last()->timestamp();
}

void BoardParameter::setLabel(const QString &label)
{
    if (m_label != label) {
        m_label = label;
        emit labelChanged();
    }
}

void BoardParameter::setUnit(const QString &unit)
{
    if (m_unit != unit) {
        m_unit = unit;
        emit unitChanged();
    }
}

void BoardParameter::addValue(const QVariant &value, const QDateTime &timestamp)
{
    BoardParameterValue *newValue = new BoardParameterValue(value, timestamp, this);
    m_values.append(newValue);
    
    emit valuesChanged();
    emit lastValueDataChanged();
    emit lastTimestampChanged();
    emit hasValuesChanged();
    emit valueCountChanged();
    
    qDebug() << "BoardParameter: Added value" << value << "for parameter" << m_label;
}

BoardParameterValue* BoardParameter::lastValue() const
{
    if (m_values.isEmpty()) {
        return nullptr;
    }
    return m_values.last();
}

bool BoardParameter::operator==(const BoardParameter &other) const
{
    return m_label == other.m_label && m_unit == other.m_unit && m_values == other.m_values;
}

bool BoardParameter::operator!=(const BoardParameter &other) const
{
    return !(*this == other);
}
