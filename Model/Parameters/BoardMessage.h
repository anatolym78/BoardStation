#ifndef BOARDMESSAGE_H
#define BOARDMESSAGE_H

#include <QObject>
#include <QDateTime>
#include <QList>
#include "BoardParameterSingle.h"

class BoardMessage : public QObject
{
    Q_OBJECT

public:
    explicit BoardMessage(const QList<BoardParameterSingle*> &parameters, 
                         const QDateTime &timestamp = QDateTime::currentDateTime(),
                         QObject *parent = nullptr);
    
    // Геттеры
    QList<BoardParameterSingle*> getParameters() const { return m_parameters; }
    QDateTime getTimestamp() const { return m_timestamp; }
    
    // Сеттеры
    void setParameters(const QList<BoardParameterSingle*> &parameters) { m_parameters = parameters; }
    void setTimestamp(const QDateTime &timestamp) { m_timestamp = timestamp; }
    
    // Методы
    bool isEmpty() const { return m_parameters.isEmpty(); }
    int parameterCount() const { return m_parameters.size(); }
    
    // Преобразование в JSON
    QJsonObject toJsonObject() const;

private:
    QList<BoardParameterSingle*> m_parameters;
    QDateTime m_timestamp;
};

#endif // BOARDMESSAGE_H
