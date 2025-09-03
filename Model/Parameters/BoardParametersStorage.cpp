#include "BoardParametersStorage.h"
#include <QDebug>

BoardParametersStorage::BoardParametersStorage(QObject *parent)
    : QObject(parent)
{
}

void BoardParametersStorage::addParameters(const QList<BoardParameter*> &parameters)
{
    for (BoardParameter *param : parameters)
    {
        if (param) {
            addParameter(param);
        }
    }
}

void BoardParametersStorage::addParameter(BoardParameter *parameter)
{
    if (!parameter || parameter->label().isEmpty()) 
    {
        qWarning() << "BoardParametersStorage: Попытка добавить параметр с пустой меткой или nullptr";
        return;
    }
    
    if (m_parameters.contains(parameter->label())) 
    {
        // Параметр уже существует - добавляем новое значение
        BoardParameter *existingParam = m_parameters[parameter->label()];
        if (parameter->hasValues()) 
        {
            existingParam->addValue(parameter->lastValueData(), parameter->lastTimestamp());
        }
        emit parameterUpdated(parameter->label());
        qDebug() << "BoardParametersStorage: Обновлен параметр:" << parameter->label();
    } 
    else 
    {
        // Новый параметр
        m_parameters.insert(parameter->label(), parameter);
        emit parameterAdded(parameter->label());
        qDebug() << "BoardParametersStorage: Добавлен новый параметр:" << parameter->label();
    }
}

BoardParameter* BoardParametersStorage::lastValue(const QString &label) const
{
    if (m_parameters.contains(label)) {
        return m_parameters[label];
    }
    return nullptr;
}

BoardParameter* BoardParametersStorage::getParameter(const QString &label) const
{
    if (m_parameters.contains(label)) {
        return m_parameters[label];
    }
    return nullptr;
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
        // Удаляем все объекты перед очисткой
        qDeleteAll(m_parameters);
        m_parameters.clear();
        emit parametersCleared();
        qDebug() << "BoardParametersStorage: Все параметры очищены";
    }
}

QList<BoardParameter*> BoardParametersStorage::getAllParameters() const
{
    return m_parameters.values();
}
