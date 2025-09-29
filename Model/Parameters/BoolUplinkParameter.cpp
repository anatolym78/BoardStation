#include "BoolUplinkParameter.h"

bool BoolUplinkParameter::setValue(const QVariant &value)
{
    if (value.canConvert<bool>())
    {
        TypedUplinkParameter<bool>::setValue(value.toBool());
        return true;
    }
    return false;
}

bool BoolUplinkParameter::isValid() const
{
    // Проверяем, что значение является булевым
    if (!m_value.canConvert<bool>())
    {
        return false;
    }

    // Если это список значений, проверяем что все значения валидны
    if (isListParameter())
    {
        for (const QVariant &variant : m_values)
        {
            if (!variant.canConvert<bool>())
            {
                return false;
            }
        }
    }

    return true;
}
