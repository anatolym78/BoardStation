#include "RealOutParameter.h"
#include <QString>

RealOutParameter::RealOutParameter(const QString &label, 
                                   double value,
                                   const QString &controlType)
    : OutParameter(label)
    , m_value(value)
    , m_controlType(controlType)
{
}

QString RealOutParameter::getValueAsString() const
{
    return QString::number(m_value, 'f', 6);
}

void RealOutParameter::setValueFromString(const QString &value)
{
    bool ok;
    double newValue = value.toDouble(&ok);
    if (ok) {
        m_value = newValue;
    }
}

QString RealOutParameter::getControlType() const
{
    return m_controlType;
}
