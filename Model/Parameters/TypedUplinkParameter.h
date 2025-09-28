#ifndef TYPEDUPLINKPARAMETER_H
#define TYPEDUPLINKPARAMETER_H

#include "BasicUplinkParameter.h"
#include "Range.h"
#include "Step.h"
#include <QList>
#include <type_traits>

template<typename T>
class TypedUplinkParameter : public BasicUplinkParameter
{
public:
    explicit TypedUplinkParameter(const QString &label, const QString &controlType = "QLineEdit")
        : BasicUplinkParameter(label, controlType)
        , m_range()
        , m_step()
    {
    }

    // Типизированные методы установки значений
    void setValue(T value)
    {
        m_value = QVariant::fromValue(value);
    }

    void setValues(const QList<T> &values)
    {
        m_values.clear();
        for (const T &value : values)
        {
            m_values.append(QVariant::fromValue(value));
        }
    }

    // Методы для работы с диапазоном и шагом (только для числовых типов)
    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value>::type
    setRange(U minimum, U maximum)
    {
        m_range.setRange(minimum, maximum);
    }

    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value>::type
    clearRange()
    {
        m_range.clearRange();
    }

    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value>::type
    setStep(U step)
    {
        m_step.setStep(step);
    }

    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value>::type
    clearStep()
    {
        m_step.clearStep();
    }

    // Переопределенные методы базового класса
    QVariant getMinimum() const override
    {
        if constexpr (std::is_arithmetic_v<T>)
        {
            if (m_range.hasRange())
            {
                return QVariant::fromValue(m_range.getMinimum());
            }
        }
        return QVariant();
    }

    QVariant getMaximum() const override
    {
        if constexpr (std::is_arithmetic_v<T>)
        {
            if (m_range.hasRange())
            {
                return QVariant::fromValue(m_range.getMaximum());
            }
        }
        return QVariant();
    }

    QVariant getStep() const override
    {
        if constexpr (std::is_arithmetic_v<T>)
        {
            if (m_step.hasStep())
            {
                return QVariant::fromValue(m_step.getStep());
            }
        }
        return QVariant();
    }

    bool hasRange() const override
    {
        if constexpr (std::is_arithmetic_v<T>)
        {
            return m_range.hasRange();
        }
        return false;
    }

    bool hasStep() const override
    {
        if constexpr (std::is_arithmetic_v<T>)
        {
            return m_step.hasStep();
        }
        return false;
    }

    // Абстрактные методы для реализации в конкретных типах
    virtual bool isValid() const = 0;

protected:
    Range<T> m_range;
    Step<T> m_step;
};

#endif // TYPEDUPLINKPARAMETER_H
