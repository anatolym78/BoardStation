#ifndef BOARDPARAMETERVALUE_H
#define BOARDPARAMETERVALUE_H

#include <QVariant>
#include <QDateTime>

// Структура для хранения значения параметра и времени его получения
struct BoardParameterValue
{
    QVariant value;          // Значение параметра
    QDateTime timestamp;     // Время получения параметра
    
    BoardParameterValue() = default;
    BoardParameterValue(const QVariant &value, const QDateTime &timestamp = QDateTime::currentDateTime())
        : value(value), timestamp(timestamp) {}
    
    bool operator==(const BoardParameterValue &other) const {
        return value == other.value && timestamp == other.timestamp;
    }
    
    bool operator!=(const BoardParameterValue &other) const {
        return !(*this == other);
    }
};

#endif // BOARDPARAMETERVALUE_H
