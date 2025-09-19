#ifndef REALOUTPARAMETER_H
#define REALOUTPARAMETER_H

#include "OutParameter.h"

class RealOutParameter : public OutParameter
{
public:
    explicit RealOutParameter(const QString &label, 
                             double value = 0.0,
                             const QString &controlType = "QSpinBox");
    
    QVariant getValue() const override;
    bool setValue(const QVariant& value) override;


    bool isValid() const override;

protected:
	double m_value;
};

#endif // REALOUTPARAMETER_H
