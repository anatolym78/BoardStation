#include "ParametersParser.h"
#include "StringOutParameter.h"
#include "RangedRealOutParameter.h"
#include "ListedRealOutParameter.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

ParametersParser::ParametersParser()
{
}

QList<OutParameter*> ParametersParser::parseParameters(const QJsonArray &parametersArray)
{
    QList<OutParameter*> parameters;
    
    for (const QJsonValue &value : parametersArray) {
        if (value.isObject()) {
            QJsonObject paramObj = value.toObject();
            OutParameter *param = parseParameter(paramObj);
            if (param && param->isValid()) {
                parameters.append(param);
            } else {
                qWarning() << "ParametersParser: Параметр не прошел валидацию:" << paramObj["label"].toString();
                delete param;
            }
        }
    }
    
    qDebug() << "ParametersParser: Успешно создано" << parameters.size() << "параметров";
    return parameters;
}

OutParameter* ParametersParser::parseParameter(const QJsonObject &parameterObj)
{
    if (!parameterObj.contains("label") || !parameterObj.contains("valueType") || !parameterObj.contains("controlType")) {
        qWarning() << "ParametersParser: Параметр не содержит обязательные поля";
        return nullptr;
    }
    
    QString label = parameterObj["label"].toString();
    QString valueType = parameterObj["valueType"].toString();
    QString controlType = parameterObj["controlType"].toString();
    
    return createParameter(label, valueType, controlType, parameterObj);
}

void ParametersParser::clearParameters(QList<OutParameter*> &parameters)
{
    for (OutParameter *param : parameters) {
        delete param;
    }
    parameters.clear();
}

OutParameter* ParametersParser::createParameter(const QString &label, 
                                               const QString &valueType,
                                               const QString &controlType,
                                               const QJsonObject &parameterObj)
{
    if (valueType == "string") {
        QStringList valuesList = parseStringValuesList(parameterObj);
        return new StringOutParameter(label, QString(), valuesList, controlType);
    }
    else if (valueType == "double" || valueType == "int") {
        // Проверяем, есть ли диапазон
        double min, max, step;
        if (parseRange(parameterObj, min, max, step)) {
            return new RangedRealOutParameter(label, min, min, max, step, controlType);
        }
        
        // Проверяем, есть ли список значений
        QList<double> valuesList = parseValuesList(parameterObj);
        if (!valuesList.isEmpty()) {
            return new ListedRealOutParameter(label, valuesList.first(), valuesList, controlType);
        }
        
        // Простой вещественный параметр
        return new RangedRealOutParameter(label, 0.0, -1000.0, 1000.0, 1.0, controlType);
    }
    else if (valueType == "bool") {
        // Для булевых параметров используем QCheckBox
        return new StringOutParameter(label, "false", QStringList{"true", "false"}, "QCheckBox");
    }
    
    qWarning() << "ParametersParser: Неподдерживаемый тип значения:" << valueType;
    return nullptr;
}

bool ParametersParser::parseRange(const QJsonObject &parameterObj, 
                                 double &minimum, double &maximum, double &step)
{
    if (!parameterObj.contains("range")) {
        return false;
    }
    
    QJsonValue rangeValue = parameterObj["range"];
    if (rangeValue.isString()) {
        QString rangeStr = rangeValue.toString();
        QStringList parts = rangeStr.split(",");
        if (parts.size() >= 2) {
            bool ok1, ok2, ok3;
            minimum = parts[0].trimmed().toDouble(&ok1);
            maximum = parts[1].trimmed().toDouble(&ok2);
            step = (parts.size() > 2) ? parts[2].trimmed().toDouble(&ok3) : 1.0;
            
            if (ok1 && ok2 && (parts.size() <= 2 || ok3)) {
                return true;
            }
        }
    }
    
    return false;
}

QList<double> ParametersParser::parseValuesList(const QJsonObject &parameterObj)
{
    QList<double> values;
    
    if (!parameterObj.contains("values")) {
        return values;
    }
    
    QJsonValue valuesValue = parameterObj["values"];
    if (valuesValue.isArray()) {
        QJsonArray valuesArray = valuesValue.toArray();
        for (const QJsonValue &value : valuesArray) {
            if (value.isDouble()) {
                values.append(value.toDouble());
            }
        }
    }
    
    return values;
}

QStringList ParametersParser::parseStringValuesList(const QJsonObject &parameterObj)
{
    QStringList values;
    
    if (!parameterObj.contains("values")) {
        return values;
    }
    
    QJsonValue valuesValue = parameterObj["values"];
    if (valuesValue.isArray()) {
        QJsonArray valuesArray = valuesValue.toArray();
        for (const QJsonValue &value : valuesArray) {
            if (value.isString()) {
                values.append(value.toString());
            }
        }
    }
    
    return values;
}
