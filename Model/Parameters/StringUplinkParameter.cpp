#include "StringUplinkParameter.h"

bool StringUplinkParameter::isValid() const
{
    // Проверяем, что значение является строкой
    if (!m_value.canConvert<QString>())
    {
        return false;
    }

    // Если это список значений, проверяем что все значения валидны
    if (isListParameter())
    {
        for (const QVariant &variant : m_values)
        {
            if (!variant.canConvert<QString>())
            {
                return false;
            }
        }
    }

    return true;
}
