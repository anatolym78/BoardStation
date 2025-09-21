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

bool BooleanOutParameter::setValue(const QVariant& value)
{
    if (value.canConvert<bool>())
    {
        m_value = value.toBool();

        return true;
    }

	return false;
}

bool BooleanOutParameter::isValid() const
{
    // Допустимые контролы для булевого параметра
    return (m_controlType == "QCheckBox") &&
           m_aliases.contains(false) && 
           m_aliases.contains(true);
}





