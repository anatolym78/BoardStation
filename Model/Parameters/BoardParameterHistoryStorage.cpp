#include "BoardParameterHistoryStorage.h"
#include "BoardMessagesSqliteReader.h"
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
        return;
    }

    // Просто добавляем параметр в список сессии
    m_sessionParameters.append(parameter);
    
    emit newParameterAdded(parameter);
    
    // Эмитируем сигнал для плеера
    emit parameterEmitted(parameter);
}

void BoardParameterHistoryStorage::loadSessionData(int sessionId, BoardMessagesSqliteReader* reader)
{
    return;

    if (!reader)
    {
        qWarning() << "BoardParameterHistoryStorage: Reader is not set";
        return;
    }
    
    // Очищаем текущие данные
    clear();
    
    // Загружаем данные сессии из базы
    m_sessionParameters = reader->getSessionParameters(sessionId, "");
    
    qDebug() << "BoardParameterHistoryStorage: Loaded" << m_sessionParameters.size() << "parameters for session" << sessionId;
    
    emit sessionDataLoaded(sessionId);
}

void BoardParameterHistoryStorage::clear()
{
    m_sessionParameters.clear();
}

QList<BoardParameterSingle*> BoardParameterHistoryStorage::getSessionParameters() const
{
    return m_sessionParameters;
}

QList<BoardParameterSingle*> BoardParameterHistoryStorage::getParametersInTimeRange(const QDateTime &startTime, const QDateTime &endTime) const
{
    QList<BoardParameterSingle*> result;
    
    for (BoardParameterSingle* param : m_sessionParameters)
    {
        if (param && param->timestamp() >= startTime && param->timestamp() <= endTime)
        {
            result.append(param);
        }
    }
    
    return result;
}
