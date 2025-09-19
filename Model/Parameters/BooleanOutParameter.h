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
                                const QString &falseAlias = "Выключено",
                                const QString &trueAlias = "Включено",
                                const QString &controlType = "QCheckBox");
    
    // Геттеры
    QVariant getValue() const override { return QVariant::fromValue(m_value); }
    QString getFalseAlias() const { return m_aliases.value(false); }
    QString getTrueAlias() const { return m_aliases.value(true); }
    QString getValueAlias() const { return m_aliases.value(m_value); }
    
    // Сеттеры
    void setValue(bool value) { m_value = value; }
    void setFalseAlias(const QString &alias) { m_aliases[false] = alias; }
    void setTrueAlias(const QString &alias) { m_aliases[true] = alias; }
    
    // Переопределенные методы
    bool isValid() const override;

private:
    bool m_value;
    QMap<bool, QString> m_aliases;
};

#endif // BOOLEANOUTPARAMETER_H
