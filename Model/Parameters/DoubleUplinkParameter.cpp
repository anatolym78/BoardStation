#include "DoubleUplinkParameter.h"
#include <cmath>

bool DoubleUplinkParameter::setValue(const QVariant &value)
{
    if (value.canConvert<double>())
    {
        TypedUplinkParameter<double>::setValue(value.toDouble());
        return true;
    }
    return false;
}

bool DoubleUplinkParameter::isValid() const
{
    // Проверяем, что значение является вещественным числом
    if (!m_value.canConvert<double>())
    {
        return false;
    }

    // Если есть диапазон, проверяем что значение входит в диапазон
    if (hasRange())
    {
        double value = m_value.toDouble();
        double min = m_range.getMinimum();
        double max = m_range.getMaximum();
        
        if (value < min || value > max)
        {
            return false;
        }
    }

    // Если это список значений, проверяем что все значения валидны
    if (isListParameter())
    {
        for (const QVariant &variant : m_values)
        {
            if (!variant.canConvert<double>())
            {
                return false;
            }
        }
    }

    return true;
}
