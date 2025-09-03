#include "BoardDataJsonGenerator.h"
#include <QDebug>

BoardDataJsonGenerator::BoardDataJsonGenerator(QObject *parent)
    : QObject(parent)
{
}

QString BoardDataJsonGenerator::createJsonString(const QList<BoardParameter*> &parameters)
{
    QJsonDocument doc = createJsonDocument(parameters);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    
    if (jsonString.isEmpty()) {
        emit writingError("Не удалось создать JSON строку");
    } else {
        emit writingSuccess(jsonString);
    }
    
    return jsonString;
}

QJsonDocument BoardDataJsonGenerator::createJsonDocument(const QList<BoardParameter*> &parameters)
{
    QJsonArray array = createJsonArray(parameters);
    return QJsonDocument(array);
}

QJsonArray BoardDataJsonGenerator::createJsonArray(const QList<BoardParameter*> &parameters)
{
    QJsonArray array;
    
    for (BoardParameter *param : parameters) {
        if (param) {
            QJsonObject obj = parameterToJsonObject(*param);
            array.append(obj);
        }
    }
    
    return array;
}

QJsonObject BoardDataJsonGenerator::parameterToJsonObject(const BoardParameter &parameter)
{
    QJsonObject obj;
    obj["label"] = parameter.label();
    obj["unit"] = parameter.unit();
    
    // Добавляем последнее значение и время
    if (parameter.hasValues()) {
        obj["value"] = QJsonValue::fromVariant(parameter.lastValueData());
        obj["timestamp"] = parameter.lastTimestamp().toString(Qt::ISODate);
    }
    
    // Добавляем историю значений, если она есть
    if (parameter.valueCount() > 1) {
        QJsonArray valuesArray;
        for (BoardParameterValue *value : parameter.values()) {
            if (value) {
                QJsonObject valueObj;
                valueObj["value"] = QJsonValue::fromVariant(value->value());
                valueObj["timestamp"] = value->timestamp().toString(Qt::ISODate);
                valuesArray.append(valueObj);
            }
        }
        obj["values"] = valuesArray;
    }
    
    return obj;
}
