#ifndef BOARDPARAMETER_H
#define BOARDPARAMETER_H

#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QList>
#include "BoardParameterValue.h"

struct BoardParameter
{
    QString label;           // Название параметра
    QString unit;            // Единица измерения
    QList<BoardParameterValue> values;  // Список значений и времени их получения
    
    BoardParameter() = default;
    BoardParameter(const QString &label, const QString &unit = QString())
        : label(label), unit(unit) {}
    
    BoardParameter(const QString &label, const QVariant &value, const QString &unit = QString())
        : label(label), unit(unit) {
        values.append(BoardParameterValue(value));
    }
    
    BoardParameter(const QString &label, const QVariant &value, const QDateTime &timestamp, const QString &unit = QString())
        : label(label), unit(unit) {
        values.append(BoardParameterValue(value, timestamp));
    }
    
    // Добавление нового значения
    void addValue(const QVariant &value, const QDateTime &timestamp = QDateTime::currentDateTime()) {
        values.append(BoardParameterValue(value, timestamp));
    }
    
    // Получение последнего значения
    BoardParameterValue lastValue() const {
        if (values.isEmpty()) {
            return BoardParameterValue();
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
    
    bool operator==(const BoardParameter &other) const {
        return label == other.label && unit == other.unit && values == other.values;
    }
    
    bool operator!=(const BoardParameter &other) const {
        return !(*this == other);
    }
};

#endif // BOARDPARAMETER_H
