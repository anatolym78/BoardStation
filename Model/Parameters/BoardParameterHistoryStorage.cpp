#include "BoardParameterHistoryStorage.h"
#include <QDebug>

BoardParameterHistoryStorage::BoardParameterHistoryStorage(QObject *parent)
    : QObject(parent)
{
}

void BoardParameterHistoryStorage::addParameters(const QList<BoardParameterSingle*> &parameters)
{
    for (BoardParameterSingle *param : parameters)
    {
        if (param) 
        {
            addParameter(param);
        }
    }
}

void BoardParameterHistoryStorage::addParameter(BoardParameterSingle *parameter)
{
    if (!parameter || parameter->label().isEmpty()) 
    {
        //qWarning() << "BoardParameterHistoryStorage: Попытка добавить параметр с пустой меткой или nullptr";
        return;
    }

    // Ищем существующую историю для данной метки
    BoardParameterHistory* existingHistory = findHistoryByLabel(parameter->label());
    
    if (!existingHistory) 
    {
        // История ещё не существует, создаём новую
        existingHistory = new BoardParameterHistory(parameter->label(), parameter->unit(), this);
        m_parameterHistories.append(existingHistory);
        emit parameterAdded(parameter->label());
    }

    // Добавляем параметр в историю
    if (existingHistory->addParameter(parameter)) 
    {
        emit parameterUpdated(parameter->label());
    }

    emit newParameterAdded(parameter);
}

BoardParameterHistory* BoardParameterHistoryStorage::getParameterHistory(const QString &label) const
{
    return findHistoryByLabel(label);
}

QStringList BoardParameterHistoryStorage::getParameterLabels() const
{
    QStringList labels;
    for (const BoardParameterHistory *history : m_parameterHistories) 
    {
        if (history) 
        {
            labels.append(history->label());
        }
    }
    return labels;
}

int BoardParameterHistoryStorage::parameterHistoryCount() const
{
    return m_parameterHistories.size();
}

bool BoardParameterHistoryStorage::hasParameterHistory(const QString &label) const
{
    return findHistoryByLabel(label) != nullptr;
}

void BoardParameterHistoryStorage::clear()
{
    if (!m_parameterHistories.isEmpty()) 
    {
        // Удаляем все объекты перед очисткой
        qDeleteAll(m_parameterHistories);
        m_parameterHistories.clear();
        emit parametersCleared();
        //qDebug() << "BoardParameterHistoryStorage: Все истории параметров очищены";
    }
}

QList<BoardParameterHistory*> BoardParameterHistoryStorage::getAllParameterHistories() const
{
    return m_parameterHistories;
}

BoardParameterHistory* BoardParameterHistoryStorage::findHistoryByLabel(const QString &label) const
{
    for (BoardParameterHistory *history : m_parameterHistories) 
    {
        if (history && history->label() == label) 
        {
            return history;
        }
    }
    return nullptr;
}
