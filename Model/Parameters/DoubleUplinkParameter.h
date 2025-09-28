#ifndef DOUBLEUPLINKPARAMETER_H
#define DOUBLEUPLINKPARAMETER_H

#include "TypedUplinkParameter.h"

class DoubleUplinkParameter : public TypedUplinkParameter<double>
{
public:
    explicit DoubleUplinkParameter(const QString &label, const QString &controlType = "QDoubleSpinBox")
        : TypedUplinkParameter<double>(label, controlType)
    {
    }

    // Переопределенные методы
    bool isValid() const override;
    bool isIntParameter() const override { return false; }
    bool isDoubleParameter() const override { return true; }
    bool isStringParameter() const override { return false; }
    bool isBoolParameter() const override { return false; }
};

#endif // DOUBLEUPLINKPARAMETER_H
