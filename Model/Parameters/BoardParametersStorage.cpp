#include "BoardParametersStorage.h"
#include <QDebug>

BoardParametersStorage::BoardParametersStorage(QObject *parent)
    : QObject(parent)
{
}

void BoardParametersStorage::addParameters(const QList<BoardParameterSingle*> &parameters)
{
    for (BoardParameterSingle *param : parameters)
    {
        if (param) 
        {
            addParameter(param);
        }
    }
}

void BoardParametersStorage::addParameter(BoardParameterSingle *parameter)
{
    if (!parameter || parameter->label().isEmpty()) 
    {
        return;
    }

    if (!m_parameters.contains(parameter->label())) 
    {
		m_parameters.insert(parameter->label(), parameter);
		emit parameterAdded(parameter->label());
    }
    else
    {
        // Replace the old parameter with the new one.
        // The old parameter object will be deleted if this storage object owns it.
        delete m_parameters[parameter->label()];
        m_parameters[parameter->label()] = parameter;
    }
	
	emit parameterUpdated(parameter->label());
}

BoardParameterSingle* BoardParametersStorage::lastValue(const QString &label) const
{
    return m_parameters.value(label, nullptr);
}

BoardParameterSingle* BoardParametersStorage::getParameter(const QString &label) const
{
    return m_parameters.value(label, nullptr);
}

QStringList BoardParametersStorage::getParameterLabels() const
{
    return m_parameters.keys();
}

int BoardParametersStorage::parameterCount() const
{
    return m_parameters.size();
}

bool BoardParametersStorage::hasParameter(const QString &label) const
{
    return m_parameters.contains(label);
}

void BoardParametersStorage::clear()
{
    if (!m_parameters.isEmpty()) {
        // qDeleteAll takes care of deleting all the pointed-to objects
        qDeleteAll(m_parameters);
        m_parameters.clear();
        emit parametersCleared();
        qDebug() << "BoardParametersStorage: Все параметры очищены";
    }
}

QList<BoardParameterSingle*> BoardParametersStorage::getAllParameters() const
{
    return m_parameters.values();
}
