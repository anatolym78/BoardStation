#include "ParametersStorage.h"
#include <QDebug>

ParametersStorage::ParametersStorage(QObject *parent)
    : QObject(parent)
{
}

void ParametersStorage::addParameters(const QList<Parameter> &parameters)
{
    for (const Parameter &param : parameters) {
        addParameter(param);
    }
}

void ParametersStorage::addParameter(const Parameter &parameter)
{
    if (parameter.label.isEmpty()) {
        qWarning() << "ParametersStorage: Попытка добавить параметр с пустой меткой";
        return;
    }
    
    if (m_parameters.contains(parameter.label)) {
        // Параметр уже существует - добавляем новое значение
        Parameter &existingParam = m_parameters[parameter.label];
        if (parameter.hasValues()) {
            existingParam.addValue(parameter.lastValueData(), parameter.lastTimestamp());
        }
        emit parameterUpdated(parameter.label);
        qDebug() << "ParametersStorage: Обновлен параметр:" << parameter.label;
    } else {
        // Новый параметр
        m_parameters.insert(parameter.label, parameter);
        emit parameterAdded(parameter.label);
        qDebug() << "ParametersStorage: Добавлен новый параметр:" << parameter.label;
    }
}

Parameter ParametersStorage::lastValue(const QString &label) const
{
    if (m_parameters.contains(label)) {
        return m_parameters[label];
    }
    return Parameter();
}

Parameter ParametersStorage::getParameter(const QString &label) const
{
    if (m_parameters.contains(label)) {
        return m_parameters[label];
    }
    return Parameter();
}

QStringList ParametersStorage::getParameterLabels() const
{
    return m_parameters.keys();
}

int ParametersStorage::parameterCount() const
{
    return m_parameters.size();
}

bool ParametersStorage::hasParameter(const QString &label) const
{
    return m_parameters.contains(label);
}

void ParametersStorage::clear()
{
    if (!m_parameters.isEmpty()) {
        m_parameters.clear();
        emit parametersCleared();
        qDebug() << "ParametersStorage: Все параметры очищены";
    }
}

QList<Parameter> ParametersStorage::getAllParameters() const
{
    return m_parameters.values();
}
