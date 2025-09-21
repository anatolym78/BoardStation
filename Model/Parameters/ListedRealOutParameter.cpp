#include "ListedRealOutParameter.h"
#include <QString>
#include <cmath>

ListedRealOutParameter::ListedRealOutParameter(const QString &label, 
                                               double value,
                                               const QList<double> &values,
                                               const QString &controlType)
    : RealOutParameter(label, value, controlType)
    , m_values(values)
{
    m_index = 0;
}

QVariant ListedRealOutParameter::getValue() const
{
    return m_value;

    if (m_index == -1) return { };

    return m_values[m_index];
}

bool ListedRealOutParameter::isValid() const
{
    // Допустимые контролы для параметра со списком значений
    return (m_controlType == "QSpinBox" || 
            m_controlType == "QComboBox" || 
            m_controlType == "QSlider") &&
           !m_values.isEmpty();
}

bool ListedRealOutParameter::setValue(const QVariant& value)
{
    if (!value.canConvert<double>()) return false;

    auto doubleValue = value.toDouble();

    // Если список значений пуст, устанавливаем любое значение
    if (m_values.isEmpty())
    {
        RealOutParameter::setValue(value);
        return false;
    }
    
    // Ищем ближайшее значение из списка
    double closestValue = m_values.first();
    double minDistance = std::abs(doubleValue - closestValue);
    
    for (double listValue : m_values) 
    {
        double distance = std::abs(doubleValue - listValue);
        if (distance < minDistance)
        {
            minDistance = distance;
            closestValue = listValue;
        }
    }
    
    return RealOutParameter::setValue(closestValue);
}

bool ListedRealOutParameter::selectIndex(int index)
{
    if (index >= 0 && index < m_values.count())
    {
        m_index = index;

        m_value = m_values[index];

        return true;
    }

    return false;
}





