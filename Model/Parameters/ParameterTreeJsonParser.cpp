#include "ParameterTreeJsonParser.h"
#include "Tree/ParameterTreeStorage.h"
#include "Tree/ParameterTreeGroupItem.h"
#include "Tree/ParameterTreeHistoryItem.h"
#include "Tree/ParameterTreeArrayItem.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QDateTime>

ParameterTreeJsonParser::ParameterTreeJsonParser(QObject *parent)
    : QObject(parent)
{
}

ParameterTreeStorage* ParameterTreeJsonParser::parseJson(const QString &jsonString)
{
    auto root = new ParameterTreeStorage(this);
    updateJson(jsonString, root);
    return root;
}

void ParameterTreeJsonParser::updateJson(const QString &jsonString, ParameterTreeStorage *root)
{
    m_lastError.clear();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        m_lastError = "Json parse error: " + parseError.errorString();
        return;
    }

    QJsonArray array;
    
    // Поддержка формата с объектом, содержащим поле "Parameters"
    if (doc.isObject())
    {
        QJsonObject obj = doc.object();
        if (obj.contains("Parameters") && obj["Parameters"].isArray())
        {
            array = obj["Parameters"].toArray();
        }
        else
        {
            m_lastError = "JSON object does not contain 'Parameters' array.";
            return;
        }
    }
    // Поддержка формата с массивом на верхнем уровне
    else if (doc.isArray())
    {
        array = doc.array();
    }
    else
    {
        m_lastError = "Root element is neither a JSON array nor an object with 'Parameters' field.";
        return;
    }

    updateJsonFromArray(array, root);
}

void ParameterTreeJsonParser::updateJsonFromArray(const QJsonArray &jsonArray, ParameterTreeStorage *root)
{
    m_lastError.clear();
    
    for (const QJsonValue &value : jsonArray)
    {
        if (value.isObject())
        {
            processJsonObject(value.toObject(), root);
        }
    }
}

QString ParameterTreeJsonParser::getLastError() const
{
    return m_lastError;
}

void ParameterTreeJsonParser::processJsonObject(const QJsonObject &jsonObject, ParameterTreeItem *parent)
{
    if (!jsonObject.contains("label") || !jsonObject.contains("value"))
    {
        return; // Пропускаем объекты без обязательных полей
    }

    QString label = jsonObject["label"].toString();
    QJsonValue value = jsonObject["value"];

    // Извлекаем дополнительные поля для ParameterTreeHistoryItem
    QString control;
    QVariant minValue;
    QVariant maxValue;

    if (jsonObject.contains("control"))
    {
        control = jsonObject["control"].toString();
    }

    if (jsonObject.contains("min"))
    {
        minValue = convertJsonValue(jsonObject["min"]);
    }

    if (jsonObject.contains("max"))
    {
        maxValue = convertJsonValue(jsonObject["max"]);
    }

    processValue(label, value, parent, control, minValue, maxValue);
}

void ParameterTreeJsonParser::processValue(const QString &key, const QJsonValue &value, ParameterTreeItem *parent, 
                                           const QString &control, const QVariant &min, const QVariant &max)
{
    QStringList parts = key.split('.');
    ParameterTreeItem *currentItem = parent;

    for (int i = 0; i < parts.size() - 1; ++i)
    {
        const QString &part = parts[i];
        ParameterTreeItem *child = currentItem->findChildByLabel(part);
        if (!child)
        {
            child = new ParameterTreeGroupItem(part, currentItem);
            currentItem->appendChild(child);
        }
        currentItem = child;
    }

    QString finalKey = parts.last();

    if (value.isArray())
    {
        ParameterTreeItem *arrayItem = currentItem->findChildByLabel(finalKey);
        if (!arrayItem)
        {
            arrayItem = new ParameterTreeArrayItem(finalKey, currentItem);
            currentItem->appendChild(arrayItem);
        }

        QJsonArray array = value.toArray();
        for (int i = 0; i < array.size(); ++i)
        {
            QString itemKey = QString::number(i);
            ParameterTreeHistoryItem *historyItem = static_cast<ParameterTreeHistoryItem*>(arrayItem->findChildByLabel(itemKey));
            if (!historyItem)
            {
                historyItem = new ParameterTreeHistoryItem(itemKey, arrayItem);
                arrayItem->appendChild(historyItem);
            }
            historyItem->addValue(convertJsonValue(array[i]), QDateTime::currentDateTime());
            // Устанавливаем дополнительные поля для элементов массива
            if (!control.isEmpty())
            {
                historyItem->setControl(control);
            }
            if (min.isValid())
            {
                historyItem->setMin(min);
            }
            if (max.isValid())
            {
                historyItem->setMax(max);
            }
        }
    }
    else
    {
        ParameterTreeHistoryItem *historyItem = static_cast<ParameterTreeHistoryItem*>(currentItem->findChildByLabel(finalKey));
        if (!historyItem)
        {
            historyItem = new ParameterTreeHistoryItem(finalKey, currentItem);
            currentItem->appendChild(historyItem);
        }
        historyItem->addValue(convertJsonValue(value), QDateTime::currentDateTime());
        // Устанавливаем дополнительные поля
        if (!control.isEmpty())
        {
            historyItem->setControl(control);
        }
        if (min.isValid())
        {
            historyItem->setMin(min);
        }
        if (max.isValid())
        {
            historyItem->setMax(max);
        }
    }
}

QVariant ParameterTreeJsonParser::convertJsonValue(const QJsonValue &jsonValue)
{
    switch (jsonValue.type())
    {
        case QJsonValue::Bool:
            return jsonValue.toBool();
        case QJsonValue::Double:
            {
                QString str = QString::number(jsonValue.toDouble(), 'f', 10);
                if (str.contains('.'))
                {
                    return jsonValue.toDouble();
                }
                return jsonValue.toInt();
            }
        case QJsonValue::String:
            return jsonValue.toString();
        default:
            return QVariant();
    }
}
