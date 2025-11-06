#include "BoardParameterHistoryStorage.h"
#include "BoardMessagesSqliteReader.h"
#include <QDebug>
#include <QSet>

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

bool BoardParameterHistoryStorage::containsParameter(const QString& label)
{
	for (BoardParameterSingle* param : m_sessionParameters)
	{
		if (param && param->label() == label)
		{
			return true;
		}
	}

	return false;
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

QList<BoardParameterSingle*> BoardParameterHistoryStorage::getParametersInTimeRange(const QDateTime& startTime, const QDateTime& endTime, const QString& label) const
{
	QList<BoardParameterSingle*> result;

	for (BoardParameterSingle* param : m_sessionParameters)
	{
		if (param && param->timestamp() >= startTime && param->timestamp() <= endTime)
		{
			if (label.isEmpty() || param->label() == label)
			{
				result.append(param);
			}
		}
	}

	return result;
}

BoardParameterHistoryStorage* BoardParameterHistoryStorage::extractRange(const QDateTime& startTime, const QDateTime& endTime) const
{
	BoardParameterHistoryStorage* subStorage = new BoardParameterHistoryStorage();
	QList<BoardParameterSingle*> paramsInRange = getParametersInTimeRange(startTime, endTime);
	
	// Мы не можем просто сделать subStorage->m_sessionParameters = paramsInRange,
	// так как параметры будут удалены вместе с основным хранилищем.
	// Поэтому нужно создать глубокие копии.
	for (BoardParameterSingle* param : paramsInRange)
	{
		if (param)
		{
            //subStorage->addParameter(new BoardParameterSingle(*param));
		}
	}
	
	return subStorage;
}

int BoardParameterHistoryStorage::getMessagesCount() const
{
	QSet<QDateTime> uniqueTimestamps;
	
	for (BoardParameterSingle* param : m_sessionParameters)
	{
		if (param)
		{
			uniqueTimestamps.insert(param->timestamp());
		}
	}
	
	return uniqueTimestamps.size();
}
