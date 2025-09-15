#ifndef BOARDPARAMETERSJSONPARSERNEW_H
#define BOARDPARAMETERSJSONPARSERNEW_H

#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QList>
#include "BoardParameterSingle.h"

class BoardParametersJsonParserNew : public QObject
{
    Q_OBJECT

public:
    explicit BoardParametersJsonParserNew(QObject *parent = nullptr);
    
    // Чтение JSON из строки - возвращает отдельные параметры
    QList<BoardParameterSingle*> parseParametersFromString(const QString &jsonString);
    
    // Чтение JSON из файла
    QList<BoardParameterSingle*> parseParametersFromFile(const QString &filePath);
    
    // Чтение JSON из QJsonArray
    QList<BoardParameterSingle*> parseParametersFromJsonArray(const QJsonArray &jsonArray);
    
    // Проверка валидности JSON
    bool isValidJson(const QString &jsonString);
    
    // Получение последней ошибки парсинга
    QString getLastError() const;

signals:
    void parsingError(const QString &error);
    void parsingSuccess(const QList<BoardParameterSingle*> &parameters);

private:
    QJsonParseError m_lastParseError;
    QString m_lastError;
    
    // Валидация структуры параметра
    bool isValidParameterStructure(const QJsonObject &obj);
    bool isValidTimestamp(const QString &timestamp);
};

#endif // BOARDPARAMETERSJSONPARSERNEW_H
