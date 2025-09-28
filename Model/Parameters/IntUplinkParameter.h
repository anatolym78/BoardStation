#ifndef INTUPLINKPARAMETER_H
#define INTUPLINKPARAMETER_H

#include "TypedUplinkParameter.h"

class IntUplinkParameter : public TypedUplinkParameter<int>
{
public:
    explicit IntUplinkParameter(const QString &label, const QString &controlType = "QSpinBox")
        : TypedUplinkParameter<int>(label, controlType)
    {
    }

    // Переопределенные методы
    bool isValid() const override;
    bool isIntParameter() const override { return true; }
    bool isDoubleParameter() const override { return false; }
    bool isStringParameter() const override { return false; }
    bool isBoolParameter() const override { return false; }
};

#endif // INTUPLINKPARAMETER_H
