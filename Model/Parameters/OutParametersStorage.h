#ifndef OUTPARAMETERSSTORAGE_H
#define OUTPARAMETERSSTORAGE_H

#include <QObject>
#include <QMap>
#include <QString>
#include "OutParameter.h"

class OutParametersStorage : public QObject
{
    Q_OBJECT

public:
    explicit OutParametersStorage(QObject *parent = nullptr);
    
    // Добавление параметров - раскидывает по словарю по метке
    void addParameters(const QList<OutParameter*> &parameters);
    
    // Добавление одного параметра
    void addParameter(OutParameter *parameter);
    
    // Получение параметра по метке
    OutParameter* getParameter(const QString &label) const;
    
    // Получение всех меток параметров
    QStringList getParameterLabels() const;
    
    // Получение количества параметров
    int parameterCount() const;
    
    // Проверка существования параметра
    bool hasParameter(const QString &label) const;
    
    // Очистка всех параметров
    void clear();
    
    // Получение всех параметров как список
    QList<OutParameter*> getAllParameters() const;
    
    // Получение параметров определенного типа
    template<typename T>
    QList<T*> getParametersOfType() const;

signals:
    void parameterAdded(const QString &label);
    void parameterUpdated(const QString &label);
    void parametersCleared();

private:
    QMap<QString, OutParameter*> m_parameters;
};

// Шаблонная реализация для получения параметров определенного типа
template<typename T>
QList<T*> OutParametersStorage::getParametersOfType() const
{
    QList<T*> result;
    for (OutParameter *param : m_parameters.values()) {
        if (T *typedParam = dynamic_cast<T*>(param)) {
            result.append(typedParam);
        }
    }
    return result;
}

#endif // OUTPARAMETERSSTORAGE_H
