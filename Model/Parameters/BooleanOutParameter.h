#ifndef BOOLEANOUTPARAMETER_H
#define BOOLEANOUTPARAMETER_H

#include "OutParameter.h"
#include <QMap>
#include <QString>

class BooleanOutParameter : public OutParameter
{
public:
    explicit BooleanOutParameter(const QString &label, 
                                bool value = false,
                                const QString &falseAlias = "checked",
                                const QString &trueAlias = "unchecked",
                                const QString &controlType = "QCheckBox");
    
    QVariant getValue() const override { return QVariant::fromValue(m_value); }
	bool setValue(const QVariant& value) override;

    QString getFalseAlias() const { return m_aliases.value(false); }
    QString getTrueAlias() const { return m_aliases.value(true); }
    QVariant getValueAlias() const { return m_aliases.value(m_value); }
    
    void setFalseAlias(const QString &alias) { m_aliases[false] = alias; }
    void setTrueAlias(const QString &alias) { m_aliases[true] = alias; }
    
    // Переопределенные методы
    bool isValid() const override;

private:
    bool m_value;
    QMap<bool, QString> m_aliases;
};

#endif // BOOLEANOUTPARAMETER_H
