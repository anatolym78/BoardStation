#include "BoardParametersJsonParserNew.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

BoardParametersJsonParserNew::BoardParametersJsonParserNew(QObject *parent)
    : QObject(parent)
{
}

QList<BoardParameterSingle*> BoardParametersJsonParserNew::parseParametersFromString(const QString &jsonString)
{
    QList<BoardParameterSingle*> parameters;
    
    if (jsonString.isEmpty()) 
    {
        m_lastError = "JSON строка пуста";
        emit parsingError(m_lastError);
        return parameters;
    }
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);
    
    if (parseError.error != QJsonParseError::NoError) 
    {
        m_lastParseError = parseError;
        m_lastError = QString("Ошибка парсинга JSON: %1").arg(parseError.errorString());
        emit parsingError(m_lastError);
        return parameters;
    }
    
    if (!doc.isArray()) 
    {
        m_lastError = "JSON должен содержать массив";
        emit parsingError(m_lastError);
        return parameters;
    }
    
    parameters = parseParametersFromJsonArray(doc.array());
    
    if (parameters.isEmpty()) 
    {
        m_lastError = "Не удалось извлечь параметры из JSON";
        emit parsingError(m_lastError);
    } 
    else 
    {
        emit parsingSuccess(parameters);
    }
    
    return parameters;
}

QList<BoardParameterSingle*> BoardParametersJsonParserNew::parseParametersFromFile(const QString &filePath)
{
    QList<BoardParameterSingle*> parameters;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        m_lastError = QString("Не удалось открыть файл: %1").arg(filePath);
        emit parsingError(m_lastError);
        return parameters;
    }
    
    QTextStream in(&file);
    QString jsonString = in.readAll();
    file.close();
    
    return parseParametersFromString(jsonString);
}

QList<BoardParameterSingle*> BoardParametersJsonParserNew::parseParametersFromJsonArray(const QJsonArray &jsonArray)
{
    QList<BoardParameterSingle*> parameters;
    
    for (const QJsonValue &value : jsonArray) 
    {
        if (!value.isObject()) 
        {
            qWarning() << "Пропускаем не-объект в JSON массиве";
            continue;
        }
        
        QJsonObject obj = value.toObject();
        if (isValidParameterStructure(obj)) 
        {
            QString label = obj["label"].toString();
            QVariant paramValue = obj["value"].toVariant();
            QString unit = obj["unit"].toString();
            
            // Парсим timestamp, если он есть
            QDateTime timestamp = QDateTime::currentDateTime();
            if (obj.contains("timestamp")) 
            {
                timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
                if (!timestamp.isValid()) 
                {
                    timestamp = QDateTime::currentDateTime();
                }
            }
            
            // Создаём отдельный параметр (не историю)
            BoardParameterSingle *param = new BoardParameterSingle(label, paramValue, timestamp, unit, this);
            parameters.append(param);
        } 
        else 
        {
            qWarning() << "Пропускаем невалидный параметр в JSON";
        }
    }
    
    return parameters;
}

bool BoardParametersJsonParserNew::isValidJson(const QString &jsonString)
{
    QJsonParseError parseError;
    QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);
    return parseError.error == QJsonParseError::NoError;
}

QString BoardParametersJsonParserNew::getLastError() const
{
    return m_lastError;
}

bool BoardParametersJsonParserNew::isValidParameterStructure(const QJsonObject &obj)
{
    // Проверяем наличие обязательных полей
    if (!obj.contains("label") || !obj.contains("value")) 
    {
        return false;
    }
    
    // Проверяем, что label - это строка
    if (!obj["label"].isString()) 
    {
        return false;
    }
    
    // Проверяем, что label не пустой
    if (obj["label"].toString().isEmpty()) 
    {
        return false;
    }
    
    // value может быть любым типом, но должен существовать
    return !obj["value"].isUndefined();
}

bool BoardParametersJsonParserNew::isValidTimestamp(const QString &timestamp)
{
    QDateTime dt = QDateTime::fromString(timestamp, Qt::ISODate);
    return dt.isValid();
}
