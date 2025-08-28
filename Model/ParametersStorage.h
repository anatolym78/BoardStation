#ifndef PARAMETERSSTORAGE_H
#define PARAMETERSSTORAGE_H

#include <QObject>
#include <QMap>
#include <QString>
#include "Parameter.h"

class ParametersStorage : public QObject
{
    Q_OBJECT

public:
    explicit ParametersStorage(QObject *parent = nullptr);
    
    // Добавление параметров - раскидывает по словарю по метке
    void addParameters(const QList<Parameter> &parameters);
    
    // Добавление одного параметра
    void addParameter(const Parameter &parameter);
    
    // Получение последнего значения параметра по метке
    Parameter lastValue(const QString &label) const;
    
    // Получение параметра по метке
    Parameter getParameter(const QString &label) const;
    
    // Получение всех меток параметров
    QStringList getParameterLabels() const;
    
    // Получение количества параметров
    int parameterCount() const;
    
    // Проверка существования параметра
    bool hasParameter(const QString &label) const;
    
    // Очистка всех параметров
    void clear();
    
    // Получение всех параметров как список
    QList<Parameter> getAllParameters() const;

signals:
    void parameterAdded(const QString &label);
    void parameterUpdated(const QString &label);
    void parametersCleared();

private:
    QMap<QString, Parameter> m_parameters;
};

#endif // PARAMETERSSTORAGE_H
