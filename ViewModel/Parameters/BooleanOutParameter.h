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
    bool getValue() const { return m_value; }
    QString getFalseAlias() const { return m_aliases.value(false); }
    QString getTrueAlias() const { return m_aliases.value(true); }
    QString getValueAlias() const { return m_aliases.value(m_value); }
    
    // Сеттеры
    void setValue(bool value) { m_value = value; }
    void setFalseAlias(const QString &alias) { m_aliases[false] = alias; }
    void setTrueAlias(const QString &alias) { m_aliases[true] = alias; }
    
    // Переопределенные методы
    bool isValid() const override;
    QString getValueAsString() const override;
    void setValueFromString(const QString &value) override;
    
    // Метод для получения виджета управления
    QWidget* getControlWidget() const { return nullptr; } // Убрано, теперь создается через OutParameterWidgetCreator
    
    // Метод для установки родителя виджета (убрано, больше не нужно)
    void setWidgetParent(QWidget *parent) { Q_UNUSED(parent); }

private:
    bool m_value;
    QMap<bool, QString> m_aliases;
};

#endif // BOOLEANOUTPARAMETER_H
