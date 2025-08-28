#ifndef LISTEDREALOUTPARAMETER_H
#define LISTEDREALOUTPARAMETER_H

#include "RealOutParameter.h"
#include <QList>

class ListedRealOutParameter : public RealOutParameter
{
public:
    explicit ListedRealOutParameter(const QString &label, 
                                   double value = 0.0,
                                   const QList<double> &values = QList<double>(),
                                   const QString &controlType = "QComboBox");
    
    // Геттеры
    QList<double> getValues() const { return m_values; }
    
    // Сеттеры
    void setValues(const QList<double> &values) { m_values = values; }
    
    // Переопределенные методы
    bool isValid() const override;
    void setValue(double value) override;

private:
    QList<double> m_values;
};

#endif // LISTEDREALOUTPARAMETER_H
