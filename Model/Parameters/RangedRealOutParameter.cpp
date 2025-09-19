#include "RangedRealOutParameter.h"
#include <QString>

RangedRealOutParameter::RangedRealOutParameter(const QString &label, 
                                               double value,
                                               double minimum,
                                               double maximum,
                                               double step,
                                               const QString &controlType)
    : RealOutParameter(label, value, controlType)
    , m_minimum(minimum)
    , m_maximum(maximum)
    , m_step(step)
{

}

bool RangedRealOutParameter::isValid() const
{
    // Допустимые контролы для параметра с диапазоном
    return (m_controlType == "QSlider" || m_controlType == "QSpinBox") &&
           m_minimum < m_maximum &&
           m_step > 0.0;
}

bool RangedRealOutParameter::setValue(const QVariant& value)
{
    auto realValue = value.toDouble();

    if (realValue < m_minimum || realValue > m_maximum) return false;
    
    // Округляем до ближайшего шага
    double steps = (realValue - m_minimum) / m_step;
    realValue = m_minimum + (qRound(steps) * m_step);
    

    return RealOutParameter::setValue(value);
}


