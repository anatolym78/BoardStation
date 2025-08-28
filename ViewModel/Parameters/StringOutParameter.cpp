#include "StringOutParameter.h"

StringOutParameter::StringOutParameter(const QString &label, 
                                     const QString &value,
                                     const QStringList &valuesList,
                                     const QString &controlType)
    : OutParameter(label)
    , m_value(value)
    , m_valuesList(valuesList)
    , m_controlType(controlType)
{
}

bool StringOutParameter::isValid() const
{
    // Для строкового параметра допустим только QTextEdit
    return m_controlType == "QTextEdit";
}

QString StringOutParameter::getValueAsString() const
{
    return m_value;
}

void StringOutParameter::setValueFromString(const QString &value)
{
    m_value = value;
}

QString StringOutParameter::getControlType() const
{
    return m_controlType;
}
