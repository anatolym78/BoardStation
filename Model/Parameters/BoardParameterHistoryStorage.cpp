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
        //emit parameterAdded(parameter->label());
    }

    // Добавляем параметр в историю
    if (existingHistory->addParameter(parameter)) 
    {
        //emit parameterUpdated(parameter->label());
    }

    emit newParameterAdded(parameter);
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
    
    // Очищаем список параметров сессии
    qDeleteAll(m_sessionParameters);
    m_sessionParameters.clear();
}

int BoardParameterHistoryStorage::indexOf(const QString& label) const
{
    for (auto i = 0; i < m_parameterHistories.count(); i++)
	{
        if(m_parameterHistories[i] == nullptr) continue;

        if (m_parameterHistories[i]->label() == label) return i;
	}

    return -1;
}

QList<BoardParameterHistory*> BoardParameterHistoryStorage::getAllParameterHistories() const
{
    return m_parameterHistories;
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
