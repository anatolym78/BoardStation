#ifndef RANGEDREALOUTPARAMETER_H
#define RANGEDREALOUTPARAMETER_H

#include "RealOutParameter.h"

class RangedRealOutParameter : public RealOutParameter
{
public:
    explicit RangedRealOutParameter(const QString &label, 
                                   double value = 0.0,
                                   double minimum = 0.0,
                                   double maximum = 100.0,
                                   double step = 1.0,
                                   const QString &controlType = "QSlider");
    
    // Геттеры
    double getMinimum() const { return m_minimum; }
    double getMaximum() const { return m_maximum; }
    double getStep() const { return m_step; }
    
    // Сеттеры
    void setMinimum(double minimum) { m_minimum = minimum; }
    void setMaximum(double maximum) { m_maximum = maximum; }
    void setStep(double step) { m_step = step; }
    
    // Переопределенные методы
    bool isValid() const override;
    void setValue(double value) override;

private:
    double m_minimum;
    double m_maximum;
    double m_step;
};

#endif // RANGEDREALOUTPARAMETER_H
