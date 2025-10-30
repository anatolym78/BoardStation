#include "BoardParametersJsonParser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "BoardParameterSingle.h"

BoardParametersJsonParser::BoardParametersJsonParser(QObject *parent)
    : QObject(parent)
{
}

QList<BoardParameterSingle*> BoardParametersJsonParser::parseParametersFromString(const QString &jsonString)
{
    QList<BoardParameterSingle*> parameters;
    
    if (jsonString.isEmpty()) {
        m_lastError = "JSON строка пуста";
        emit parsingError(m_lastError);
        return parameters;
    }
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        m_lastParseError = parseError;
        m_lastError = QString("Ошибка парсинга JSON: %1").arg(parseError.errorString());
        emit parsingError(m_lastError);
        return parameters;
    }
    
    if (!doc.isArray()) {
        m_lastError = "JSON должен содержать массив";
        emit parsingError(m_lastError);
        return parameters;
    }
    
    parameters = parseParametersFromJsonArray(doc.array());
    
    if (m_lastError.isEmpty())
    {
        emit parsingSuccess(parameters);
    }
    
    return parameters;
}

QList<BoardParameterSingle*> BoardParametersJsonParser::parseParametersFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = QString("Не удалось открыть файл: %1").arg(filePath);
        emit parsingError(m_lastError);
        return {};
    }
    
    QTextStream in(&file);
    QString jsonString = in.readAll();
    file.close();
    
    return parseParametersFromString(jsonString);
}

QList<BoardParameterSingle*> BoardParametersJsonParser::parseParametersFromJsonArray(const QJsonArray &jsonArray)
{
    QList<BoardParameterSingle*> parameters;
    m_lastError.clear();
    
    for (const QJsonValue &value : jsonArray) {
        if (!value.isObject()) {
            qWarning() << "Пропускаем не-объект в JSON массиве";
            continue;
        }
        
        QJsonObject obj = value.toObject();
        if (isValidParameterStructure(obj)) {
            QString label = obj["label"].toString();
            QVariant valueData = obj["value"].toVariant();
            QString unit = obj.contains("unit") ? obj["unit"].toString() : QString();
            
            QDateTime timestamp = QDateTime::currentDateTime();
            if (obj.contains("timestamp")) {
                timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
                if (!timestamp.isValid()) {
                    timestamp = QDateTime::currentDateTime();
                }
            }
            
            parameters.append(new BoardParameterSingle(label, valueData, timestamp, unit));
        } else {
            qWarning() << "Пропускаем невалидный параметр в JSON:" << obj;
            m_lastError = "Один или более параметров в JSON имеют неверную структуру";
            // Не выходим, просто продолжаем парсить остальные
        }
    }
    
    if (!m_lastError.isEmpty())
    {
        emit parsingError(m_lastError);
    }

    return parameters;
}

bool BoardParametersJsonParser::isValidJson(const QString &jsonString)
{
    QJsonParseError parseError;
    QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);
    return parseError.error == QJsonParseError::NoError;
}

QString BoardParametersJsonParser::getLastError() const
{
    return m_lastError;
}

bool BoardParametersJsonParser::isValidParameterStructure(const QJsonObject &obj)
{
    // Проверяем наличие обязательных полей
    if (!obj.contains("label") || !obj.contains("value")) {
        return false;
    }
    
    // Проверяем, что label - это строка
    if (!obj["label"].isString()) {
        return false;
    }
    
    // Проверяем, что label не пустой
    if (obj["label"].toString().isEmpty()) {
        return false;
    }
    
    // value может быть любым типом, но должен существовать
    return !obj["value"].isUndefined();
}
