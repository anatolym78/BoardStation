#ifndef BOARDPARAMETER_H
#define BOARDPARAMETER_H

#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QList>
#include <QObject>
#include "BoardParameterValue.h"

class BoardParameter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(QString unit READ unit WRITE setUnit NOTIFY unitChanged)
    Q_PROPERTY(QList<BoardParameterValue*> values READ values NOTIFY valuesChanged)
    Q_PROPERTY(QVariant lastValueData READ lastValueData NOTIFY lastValueDataChanged)
    Q_PROPERTY(QDateTime lastTimestamp READ lastTimestamp NOTIFY lastTimestampChanged)
    Q_PROPERTY(bool hasValues READ hasValues NOTIFY hasValuesChanged)
    Q_PROPERTY(int valueCount READ valueCount NOTIFY valueCountChanged)

public:
    explicit BoardParameter(QObject *parent = nullptr);
    BoardParameter(const QString &label, const QString &unit = QString(), QObject *parent = nullptr);
    BoardParameter(const QString &label, const QVariant &value, const QString &unit = QString(), QObject *parent = nullptr);
    BoardParameter(const QString &label, const QVariant &value, const QDateTime &timestamp, const QString &unit = QString(), QObject *parent = nullptr);
    
    // Деструктор для очистки памяти
    ~BoardParameter();
    
    // Геттеры
    QString label() const { return m_label; }
    QString unit() const { return m_unit; }
    QList<BoardParameterValue*> values() const { return m_values; }
    QVariant lastValueData() const;
    QDateTime lastTimestamp() const;
    bool hasValues() const { return !m_values.isEmpty(); }
    int valueCount() const { return m_values.size(); }
    
    // Сеттеры
    void setLabel(const QString &label);
    void setUnit(const QString &unit);
    
    // Добавление нового значения
    void addValue(const QVariant &value, const QDateTime &timestamp = QDateTime::currentDateTime());
    
    // Получение последнего значения
    BoardParameterValue* lastValue() const;
    
    // Операторы сравнения
    bool operator==(const BoardParameter &other) const;
    bool operator!=(const BoardParameter &other) const;

signals:
    void labelChanged();
    void unitChanged();
    void valuesChanged();
    void lastValueDataChanged();
    void lastTimestampChanged();
    void hasValuesChanged();
    void valueCountChanged();

private:
    QString m_label;           // Название параметра
    QString m_unit;            // Единица измерения
    QList<BoardParameterValue*> m_values;  // Список значений и времени их получения
};

#endif // BOARDPARAMETER_H
