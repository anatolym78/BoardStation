#include "BoardMessage.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

BoardMessage::BoardMessage(const QList<BoardParameter*> &parameters, 
                           const QDateTime &timestamp,
                           QObject *parent)
    : QObject(parent)
    , m_parameters(parameters)
    , m_timestamp(timestamp)
{
}

QJsonObject BoardMessage::toJsonObject() const
{
    QJsonObject obj;
    
    // Добавляем временную метку
    obj["TimeStamp"] = m_timestamp.toString("dd.MM.yyyy hh:mm:ss.zzz");
    
    // Добавляем параметры
    QJsonArray parametersArray;
    for (BoardParameter *param : m_parameters) {
        if (param) {
            QJsonObject paramObj;
            paramObj["label"] = param->label();
            
            // Добавляем последнее значение параметра
            if (param->hasValues()) {
                paramObj["value"] = QJsonValue::fromVariant(param->lastValueData());
            }
            
            parametersArray.append(paramObj);
        }
    }
    
    obj["Parameters"] = parametersArray;
    
    return obj;
}
