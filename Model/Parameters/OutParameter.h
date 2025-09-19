#ifndef OUTPARAMETER_H
#define OUTPARAMETER_H

#include <QString>
#include <QObject>
#include <QVariant>

class OutParameter : public QObject
{
    Q_OBJECT

public:
    explicit OutParameter(const QString &label, const QString &controlType = "QLineEdit");
    virtual ~OutParameter() = default;

    QString getLabel() const { return m_label; }

    virtual QVariant getValue() const = 0;
    virtual bool setValue(const QVariant& value) = 0;
    
    virtual bool isValid() const = 0;
    virtual QString getControlType() const { return m_controlType; }
    
    // Создание JSON блока параметра
    QJsonObject toJsonObject() const;
   
protected:
    QString m_label;
    QString m_controlType;
};

#endif // OUTPARAMETER_H
