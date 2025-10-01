#include "UplinkParametersParser.h"
#include "IntUplinkParameter.h"
#include "DoubleUplinkParameter.h"
#include "StringUplinkParameter.h"
#include "BoolUplinkParameter.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <functional>

UplinkParametersParser::UplinkParametersParser()
{
}

QList<BasicUplinkParameter*> UplinkParametersParser::parseParameters(const QJsonArray &parametersArray)
{
    QList<BasicUplinkParameter*> parameters;
    
    for (const QJsonValue &value : parametersArray) 
    {
        if (value.isObject()) 
        {
            QJsonObject paramObj = value.toObject();
            BasicUplinkParameter *param = parseParameter(paramObj);
            if (param && param->isValid()) 
            {
                parameters.append(param);
            } 
            else 
            {
                qWarning() << "UplinkParametersParser: Параметр не прошел валидацию:" << paramObj["label"].toString();
                delete param;
            }
        }
    }
    
    qDebug() << "UplinkParametersParser: Успешно создано" << parameters.size() << "параметров";
    return parameters;
}

BasicUplinkParameter* UplinkParametersParser::parseParameter(const QJsonObject &parameterObj)
{
    if (!parameterObj.contains("label") || !parameterObj.contains("valueType")) 
    {
        qWarning() << "UplinkParametersParser: Параметр не содержит обязательные поля";
        return nullptr;
    }
    
    QString label = parameterObj["label"].toString();
    QString valueType = parameterObj["valueType"].toString();
    QString controlType = parameterObj.contains("controlType") ? 
                         parameterObj["controlType"].toString() : "TextEdit";
    
    // Корректируем тип контрола если он не подходит
    QString correctedControlType = correctControlType(valueType, controlType, parameterObj);
    
    return createParameter(label, valueType, correctedControlType, parameterObj);
}

void UplinkParametersParser::clearParameters(QList<BasicUplinkParameter*> &parameters)
{
    for (BasicUplinkParameter *param : parameters) 
    {
        delete param;
    }
    parameters.clear();
}

BasicUplinkParameter* UplinkParametersParser::createParameter(const QString &label, 
                                                           const QString &valueType,
                                                           const QString &controlType,
                                                           const QJsonObject &parameterObj)
{
    // Получаем значение по умолчанию для параметра
    QJsonValue defaultValue = parameterObj.contains("value") ? 
                             parameterObj["value"] : 
                             getDefaultValueForType(valueType);
    
    if (valueType == "string") 
    {
        QStringList valuesList = parseStringValuesList(parameterObj);
        StringUplinkParameter *param = new StringUplinkParameter(label, controlType);
        
        // Устанавливаем значение по умолчанию
        if (defaultValue.isString())
        {
            param->setValue(defaultValue.toString());
        }
        
        if (!valuesList.isEmpty())
        {
            param->setValues(valuesList);
        }
        
        return param;
    }
    else if (valueType == "int") 
    {
        IntUplinkParameter *param = new IntUplinkParameter(label, controlType);
        
        // Устанавливаем значение по умолчанию
        if (defaultValue.isDouble())
        {
            param->setValue(static_cast<int>(defaultValue.toInt()));
        }
        
        // Проверяем, есть ли диапазон
        double min, max, step;
        if (parseRange(parameterObj, min, max, step)) 
        {
            param->setRange(static_cast<int>(min), static_cast<int>(max));
            if (step != 1.0) // Если шаг указан и не равен 1
            {
                param->setStep(static_cast<int>(step));
            }
        }
        
        // Проверяем, есть ли список значений
        QList<double> valuesList = parseValuesList(parameterObj);
        if (!valuesList.isEmpty())
        {
            QList<int> intValues;
            for (double val : valuesList)
            {
                intValues.append(static_cast<int>(val));
            }
            param->setValues(intValues);
        }
        
        return param;
    }
    else if (valueType == "double") 
    {
        DoubleUplinkParameter *param = new DoubleUplinkParameter(label, controlType);
        
        // Устанавливаем значение по умолчанию
        if (defaultValue.isDouble())
        {
            param->setValue(defaultValue.toDouble());
        }
        
        // Проверяем, есть ли диапазон
        double min, max, step;
        if (parseRange(parameterObj, min, max, step)) 
        {
            param->setRange(min, max);
            if (step != 1.0) // Если шаг указан и не равен 1
            {
                param->setStep(step);
            }
        }
        
        // Проверяем, есть ли список значений
        QList<double> valuesList = parseValuesList(parameterObj);
        if (!valuesList.contains(defaultValue.toDouble()) && defaultValue.isDouble())
        {
            param->setValue(defaultValue.toDouble());
        }
        
        if (!valuesList.isEmpty())
        {
            param->setValues(valuesList);
        }
        
        return param;
    }
    else if (valueType == "bool") 
    {
        QList<bool> valuesList = parseBoolValuesList(parameterObj);
        
        // Получаем алиасы для булевых значений
        QString falseAlias = "Off";
        QString trueAlias = "On";
        
        if (parameterObj.contains("falseAlias")) 
        {
            falseAlias = parameterObj["falseAlias"].toString();
        }
        if (parameterObj.contains("trueAlias")) 
        {
            trueAlias = parameterObj["trueAlias"].toString();
        }
        
        BoolUplinkParameter *param = new BoolUplinkParameter(label, controlType, falseAlias, trueAlias);
        
        // Устанавливаем значение по умолчанию
        if (defaultValue.isBool())
        {
            param->setValue(defaultValue.toBool());
        }
        
        if (!valuesList.isEmpty())
        {
            param->setValues(valuesList);
        }
        
        return param;
    }
    
    qWarning() << "UplinkParametersParser: Неподдерживаемый тип значения:" << valueType;
    return nullptr;
}

bool UplinkParametersParser::parseRange(const QJsonObject &parameterObj, 
                                       double &minimum, double &maximum, double &step)
{
    if (!parameterObj.contains("range")) 
    {
        return false;
    }
    
    QJsonValue rangeValue = parameterObj["range"];
    if (rangeValue.isString()) 
    {
        QString rangeStr = rangeValue.toString();
        QStringList parts = rangeStr.split(",");
        if (parts.size() >= 2) 
        {
            bool ok1, ok2, ok3;
            minimum = parts[0].trimmed().toDouble(&ok1);
            maximum = parts[1].trimmed().toDouble(&ok2);
            
            // Шаг может быть не указан
            if (parts.size() > 2)
            {
                step = parts[2].trimmed().toDouble(&ok3);
                if (!ok3)
                {
                    step = 1.0; // Значение по умолчанию
                }
            }
            else
            {
                step = 1.0; // Значение по умолчанию
            }
            
            if (ok1 && ok2) 
            {
                return true;
            }
        }
    }
    
    return false;
}

QList<double> UplinkParametersParser::parseValuesList(const QJsonObject &parameterObj)
{
    QList<double> values;
    
    if (!parameterObj.contains("values")) 
    {
        return values;
    }
    
    QJsonValue valuesValue = parameterObj["values"];
    if (valuesValue.isArray()) 
    {
        QJsonArray valuesArray = valuesValue.toArray();
        for (const QJsonValue &value : valuesArray) 
        {
            if (value.isDouble()) 
            {
                values.append(value.toDouble());
            }
        }
    }
    
    return values;
}

QStringList UplinkParametersParser::parseStringValuesList(const QJsonObject &parameterObj)
{
    QStringList values;
    
    if (!parameterObj.contains("values")) 
    {
        return values;
    }
    
    QJsonValue valuesValue = parameterObj["values"];
    if (valuesValue.isArray()) 
    {
        QJsonArray valuesArray = valuesValue.toArray();
        for (const QJsonValue &value : valuesArray) 
        {
            if (value.isString()) 
            {
                values.append(value.toString());
            }
        }
    }
    
    return values;
}

QList<bool> UplinkParametersParser::parseBoolValuesList(const QJsonObject &parameterObj)
{
    QList<bool> values;
    
    if (!parameterObj.contains("values")) 
    {
        return values;
    }
    
    QJsonValue valuesValue = parameterObj["values"];
    if (valuesValue.isArray()) 
    {
        QJsonArray valuesArray = valuesValue.toArray();
        for (const QJsonValue &value : valuesArray) 
        {
            if (value.isBool()) 
            {
                values.append(value.toBool());
            }
        }
    }
    
    return values;
}

QString UplinkParametersParser::correctControlType(const QString &valueType, const QString &controlType, const QJsonObject &parameterObj)
{
    // Если тип контрола уже подходит, возвращаем его
    if (isControlTypeSuitable(controlType, valueType, parameterObj)) 
    {
        return controlType;
    }
    
    // Выбираем наиболее подходящий тип контрола
    if (valueType == "bool") 
    {
        QStringList valuesList = parseStringValuesList(parameterObj);
        if (valuesList.size() == 2) 
        {
            return "ComboBox";
        }
        // Если исходный controlType был CheckBox или Switch, сохраняем его
        if (controlType == "CheckBox" || controlType == "Switch") 
        {
            return controlType;
        }
        return "Switch"; // По умолчанию для булевых параметров
    }
    else if (valueType == "string") 
    {
        QStringList valuesList = parseStringValuesList(parameterObj);
        if (!valuesList.isEmpty()) 
        {
            return "ComboBox";
        }
        return "TextEdit";
    }
    else if (valueType == "int" || valueType == "double") 
    {
        double min, max, step;
        QList<double> valuesList = parseValuesList(parameterObj);
        
        // Если есть список значений
        if (!valuesList.isEmpty()) 
        {
            int possibleValuesCount = calculatePossibleValuesCount(min, max, step);
            if (parseRange(parameterObj, min, max, step) && possibleValuesCount <= 10) 
            {
                return "ComboBox";
            }
            return "ComboBox";
        }
        
        // Если есть диапазон, но нет списка
        if (parseRange(parameterObj, min, max, step)) 
        {
            // Проверяем, есть ли шаг
            if (step != 1.0) 
            {
                int possibleValuesCount = calculatePossibleValuesCount(min, max, step);
                if (possibleValuesCount <= 10) 
                {
                    return "ComboBox";
                }
                else 
                {
                    return "Slider";
                }
            }
            else 
            {
                return "Slider";
            }
        }
        
        return "TextEdit";
    }
    
    return "TextEdit"; // По умолчанию
}

QJsonValue UplinkParametersParser::getDefaultValueForType(const QString &valueType)
{
    if (valueType == "int" || valueType == "double") 
    {
        return QJsonValue(0);
    }
    else if (valueType == "string") 
    {
        return QJsonValue(QString(""));
    }
    else if (valueType == "bool") 
    {
        return QJsonValue(false);
    }
    
    return QJsonValue();
}

bool UplinkParametersParser::isControlTypeSuitable(const QString &controlType, const QString &valueType, const QJsonObject &parameterObj)
{
    // Используем map для более элегантного решения
    static const QHash<QString, std::function<bool(const QString&, const QJsonObject&)>> controlTypeMap = {
        {"TextEdit", [this](const QString& vt, const QJsonObject& po) { return isTextEditSuitable(vt, po); }},
        {"Slider", [this](const QString& vt, const QJsonObject& po) { return isSliderSuitable(vt, po); }},
        {"CheckBox", [this](const QString& vt, const QJsonObject& po) { return isCheckBoxSuitable(vt, po); }},
        {"Switch", [this](const QString& vt, const QJsonObject& po) { return isSwitchSuitable(vt, po); }},
        {"SpinBox", [this](const QString& vt, const QJsonObject& po) { return isSpinBoxSuitable(vt, po); }},
        {"ComboBox", [this](const QString& vt, const QJsonObject& po) { return isComboBoxSuitable(vt, po); }}
    };
    
    auto it = controlTypeMap.find(controlType);
    if (it != controlTypeMap.end()) 
    {
        return it.value()(valueType, parameterObj);
    }
    
    return false;
}

int UplinkParametersParser::calculatePossibleValuesCount(double min, double max, double step)
{
    if (step <= 0 || min > max) 
    {
        return 0;
    }
    
    // Рассчитываем количество возможных значений
    double range = max - min;
    int count = static_cast<int>(range / step) + 1;
    
    return count;
}

// Реализации отдельных функций для проверки типов контролов

bool UplinkParametersParser::isTextEditSuitable(const QString &valueType, const QJsonObject &parameterObj)
{
    Q_UNUSED(valueType)
    Q_UNUSED(parameterObj)
    return true; // TextEdit подходит для всех типов
}

bool UplinkParametersParser::isSliderSuitable(const QString &valueType, const QJsonObject &parameterObj)
{
    if (valueType == "bool" || valueType == "string") 
    {
        return false;
    }
    double min, max, step;
    return parseRange(parameterObj, min, max, step);
}

bool UplinkParametersParser::isCheckBoxSuitable(const QString &valueType, const QJsonObject &parameterObj)
{
    if (valueType == "bool") 
    {
        QStringList valuesList = parseStringValuesList(parameterObj);
        return valuesList.size() == 2;
    }
    
    // Для int и double можно использовать CheckBox только если есть список из двух значений
    if (valueType == "int" || valueType == "double") 
    {
        QList<double> valuesList = parseValuesList(parameterObj);
        return valuesList.size() == 2;
    }
    
    return false;
}

bool UplinkParametersParser::isSwitchSuitable(const QString &valueType, const QJsonObject &parameterObj)
{
    // Switch работает идентично CheckBox
    return isCheckBoxSuitable(valueType, parameterObj);
}

bool UplinkParametersParser::isSpinBoxSuitable(const QString &valueType, const QJsonObject &parameterObj)
{
    if (valueType == "bool" || valueType == "string") 
    {
        return false;
    }
    
    // SpinBox подходит если указан шаг
    double min, max, step;
    if (parseRange(parameterObj, min, max, step)) 
    {
        return step != 1.0;
    }
    
    // Также проверяем в самом объекте параметра
    if (parameterObj.contains("range")) 
    {
        QString rangeStr = parameterObj["range"].toString();
        QStringList parts = rangeStr.split(",");
        return parts.size() > 2 && parts[2].trimmed().toDouble() != 1.0;
    }
    
    return false;
}

bool UplinkParametersParser::isComboBoxSuitable(const QString &valueType, const QJsonObject &parameterObj)
{
    if (valueType == "bool") 
    {
        QList<bool> valuesList = parseBoolValuesList(parameterObj);
        return !valuesList.isEmpty();
    }
    else if (valueType == "string") 
    {
        QStringList valuesList = parseStringValuesList(parameterObj);
        return !valuesList.isEmpty();
    }
    else if (valueType == "int" || valueType == "double") 
    {
        // Для int и double ComboBox подходит если есть список или диапазон с ограниченным количеством значений
        QList<double> valuesList = parseValuesList(parameterObj);
        if (!valuesList.isEmpty()) 
        {
            return true;
        }
        
        double min, max, step;
        if (parseRange(parameterObj, min, max, step)) 
        {
            int valuesCount = calculatePossibleValuesCount(min, max, step);
            return valuesCount <= 10;
        }
    }
    
    return false;
}
