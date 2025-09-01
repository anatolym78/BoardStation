#ifndef STRINGOUTPARAMETER_H
#define STRINGOUTPARAMETER_H

#include "OutParameter.h"
#include <QStringList>

class StringOutParameter : public OutParameter
{
public:
    explicit StringOutParameter(const QString &label, 
                              const QString &value = QString(),
                              const QStringList &valuesList = QStringList(),
                              const QString &controlType = "QTextEdit");
    
    // Геттеры
    QString getValue() const { return m_value; }
    QStringList getValuesList() const { return m_valuesList; }
    
    // Сеттеры
    void setValue(const QString &value) { m_value = value; }
    void setValuesList(const QStringList &valuesList) { m_valuesList = valuesList; }
    
    // Переопределенные методы
    bool isValid() const override;
    QString getValueAsString() const override;
    void setValueFromString(const QString &value) override;
    
    // Метод для получения виджета управления
    QWidget* getControlWidget() const { return nullptr; } // Убрано, теперь создается через OutParameterWidgetCreator
    
    // Метод для установки родителя виджета (убрано, больше не нужно)
    void setWidgetParent(QWidget *parent) { Q_UNUSED(parent); }
    
    // Геттер для значений (используется в OutParameterWidgetCreator)
    QStringList getValues() const { return m_valuesList; }

private:
    QString m_value;
    QStringList m_valuesList;
};

#endif // STRINGOUTPARAMETER_H
