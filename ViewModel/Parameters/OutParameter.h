#ifndef OUTPARAMETER_H
#define OUTPARAMETER_H

#include <QString>
#include <QWidget>

class OutParameter
{
public:
    explicit OutParameter(const QString &label);
    virtual ~OutParameter() = default;

    // Геттеры
    QString getLabel() const { return m_label; }
    QWidget* getWidget() const { return m_widget; }
    
    // Сеттеры
    void setWidget(QWidget *widget) { m_widget = widget; }
    
    // Виртуальные методы
    virtual bool isValid() const = 0;
    virtual QString getValueAsString() const = 0;
    virtual void setValueFromString(const QString &value) = 0;
    virtual QString getControlType() const = 0;

protected:
    QString m_label;
    QWidget *m_widget;
};

#endif // OUTPARAMETER_H
