#ifndef JSONREADER_H
#define JSONREADER_H

#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QList>
#include "Parameter.h"

class JsonReader : public QObject
{
    Q_OBJECT

public:
    explicit JsonReader(QObject *parent = nullptr);
    
    // Чтение JSON из строки
    QList<Parameter> parseParametersFromString(const QString &jsonString);
    
    // Чтение JSON из файла
    QList<Parameter> parseParametersFromFile(const QString &filePath);
    
    // Чтение JSON из QJsonArray
    QList<Parameter> parseParametersFromJsonArray(const QJsonArray &jsonArray);
    
    // Проверка валидности JSON
    bool isValidJson(const QString &jsonString);
    
    // Получение последней ошибки парсинга
    QString getLastError() const;

signals:
    void parsingError(const QString &error);
    void parsingSuccess(const QList<Parameter> &parameters);

private:
    QJsonParseError m_lastParseError;
    QString m_lastError;
    
    // Валидация структуры параметра
    bool isValidParameterStructure(const QJsonObject &obj);
    bool isValidTimestamp(const QString &timestamp);
};

#endif // JSONREADER_H
