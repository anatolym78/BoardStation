#ifndef OUTPARAMETER_H
#define OUTPARAMETER_H

#include <QString>
#include <QWidget>
#include <QObject>

class OutParameter : public QObject
{
    Q_OBJECT

public:
    explicit OutParameter(const QString &label, const QString &controlType = "QLineEdit");
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
    virtual QString getControlType() const { return m_controlType; }
    
    // Сеттеры
    void setControlType(const QString &controlType) { m_controlType = controlType; }

signals:
    void valueChanged();

protected:
    QString m_label;
    QWidget *m_widget;
    QString m_controlType;
};

#endif // OUTPARAMETER_H
