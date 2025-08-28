#ifndef JSONWRITER_H
#define JSONWRITER_H

#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include "Parameter.h"

class JsonWriter : public QObject
{
    Q_OBJECT

public:
    explicit JsonWriter(QObject *parent = nullptr);
    
    // Создание JSON строки из списка параметров
    QString createJsonString(const QList<Parameter> &parameters);
    
    // Создание JSON документа из списка параметров
    QJsonDocument createJsonDocument(const QList<Parameter> &parameters);
    
    // Создание JSON массива из списка параметров
    QJsonArray createJsonArray(const QList<Parameter> &parameters);

signals:
    void writingError(const QString &error);
    void writingSuccess(const QString &jsonString);

private:
    // Преобразование параметра в JSON объект
    QJsonObject parameterToJsonObject(const Parameter &parameter);
};

#endif // JSONWRITER_H
