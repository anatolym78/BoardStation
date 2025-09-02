#include "BooleanOutParameter.h"
#include <QString>

BooleanOutParameter::BooleanOutParameter(const QString &label, 
                                         bool value,
                                         const QString &falseAlias,
                                         const QString &trueAlias,
                                         const QString &controlType)
    : OutParameter(label, controlType)
    , m_value(value)
{
    m_aliases[false] = falseAlias;
    m_aliases[true] = trueAlias;
}

bool BooleanOutParameter::isValid() const
{
    // Допустимые контролы для булевого параметра
    return (m_controlType == "QCheckBox" || 
            m_controlType == "QComboBox" || 
            m_controlType == "QPushButton") &&
           m_aliases.contains(false) && 
           m_aliases.contains(true);
}

QString BooleanOutParameter::getValueAsString() const
{
    return m_aliases.value(m_value, m_value ? "true" : "false");
}

void BooleanOutParameter::setValueFromString(const QString &value)
{
    // Пытаемся найти значение по алиасу
    if (value == m_aliases.value(true)) {
        m_value = true;
    } else if (value == m_aliases.value(false)) {
        m_value = false;
    } else {
        // Если алиас не найден, пытаемся распарсить как bool
        QString lowerValue = value.toLower();
        if (lowerValue == "true" || lowerValue == "1" || lowerValue == "да") {
            m_value = true;
        } else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "нет") {
            m_value = false;
        }
    }
}






