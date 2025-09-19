#include "RealOutParameter.h"
#include <QString>

RealOutParameter::RealOutParameter(const QString &label, 
                                   double value,
                                   const QString &controlType)
    : OutParameter(label, controlType)
    , m_value(value)
{
}

QVariant RealOutParameter::getValue() const
{
	return QVariant::fromValue(m_value);
}

bool RealOutParameter::setValue(const QVariant& value)
{
    bool ok = false;

    m_value = value.toDouble(&ok);

    return ok;
}

bool RealOutParameter::isValid() const
{
    return m_controlType == "QSlider";
}

