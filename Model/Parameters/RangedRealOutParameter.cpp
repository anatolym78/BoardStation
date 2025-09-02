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

void RangedRealOutParameter::setValue(double value)
{
    // Ограничиваем значение диапазоном
    if (value < m_minimum) {
        value = m_minimum;
    } else if (value > m_maximum) {
        value = m_maximum;
    }
    
    // Округляем до ближайшего шага
    double steps = (value - m_minimum) / m_step;
    value = m_minimum + (qRound(steps) * m_step);
    
    RealOutParameter::setValue(value);
}


