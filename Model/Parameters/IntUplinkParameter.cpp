#include "IntUplinkParameter.h"

bool IntUplinkParameter::setValue(const QVariant &value)
{
    if (value.canConvert<int>())
    {
        TypedUplinkParameter<int>::setValue(value.toInt());
        return true;
    }
    return false;
}

bool IntUplinkParameter::isValid() const
{
    // Проверяем, что значение является целым числом
    if (!m_value.canConvert<int>())
    {
        return false;
    }

    // Если есть диапазон, проверяем что значение входит в диапазон
    if (hasRange())
    {
        int value = m_value.toInt();
        int min = m_range.getMinimum();
        int max = m_range.getMaximum();
        
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
            if (!variant.canConvert<int>())
            {
                return false;
            }
        }
    }

    return true;
}
