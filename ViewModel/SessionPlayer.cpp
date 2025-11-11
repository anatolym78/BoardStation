#include "SessionPlayer.h"
#include <QDebug>
#include <QDateTime>
#include "Model/Parameters/Tree/ParameterTreeStorage.h"
#include "Model/Parameters/Tree/ParameterTreeItem.h"
#include "Model/Parameters/Tree/ParameterTreeHistoryItem.h"

SessionPlayer::SessionPlayer(QObject *parent)
	: DataPlayer(parent)
	, m_reader(nullptr)
	, m_currentSessionId(-1)
	, m_lastPlayedIndex(-1)
	, m_lastPlayedPosition(QDateTime())
{
	connect(this, &DataPlayer::stopped, this, [this]()
		{
			//initialPlay();
		});
}

SessionPlayer::~SessionPlayer()
{
	//stop();
}

void SessionPlayer::setStorage(ParameterTreeStorage* storage)
{
	DataPlayer::setStorage(storage);
}

void SessionPlayer::setReader(BoardMessagesSqliteReader* reader)
{
	m_reader = reader;
}

void SessionPlayer::onSessionDataLoaded(int sessionId)
{
	//if (!m_reader)
	//{
	//	return;
	//}
	//
	//// Получаем информацию о сессии
	//auto sessionInfo = m_reader->getSessionInfo(sessionId);
	//if (sessionInfo.id == -1)
	//{
	//	qWarning() << "SessionPlayer: Session not found:" << sessionId;
	//	return;
	//}
	//
	//m_currentSessionId = sessionId;
	//m_currentSessionName = sessionInfo.name;
	//
	//// Получаем параметры из хранилища для определения временных границ
	//QList<BoardParameterSingle*> sessionParams = m_storage->getParametersInTimeRange(QDateTime::fromSecsSinceEpoch(0), QDateTime::currentDateTime().addYears(100));
	//
	//if (!sessionParams.isEmpty())
	//{
	//	// Определяем время начала и конца сессии
	//	auto firstParam = sessionParams.first();
	//	auto lastParam = sessionParams.last();
	//	
	//	if (firstParam && lastParam)
	//	{
	//		m_sessionStartTime = firstParam->timestamp();
	//		m_sessionEndTime = lastParam->timestamp();
	//	}
	//}
	//else
	//{
	//	// Если параметров нет, используем время создания сессии
	//	m_sessionStartTime = sessionInfo.createdAt;
	//	m_sessionEndTime = m_sessionStartTime;
	//}
	//
	//// Устанавливаем курсор на начало сессии (после определения временных границ)
	//m_currentPosition = m_sessionStartTime;
	//m_lastPlayedIndex = -1;
	//m_lastPlayedPosition = m_sessionStartTime;
	//
	//emit currentSessionNameChanged();
	//emit sessionStartTimeChanged();
	//emit currentPositionChanged();
	//emit elapsedTimeChanged();
	//
	//emit sessionEndTimeChanged();
	//emit sessionDurationChanged();
}

void SessionPlayer::initialPlay()
{
	//playParametersInTimeRange(m_sessionStartTime, m_sessionStartTime.addMSecs(1));
}

void SessionPlayer::moveToBegin()
{
	DataPlayer::moveToBegin();
}

void SessionPlayer::initializeWithLoadedData()
{
	//return; // !!!
	if (!m_storage)
	{
		qWarning() << "SessionPlayer: Storage not available for initialization";
		return;
	}

	// Обходим дерево и находим минимальную и максимальную метки времени
	QDateTime minTs;
	QDateTime maxTs;

	std::function<void(ParameterTreeItem*)> traverse = [&](ParameterTreeItem* node)
	{
		if (!node) return;
		if (node->type() == ParameterTreeItem::ItemType::History)
		{
			auto hist = static_cast<ParameterTreeHistoryItem*>(node);
			const auto& timestamps = hist->timestamps();
			if (!timestamps.isEmpty())
			{
				const QDateTime first = timestamps.first();
				const QDateTime last = timestamps.last();
				if (minTs.isNull() || first < minTs) minTs = first;
				if (maxTs.isNull() || last > maxTs) maxTs = last;
			}
		}
		for (auto child : node->children())
		{
			traverse(child);
		}
	};

	traverse(m_storage);

	if (minTs.isNull() || maxTs.isNull())
	{
		qWarning() << "SessionPlayer: No timestamps found in storage";
		return;
	}

	m_sessionStartTime = minTs;
	m_sessionEndTime = maxTs;
	m_currentPosition = m_sessionStartTime;
	m_lastPlayedIndex = -1;
	m_lastPlayedPosition = m_sessionStartTime;

	emit sessionStartTimeChanged();
	emit sessionEndTimeChanged();
	emit sessionDurationChanged();
	emit currentPositionChanged();
	emit elapsedTimeChanged();

	qDebug() << "SessionPlayer: Initialized time range from"
			 << m_sessionStartTime.toString() << "to" << m_sessionEndTime.toString();
}

void SessionPlayer::updatePlaybackPosition()
{
	// Сохраняем предыдущую позицию для определения временного интервала
	QDateTime previousPosition = m_currentPosition;
	
	// Для SessionPlayer увеличиваем позицию на 100мс (0.1 секунды)
	m_currentPosition = m_currentPosition.addMSecs(100);
	 
	// Проверяем, не достигли ли конца сессии
	if (m_currentPosition >= m_sessionEndTime)
	{
		m_currentPosition = m_sessionEndTime;
		// Можно добавить логику завершения воспроизведения
	}
	
	// Проигрываем параметры, которые попадают в текущий временной интервал
	if (m_storage && m_isPlaying)
	{
		playParametersInTimeRange(previousPosition, m_currentPosition);
	}
	
	emit currentPositionChanged();
	emit elapsedTimeChanged();
}

