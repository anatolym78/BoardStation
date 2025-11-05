#ifndef PARAMETERTREEJSONPARSER_H
#define PARAMETERTREEJSONPARSER_H

#include <QObject>

class QJsonDocument;
class QJsonObject;
class QJsonValue;
class ParameterTreeStorage;
class ParameterTreeItem;

class ParameterTreeJsonParser : public QObject
{
    Q_OBJECT

public:
    explicit ParameterTreeJsonParser(QObject *parent = nullptr);

    ParameterTreeStorage* parseJson(const QString &jsonString);
    void updateJson(const QString &jsonString, ParameterTreeStorage *root);

    QString getLastError() const;

private:
    void processJsonObject(const QJsonObject &jsonObject, ParameterTreeItem *parent);
    void processValue(const QString &key, const QJsonValue &value, ParameterTreeItem *parent);
    QVariant convertJsonValue(const QJsonValue &jsonValue);

    QString m_lastError;
};

#endif // PARAMETERTREEJSONPARSER_H
