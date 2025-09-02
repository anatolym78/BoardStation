#include "OutParameter.h"
#include <QJsonObject>
#include <QJsonValue>

OutParameter::OutParameter(const QString &label, const QString &controlType)
    : QObject()
    , m_label(label)
    , m_controlType(controlType)
{
}

QJsonObject OutParameter::toJsonObject() const
{
    QJsonObject obj;
    obj["label"] = m_label;
    obj["value"] = QJsonValue::fromVariant(getValueAsString());
    return obj;
}
