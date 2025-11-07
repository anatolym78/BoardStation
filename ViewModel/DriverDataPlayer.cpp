#include "DriverDataPlayer.h"
#include <QDebug>
#include <QDateTime>

#include "./Model/Parameters/Tree/ParameterTreeHistoryItem.h"

DriverDataPlayer::DriverDataPlayer(QObject *parent)
	: DataPlayer(parent)
	, m_isInitialized(false)
{
	m_isPlayable = false;

	m_currentSessionName = "Live Data";
	emit currentSessionNameChanged();
}

DriverDataPlayer::~DriverDataPlayer()
{
	//stop();
}

void DriverDataPlayer::setStorage(ParameterTreeStorage* storage)
{
	if (m_storage) return; // !!!

	DataPlayer::setStorage(storage);

	connect(m_storage, &ParameterTreeStorage::valueAdded,
		this, &DriverDataPlayer::onStorageValueAdded);
	
	//resetState();
}

void DriverDataPlayer::onStorageValueAdded(ParameterTreeHistoryItem* historyItem)
{
	if (!m_isInitialized)
	{
		m_sessionStartTime = historyItem->lastTimestamp();
		m_currentPosition = m_sessionStartTime;
		m_sessionEndTime = m_sessionStartTime.addSecs(TIME_RANGE);

		m_isInitialized = true;

		emitTimeRangeSignals();

		play();
	}
}
void DriverDataPlayer::play()
{
	// В режиме реального времени просто устанавливаем флаг воспроизведения
	// Позиция обновляется при получении новых параметров
	if (!m_isPlaying)
	{
		m_isPlaying = true;
		m_shouldStop = 0;
		startPlayback();
		emit isPlayingChanged();
	}
}

void DriverDataPlayer::stop()
{
	DataPlayer::stop();
	
	// Сбрасываем состояние инициализации
	m_isInitialized = false;
}

void DriverDataPlayer::pause()
{
	// В режиме реального времени просто останавливаем воспроизведение
	if (m_isPlaying)
	{
		m_isPlaying = false;
		m_shouldStop = 1;
		stopPlayback();
		emit isPlayingChanged();
	}
}

void DriverDataPlayer::setPosition(QDateTime position)
{
	// В режиме реального времени ограничиваем позицию текущим временем
	if (position < m_sessionStartTime)
	{
		position = m_sessionStartTime;
	}
	else if (position > m_sessionEndTime)
	{
		position = m_sessionEndTime;
	}
	
	m_currentPosition = position;
	emit currentPositionChanged();
	emit elapsedTimeChanged();
}

void DriverDataPlayer::resetState()
{
	// Останавливаем плеер
	stop();
	
	// Сбрасываем состояние инициализации
	m_isInitialized = false;
	
	// Сбрасываем временные диапазоны
	m_sessionStartTime = QDateTime();
	m_sessionEndTime = QDateTime();
	m_currentPosition = QDateTime();
	
	// Эмитируем сигналы об изменении состояния
	emitTimeRangeSignals();
	
	qDebug() << "DriverDataPlayer: State reset - ready for new live session";
}

void DriverDataPlayer::moveToBegin()
{

}

void DriverDataPlayer::reset()
{
	m_isInitialized = false;
}

void DriverDataPlayer::updatePlaybackPosition()
{
	// В режиме реального времени обновляем позицию на текущее время
	QDateTime currentTime = QDateTime::currentDateTime();
	
	// Ограничиваем позицию в пределах сессии
	if (currentTime < m_sessionStartTime)
	{
		currentTime = m_sessionStartTime;
	}
	else
	{
		if (currentTime > m_sessionEndTime)
		{
			currentTime = m_sessionEndTime;
		}
	}

	auto storageRange = m_storage->extractRange(m_currentPosition, currentTime);

	emit played(storageRange, false);
	
	m_currentPosition = currentTime;
	
	// Проверяем, нужно ли расширить диапазон за 10 секунд до конца
	QDateTime thresholdTime = m_sessionEndTime.addSecs(-10);
	if (currentTime >= thresholdTime)
	{
		extendTimeRange();
	}
	
	// Проверяем и проигрываем параметры
	//checkAndPlayParameters();
	
	emit currentPositionChanged();
	emit elapsedTimeChanged();
}

void DriverDataPlayer::checkAndPlayParameters()
{
	if (!m_storage || !m_isPlaying)
	{
		return;
	}

	//// Получаем все параметры из хранилища
	//QList<BoardParameterSingle*> params = m_storage->getParametersInTimeRange(m_sessionStartTime, m_sessionEndTime);
	//
	//// Проигрываем параметры, время которых меньше или равно текущей позиции
	//for (BoardParameterSingle* param : params)
	//{
	//    if (param && param->timestamp() <= m_currentPosition)
	//    {
	//        emit parameterPlayed(param, false);
	//    }
	//}
}

void DriverDataPlayer::initializeTimeRange()
{
	if (!m_storage)
	{
		return;
	}
	
	//QList<BoardParameterSingle*> params = m_storage->getParametersInTimeRange(QDateTime::fromSecsSinceEpoch(0), QDateTime::currentDateTime().addYears(100));
	//if (params.isEmpty())
	//{
	//    return;
	//}
	//
	//// Устанавливаем время начала на основе первого параметра
	//BoardParameterSingle* firstParam = params.first();
	//if (firstParam)
	//{
	//    m_sessionStartTime = firstParam->timestamp();
	//    m_sessionEndTime = m_sessionStartTime.addSecs(TIME_RANGE);
	//    m_currentPosition = m_sessionStartTime;
	//    
	//    m_isInitialized = true;
	//    
	//    emitTimeRangeSignals();
	//    
	//    qDebug() << "DriverDataPlayer: Initialized time range from" 
	//             << m_sessionStartTime.toString() << "to" << m_sessionEndTime.toString();
	//}
}

void DriverDataPlayer::extendTimeRange()
{
	// Расширяем конечное время на TIME_RANGE секунд
	m_sessionEndTime = m_sessionEndTime.addSecs(TIME_RANGE);
	
	emit sessionEndTimeChanged();
	emit sessionDurationChanged();
	
	qDebug() << "DriverDataPlayer: Extended time range to" << m_sessionEndTime.toString();
}

void DriverDataPlayer::emitTimeRangeSignals()
{
	emit sessionStartTimeChanged();
	emit sessionEndTimeChanged();
	emit sessionDurationChanged();
	emit currentPositionChanged();
	emit elapsedTimeChanged();
}
