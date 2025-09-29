#ifndef BOOLUPLINKPARAMETER_H
#define BOOLUPLINKPARAMETER_H

#include "TypedUplinkParameter.h"
#include <QMap>
#include <QString>

class BoolUplinkParameter : public TypedUplinkParameter<bool>
{
public:
    explicit BoolUplinkParameter(const QString &label, 
                                const QString &controlType = "QCheckBox",
                                const QString &falseAlias = "Off",
                                const QString &trueAlias = "On")
        : TypedUplinkParameter<bool>(label, controlType)
    {
        m_aliases[false] = falseAlias;
        m_aliases[true] = trueAlias;
    }

    // Методы для работы с алиасами
    QString getFalseAlias() const { return m_aliases.value(false); }
    QString getTrueAlias() const { return m_aliases.value(true); }
    QString getValueAlias() const { return m_aliases.value(getTypedValue()); }
    void setFalseAlias(const QString &alias) { m_aliases[false] = alias; }
    void setTrueAlias(const QString &alias) { m_aliases[true] = alias; }

    // Переопределенные методы
    bool setValue(const QVariant &value) override;
    bool isValid() const override;
    bool isIntParameter() const override { return false; }
    bool isDoubleParameter() const override { return false; }
    bool isStringParameter() const override { return false; }
    bool isBoolParameter() const override { return true; }

private:
    QMap<bool, QString> m_aliases;
};

#endif // BOOLUPLINKPARAMETER_H
