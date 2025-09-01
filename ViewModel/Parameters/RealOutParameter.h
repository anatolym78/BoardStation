#ifndef REALOUTPARAMETER_H
#define REALOUTPARAMETER_H

#include "OutParameter.h"

class RealOutParameter : public OutParameter
{
public:
    explicit RealOutParameter(const QString &label, 
                             double value = 0.0,
                             const QString &controlType = "QSpinBox");
    
    // Геттеры
    double getValue() const { return m_value; }
    
    // Сеттеры
    virtual void setValue(double value) { m_value = value; }
    
    // Переопределенные методы
    QString getValueAsString() const override;
    void setValueFromString(const QString &value) override;

protected:
    double m_value;
};

#endif // REALOUTPARAMETER_H
