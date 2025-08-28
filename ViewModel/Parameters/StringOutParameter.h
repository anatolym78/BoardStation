#ifndef STRINGOUTPARAMETER_H
#define STRINGOUTPARAMETER_H

#include "OutParameter.h"
#include <QStringList>

class StringOutParameter : public OutParameter
{
public:
    explicit StringOutParameter(const QString &label, 
                              const QString &value = QString(),
                              const QStringList &valuesList = QStringList(),
                              const QString &controlType = "QTextEdit");
    
    // Геттеры
    QString getValue() const { return m_value; }
    QStringList getValuesList() const { return m_valuesList; }
    
    // Сеттеры
    void setValue(const QString &value) { m_value = value; }
    void setValuesList(const QStringList &valuesList) { m_valuesList = valuesList; }
    
    // Переопределенные методы
    bool isValid() const override;
    QString getValueAsString() const override;
    void setValueFromString(const QString &value) override;
    QString getControlType() const override;

private:
    QString m_value;
    QStringList m_valuesList;
    QString m_controlType;
};

#endif // STRINGOUTPARAMETER_H
