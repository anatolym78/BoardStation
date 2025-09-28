#ifndef BASICUPLINKPARAMETER_H
#define BASICUPLINKPARAMETER_H

#include <QString>
#include <QObject>
#include <QVariant>
#include <QList>
#include <QJsonObject>

class BasicUplinkParameter : public QObject
{
    Q_OBJECT

public:
    explicit BasicUplinkParameter(const QString &label, const QString &controlType = "QLineEdit");
    virtual ~BasicUplinkParameter() = default;

    // Геттеры
    QString getLabel() const { return m_label; }
    QString getControlType() const { return m_controlType; }
    QVariant getValue() const { return m_value; }
    QList<QVariant> getValues() const { return m_values; }
    virtual QVariant getMinimum() const;
    virtual QVariant getMaximum() const;
    virtual QVariant getStep() const;

    // Проверки
    bool isListParameter() const { return !m_values.isEmpty(); }
    virtual bool hasRange() const = 0;
    virtual bool hasStep() const = 0;
    virtual bool isValid() const = 0;

    // Проверки типа параметра
    virtual bool isIntParameter() const = 0;
    virtual bool isDoubleParameter() const = 0;
    virtual bool isStringParameter() const = 0;
    virtual bool isBoolParameter() const = 0;

protected:
    QString m_label;
    QString m_controlType;
    QVariant m_value;
    QList<QVariant> m_values;
};

#endif // BASICUPLINKPARAMETER_H
