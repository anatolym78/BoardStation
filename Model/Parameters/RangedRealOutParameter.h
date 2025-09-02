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
    double getMinValue() const { return m_minimum; }
    double getMaxValue() const { return m_maximum; }
    
    // Сеттеры
    void setMinimum(double minimum) { m_minimum = minimum; }
    void setMaximum(double maximum) { m_maximum = maximum; }
    void setStep(double step) { m_step = step; }
    
    // Переопределенные методы
    bool isValid() const override;
    void setValue(double value) override;
    
    // Метод для получения виджета управления
    QWidget* getControlWidget() const { return nullptr; } // Убрано, теперь создается через OutParameterWidgetCreator
    
    // Метод для установки родителя виджета (убрано, больше не нужно)
    void setWidgetParent(QWidget *parent) { Q_UNUSED(parent); }

private:
    double m_minimum;
    double m_maximum;
    double m_step;
};

#endif // RANGEDREALOUTPARAMETER_H
