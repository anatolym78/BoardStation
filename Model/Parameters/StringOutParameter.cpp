#include "StringOutParameter.h"
#include <QString>

StringOutParameter::StringOutParameter(const QString &label, 
                                     const QString &value,
                                     const QStringList &valuesList,
                                     const QString &controlType)
    : OutParameter(label, controlType)
    , m_value(value)
    , m_valuesList(valuesList)
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


