#include "ListedRealOutParameter.h"
#include <QString>
#include <cmath>
#include <algorithm>

ListedRealOutParameter::ListedRealOutParameter(const QString &label, 
                                               double value,
                                               const QList<double> &values,
                                               const QString &controlType)
    : RealOutParameter(label, value, controlType)
    , m_values(values)
{
}

bool ListedRealOutParameter::isValid() const
{
    // Допустимые контролы для параметра со списком значений
    return (m_controlType == "QSpinBox" || 
            m_controlType == "QComboBox" || 
            m_controlType == "QSlider") &&
           !m_values.isEmpty();
}

void ListedRealOutParameter::setValue(double value)
{
    // Если список значений пуст, устанавливаем любое значение
    if (m_values.isEmpty()) {
        RealOutParameter::setValue(value);
        return;
    }
    
    // Ищем ближайшее значение из списка
    double closestValue = m_values.first();
    double minDistance = std::abs(value - closestValue);
    
    for (double listValue : m_values) {
        double distance = std::abs(value - listValue);
        if (distance < minDistance) {
            minDistance = distance;
            closestValue = listValue;
        }
    }
    
    RealOutParameter::setValue(closestValue);
}






