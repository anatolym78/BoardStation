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





