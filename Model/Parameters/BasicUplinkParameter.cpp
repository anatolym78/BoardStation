#include "BasicUplinkParameter.h"

BasicUplinkParameter::BasicUplinkParameter(const QString &label, const QString &controlType)
    : QObject()
    , m_label(label)
    , m_controlType(controlType)
{
}

QVariant BasicUplinkParameter::getMinimum() const
{
    // Реализация по умолчанию - возвращает пустой QVariant
    // Конкретные реализации должны переопределить этот метод
    return QVariant();
}

QVariant BasicUplinkParameter::getMaximum() const
{
    // Реализация по умолчанию - возвращает пустой QVariant
    // Конкретные реализации должны переопределить этот метод
    return QVariant();
}

QVariant BasicUplinkParameter::getStep() const
{
    // Реализация по умолчанию - возвращает пустой QVariant
    // Конкретные реализации должны переопределить этот метод
    return QVariant();
}
