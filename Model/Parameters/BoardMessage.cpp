#include "BoardMessage.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

BoardMessage::BoardMessage(const QList<BoardParameterSingle*> &parameters, 
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
    for (BoardParameterSingle *param : m_parameters) {
        if (param) {
            QJsonObject paramObj;
            paramObj["label"] = param->label();
            paramObj["value"] = QJsonValue::fromVariant(param->value());
            parametersArray.append(paramObj);
        }
    }
    
    obj["Parameters"] = parametersArray;
    
    return obj;
}
