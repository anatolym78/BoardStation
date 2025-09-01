#include "OutParametersStorage.h"
#include <QDebug>

OutParametersStorage::OutParametersStorage(QObject *parent)
    : QObject(parent)
{
}

void OutParametersStorage::addParameters(const QList<OutParameter*> &parameters)
{
    for (OutParameter *param : parameters) {
        if (param && param->isValid()) {
            addParameter(param);
        }
    }
}

void OutParametersStorage::addParameter(OutParameter *parameter)
{
    if (!parameter || !parameter->isValid()) {
        qWarning() << "OutParametersStorage: Попытка добавить невалидный параметр";
        return;
    }
    
    QString label = parameter->getLabel();
    
    // Если параметр уже существует, удаляем старый
    if (m_parameters.contains(label)) {
        delete m_parameters[label];
        m_parameters.remove(label);
    }
    
    m_parameters[label] = parameter;
    emit parameterAdded(label);
    
    qDebug() << "OutParametersStorage: Добавлен параметр:" << label;
}

OutParameter* OutParametersStorage::getParameter(const QString &label) const
{
    return m_parameters.value(label, nullptr);
}

QStringList OutParametersStorage::getParameterLabels() const
{
    return m_parameters.keys();
}

int OutParametersStorage::parameterCount() const
{
    return m_parameters.size();
}

bool OutParametersStorage::hasParameter(const QString &label) const
{
    return m_parameters.contains(label);
}

void OutParametersStorage::clear()
{
    // Удаляем все параметры
    for (OutParameter *param : m_parameters.values()) {
        delete param;
    }
    m_parameters.clear();
    
    emit parametersCleared();
    qDebug() << "OutParametersStorage: Все параметры очищены";
}

QList<OutParameter*> OutParametersStorage::getAllParameters() const
{
    return m_parameters.values();
}
