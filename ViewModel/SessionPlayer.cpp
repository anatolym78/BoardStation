#include "SessionPlayer.h"
#include <QDebug>
#include <QDateTime>

SessionPlayer::SessionPlayer(QObject *parent)
	: DataPlayer(parent)
	, m_reader(nullptr)
	, m_currentSessionId(-1)
	, m_lastPlayedIndex(-1)
	, m_lastPlayedPosition(QDateTime())
{
	connect(this, &DataPlayer::stopped, this, [this]()
		{
			initialPlay();
		});
}

SessionPlayer::~SessionPlayer()
{
	//stop();
}

void SessionPlayer::setStorage(BoardParameterHistoryStorage* storage)
{
	// Вызываем базовую реализацию
	DataPlayer::setStorage(storage);
	
	// Подключаемся к сигналу загрузки данных сессии
	if (storage)
	{
		connect(storage, &BoardParameterHistoryStorage::sessionDataLoaded,
				this, &SessionPlayer::onSessionDataLoaded);
	}
}

void SessionPlayer::setReader(BoardMessagesSqliteReader* reader)
{
	m_reader = reader;
}

void SessionPlayer::onSessionDataLoaded(int sessionId)
{
	if (!m_reader)
	{
		return;
	}
	
	// Получаем информацию о сессии
	auto sessionInfo = m_reader->getSessionInfo(sessionId);
	if (sessionInfo.id == -1)
	{
		qWarning() << "SessionPlayer: Session not found:" << sessionId;
		return;
	}
	
	m_currentSessionId = sessionId;
	m_currentSessionName = sessionInfo.name;
	
	// Получаем параметры из хранилища для определения временных границ
	QList<BoardParameterSingle*> sessionParams = m_storage->getSessionParameters();
	
	if (!sessionParams.isEmpty())
	{
		// Определяем время начала и конца сессии
		auto firstParam = sessionParams.first();
		auto lastParam = sessionParams.last();
		
		if (firstParam && lastParam)
		{
			m_sessionStartTime = firstParam->timestamp();
			m_sessionEndTime = lastParam->timestamp();
		}
	}
	else
	{
		// Если параметров нет, используем время создания сессии
		m_sessionStartTime = sessionInfo.createdAt;
		m_sessionEndTime = m_sessionStartTime;
	}
	
	// Устанавливаем курсор на начало сессии (после определения временных границ)
	m_currentPosition = m_sessionStartTime;
	m_lastPlayedIndex = -1;
	m_lastPlayedPosition = m_sessionStartTime;
	
	emit currentSessionNameChanged();
	emit sessionStartTimeChanged();
	emit currentPositionChanged();
	emit elapsedTimeChanged();
	
	emit sessionEndTimeChanged();
	emit sessionDurationChanged();
}

void SessionPlayer::initialPlay()
{
	playParametersInTimeRange(m_sessionStartTime, m_sessionStartTime.addMSecs(1));
}

void SessionPlayer::moveToBegin()
{
	DataPlayer::moveToBegin();
}

void SessionPlayer::initializeWithLoadedData()
{
	if (!m_storage)// || !m_reader)
	{
		qWarning() << "SessionPlayer: Storage or reader not available for initialization";
		return;
	}
	
	// Получаем параметры из хранилища
	QList<BoardParameterSingle*> sessionParams = m_storage->getSessionParameters();
	if (sessionParams.isEmpty())
	{
		qWarning() << "SessionPlayer: No data available for initialization";
		return;
	}
	
	// Определяем время начала и конца сессии
	auto firstParam = sessionParams.first();
	auto lastParam = sessionParams.last();
	
	if (firstParam && lastParam)
	{
		m_sessionStartTime = firstParam->timestamp();
		m_sessionEndTime = lastParam->timestamp();
		m_currentPosition = m_sessionStartTime;
		m_lastPlayedIndex = -1;
		m_lastPlayedPosition = m_sessionStartTime;
		
		// Получаем информацию о сессии из первого параметра или используем текущее время
		m_currentSessionName = QString("Session %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
		
		emit currentSessionNameChanged();
		emit sessionStartTimeChanged();
		emit sessionEndTimeChanged();
		emit sessionDurationChanged();
		emit currentPositionChanged();
		emit elapsedTimeChanged();
		
		qDebug() << "SessionPlayer: Initialized with loaded data from" 
				 << m_sessionStartTime.toString() << "to" << m_sessionEndTime.toString();
	}
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

