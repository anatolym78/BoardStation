#ifndef PARAMETER_H
#define PARAMETER_H

#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QList>

// Структура для хранения значения параметра и времени его получения
struct ParameterValue
{
    QVariant value;          // Значение параметра
    QDateTime timestamp;     // Время получения параметра
    
    ParameterValue() = default;
    ParameterValue(const QVariant &value, const QDateTime &timestamp = QDateTime::currentDateTime())
        : value(value), timestamp(timestamp) {}
    
    bool operator==(const ParameterValue &other) const {
        return value == other.value && timestamp == other.timestamp;
    }
    
    bool operator!=(const ParameterValue &other) const {
        return !(*this == other);
    }
};

struct Parameter
{
    QString label;           // Название параметра
    QString unit;            // Единица измерения
    QList<ParameterValue> values;  // Список значений и времени их получения
    
    Parameter() = default;
    Parameter(const QString &label, const QString &unit = QString())
        : label(label), unit(unit) {}
    
    Parameter(const QString &label, const QVariant &value, const QString &unit = QString())
        : label(label), unit(unit) {
        values.append(ParameterValue(value));
    }
    
    Parameter(const QString &label, const QVariant &value, const QDateTime &timestamp, const QString &unit = QString())
        : label(label), unit(unit) {
        values.append(ParameterValue(value, timestamp));
    }
    
    // Добавление нового значения
    void addValue(const QVariant &value, const QDateTime &timestamp = QDateTime::currentDateTime()) {
        values.append(ParameterValue(value, timestamp));
    }
    
    // Получение последнего значения
    ParameterValue lastValue() const {
        if (values.isEmpty()) {
            return ParameterValue();
        }
        return values.last();
    }
    
    // Получение последнего значения как QVariant
    QVariant lastValueData() const {
        if (values.isEmpty()) {
            return QVariant();
        }
        return values.last().value;
    }
    
    // Получение времени последнего значения
    QDateTime lastTimestamp() const {
        if (values.isEmpty()) {
            return QDateTime();
        }
        return values.last().timestamp;
    }
    
    // Проверка наличия значений
    bool hasValues() const {
        return !values.isEmpty();
    }
    
    // Количество значений
    int valueCount() const {
        return values.size();
    }
    
    bool operator==(const Parameter &other) const {
        return label == other.label && unit == other.unit && values == other.values;
    }
    
    bool operator!=(const Parameter &other) const {
        return !(*this == other);
    }
};

#endif // PARAMETER_H
