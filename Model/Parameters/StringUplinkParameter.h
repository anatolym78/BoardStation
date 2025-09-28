#ifndef STRINGUPLINKPARAMETER_H
#define STRINGUPLINKPARAMETER_H

#include "TypedUplinkParameter.h"

class StringUplinkParameter : public TypedUplinkParameter<QString>
{
public:
    explicit StringUplinkParameter(const QString &label, const QString &controlType = "QLineEdit")
        : TypedUplinkParameter<QString>(label, controlType)
    {
    }

    // Переопределенные методы
    bool isValid() const override;
    bool isIntParameter() const override { return false; }
    bool isDoubleParameter() const override { return false; }
    bool isStringParameter() const override { return true; }
    bool isBoolParameter() const override { return false; }
};

#endif // STRINGUPLINKPARAMETER_H
