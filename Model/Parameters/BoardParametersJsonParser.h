#ifndef BOARDPARAMETERSJSONPARSER_H
#define BOARDPARAMETERSJSONPARSER_H

#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QList>
#include "BoardParameter.h"

class BoardParametersJsonParser : public QObject
{
    Q_OBJECT

public:
    explicit BoardParametersJsonParser(QObject *parent = nullptr);
    
    // Чтение JSON из строки
    QList<BoardParameter*> parseParametersFromString(const QString &jsonString);
    
    // Чтение JSON из файла
    QList<BoardParameter*> parseParametersFromFile(const QString &filePath);
    
    // Чтение JSON из QJsonArray
    QList<BoardParameter*> parseParametersFromJsonArray(const QJsonArray &jsonArray);
    
    // Проверка валидности JSON
    bool isValidJson(const QString &jsonString);
    
    // Получение последней ошибки парсинга
    QString getLastError() const;

signals:
    void parsingError(const QString &error);
    void parsingSuccess(const QList<BoardParameter*> &parameters);

private:
    QJsonParseError m_lastParseError;
    QString m_lastError;
    
    // Валидация структуры параметра
    bool isValidParameterStructure(const QJsonObject &obj);
    bool isValidTimestamp(const QString &timestamp);
};

#endif // BOARDPARAMETERSJSONPARSER_H
