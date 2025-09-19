#ifndef STRINGOUTPARAMETER_H
#define STRINGOUTPARAMETER_H

#include "OutParameter.h"
#include <QStringList>

class StringOutParameter : public OutParameter
{
public:
    explicit StringOutParameter(const QString &label, 
                              const int &valueIndex = 0,
                              const QStringList &valuesList = QStringList(),
                              const QString &controlType = "QTextEdit");
    
    // Геттеры
    QVariant getValue() const override;
    QStringList getValuesList() const { return m_valuesList; }
    
    // Сеттеры
    void setValueIndex(int valueIndex);
    void setValuesList(const QStringList &valuesList);
    
    // Переопределенные методы
    bool isValid() const override;
    
    // Геттер для значений (используется в OutParameterWidgetCreator)
    QStringList getValues() const { return m_valuesList; }

private:
    int m_valueIndex = 0;
    QStringList m_valuesList;
};

#endif // STRINGOUTPARAMETER_H
