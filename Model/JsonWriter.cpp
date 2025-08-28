#include "JsonWriter.h"
#include <QDebug>

JsonWriter::JsonWriter(QObject *parent)
    : QObject(parent)
{
}

QString JsonWriter::createJsonString(const QList<Parameter> &parameters)
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

QJsonDocument JsonWriter::createJsonDocument(const QList<Parameter> &parameters)
{
    QJsonArray array = createJsonArray(parameters);
    return QJsonDocument(array);
}

QJsonArray JsonWriter::createJsonArray(const QList<Parameter> &parameters)
{
    QJsonArray array;
    
    for (const Parameter &param : parameters) {
        QJsonObject obj = parameterToJsonObject(param);
        array.append(obj);
    }
    
    return array;
}

QJsonObject JsonWriter::parameterToJsonObject(const Parameter &parameter)
{
    QJsonObject obj;
    obj["label"] = parameter.label;
    obj["unit"] = parameter.unit;
    
    // Добавляем последнее значение и время
    if (parameter.hasValues()) {
        obj["value"] = QJsonValue::fromVariant(parameter.lastValueData());
        obj["timestamp"] = parameter.lastTimestamp().toString(Qt::ISODate);
    }
    
    // Добавляем историю значений, если она есть
    if (parameter.valueCount() > 1) {
        QJsonArray valuesArray;
        for (const ParameterValue &value : parameter.values) {
            QJsonObject valueObj;
            valueObj["value"] = QJsonValue::fromVariant(value.value);
            valueObj["timestamp"] = value.timestamp.toString(Qt::ISODate);
            valuesArray.append(valueObj);
        }
        obj["values"] = valuesArray;
    }
    
    return obj;
}
