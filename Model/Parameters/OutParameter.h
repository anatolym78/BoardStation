#ifndef OUTPARAMETER_H
#define OUTPARAMETER_H

#include <QString>
#include <QObject>

class OutParameter : public QObject
{
    Q_OBJECT

public:
    explicit OutParameter(const QString &label, const QString &controlType = "QLineEdit");
    virtual ~OutParameter() = default;

    // Геттеры
    QString getLabel() const { return m_label; }
    
    // Виртуальные методы
    virtual bool isValid() const = 0;
    virtual QString getValueAsString() const = 0;
    virtual void setValueFromString(const QString &value) = 0;
    virtual QString getControlType() const { return m_controlType; }
    
    // Создание JSON блока параметра
    QJsonObject toJsonObject() const;
    
    // Сеттеры
    void setControlType(const QString &controlType) { m_controlType = controlType; }

signals:
    void valueChanged();

protected:
    QString m_label;
    QString m_controlType;
};

#endif // OUTPARAMETER_H
