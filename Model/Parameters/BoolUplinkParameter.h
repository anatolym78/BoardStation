#ifndef BOOLUPLINKPARAMETER_H
#define BOOLUPLINKPARAMETER_H

#include "TypedUplinkParameter.h"

class BoolUplinkParameter : public TypedUplinkParameter<bool>
{
public:
    explicit BoolUplinkParameter(const QString &label, const QString &controlType = "QCheckBox")
        : TypedUplinkParameter<bool>(label, controlType)
    {
    }

    // Переопределенные методы
    bool isValid() const override;
    bool isIntParameter() const override { return false; }
    bool isDoubleParameter() const override { return false; }
    bool isStringParameter() const override { return false; }
    bool isBoolParameter() const override { return true; }
};

#endif // BOOLUPLINKPARAMETER_H
