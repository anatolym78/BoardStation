#include "BoardParameterHistory.h"
#include <QDebug>

BoardParameterHistory::BoardParameterHistory(QObject *parent)
    : QObject(parent)
{
}

BoardParameterHistory::BoardParameterHistory(const QString &label, const QString &unit, QObject *parent)
    : QObject(parent)
    , m_label(label)
    , m_unit(unit)
{
}

BoardParameterHistory::BoardParameterHistory(const QString &label, const QVariant &value, const QString &unit, QObject *parent)
    : QObject(parent)
    , m_label(label)
    , m_unit(unit)
{
    addValue(value);
}

BoardParameterHistory::BoardParameterHistory(const QString &label, const QVariant &value, const QDateTime &timestamp, const QString &unit, QObject *parent)
    : QObject(parent)
    , m_label(label)
    , m_unit(unit)
{
    addValue(value, timestamp);
}

BoardParameterHistory::~BoardParameterHistory()
{
    qDeleteAll(m_values);
    //qDebug() << "BoardParameterHistory: Destructor called for" << m_label;
}

QVariant BoardParameterHistory::lastValueData() const
{
    if (m_values.isEmpty()) 
    {
        return QVariant();
    }
    return m_values.last()->value();
}

QDateTime BoardParameterHistory::lastTimestamp() const
{
    if (m_values.isEmpty()) 
    {
        return QDateTime();
    }
    return m_values.last()->timestamp();
}

void BoardParameterHistory::setLabel(const QString &label)
{
    if (m_label != label) 
    {
        m_label = label;
        emit labelChanged();
    }
}

void BoardParameterHistory::setUnit(const QString &unit)
{
    if (m_unit != unit) 
    {
        m_unit = unit;
        emit unitChanged();
    }
}

void BoardParameterHistory::addValue(const QVariant &value, const QDateTime &timestamp)
{
    BoardParameterValue *newValue = new BoardParameterValue(value, timestamp, this);
    m_values.append(newValue);
    
    emit valuesChanged();
    emit lastValueDataChanged();
    emit lastTimestampChanged();
    emit hasValuesChanged();
    emit valueCountChanged();
    
    //qDebug() << "BoardParameterHistory: Added value" << value << "for parameter" << m_label;
}

bool BoardParameterHistory::addParameter(const BoardParameterSingle *parameter)
{
    if (!parameter) 
    {
        qWarning() << "BoardParameterHistory: Cannot add null parameter";
        return false;
    }
    
    // Проверяем соответствие меток
    if (!m_label.isEmpty() && m_label != parameter->label()) 
    {
        qWarning() << "BoardParameterHistory: Label mismatch. Expected:" << m_label << "Got:" << parameter->label();
        return false;
    }
    
    // Если метка ещё не установлена, устанавливаем её
    if (m_label.isEmpty()) 
    {
        setLabel(parameter->label());
    }
    
    // Если единица измерения ещё не установлена, устанавливаем её
    if (m_unit.isEmpty()) 
    {
        setUnit(parameter->unit());
    }
    
    // Добавляем значение из параметра
    addValue(parameter->value(), parameter->timestamp());
    
    return true;
}

BoardParameterValue* BoardParameterHistory::lastValue() const
{
    if (m_values.isEmpty()) 
    {
        return nullptr;
    }
    return m_values.last();
}

bool BoardParameterHistory::operator==(const BoardParameterHistory &other) const
{
    return m_label == other.m_label && m_unit == other.m_unit && m_values == other.m_values;
}

bool BoardParameterHistory::operator!=(const BoardParameterHistory &other) const
{
    return !(*this == other);
}
