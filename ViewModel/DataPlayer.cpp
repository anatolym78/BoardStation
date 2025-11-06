#include "DataPlayer.h"
#include <QDebug>
#include <QDateTime>
#include <QThread>

DataPlayer::DataPlayer(QObject *parent)
	: QObject(parent)
	, m_storage(nullptr)
	, m_shouldStop(0)
	, m_isPlaying(false)
	, m_currentPosition(QDateTime())
{
}

DataPlayer::~DataPlayer()
{
	//stop();
}

void DataPlayer::setStorage(ParameterTreeStorage* storage)
{
	m_storage = storage;
}

void DataPlayer::onParameterReceived(BoardParameterSingle* parameter)
{
	// Базовая реализация - просто эмитируем сигнал parameterPlayed
	if (parameter)
	{
		emit parameterPlayed(parameter, false);
	}
}

void DataPlayer::play()
{
	if (!m_isPlaying)
	{
		m_isPlaying = true;
		m_shouldStop = 0;
		startPlayback();
		emit isPlayingChanged();
	}
}

void DataPlayer::stop()
{
	if (m_isPlaying)
	{
		m_isPlaying = false;
		m_shouldStop = 1;
		stopPlayback();
		emit isPlayingChanged();
	}
	
	// Сбрасываем позицию на начало сессии
	m_currentPosition = m_sessionStartTime;
	emit currentPositionChanged();
	emit elapsedTimeChanged();
	emit stopped();
}

void DataPlayer::pause()
{
	if (m_isPlaying)
	{
		m_isPlaying = false;
		m_shouldStop = 1;
		stopPlayback();
		emit isPlayingChanged();
	}
}

void DataPlayer::setPosition(QDateTime position)
{
	// Ограничиваем позицию в пределах сессии
	if (position < m_sessionStartTime)
	{
		position = m_sessionStartTime;
	}
	else if (position > m_sessionEndTime)
	{
		position = m_sessionEndTime;
	}
	
	// Проигрываем параметры, которые попадают в текущий временной интервал
	if (m_storage && !m_isPlaying)
	{
		playParametersInTimeRange(m_currentPosition, position);
	}

	m_currentPosition = position;

	emit currentPositionChanged();
	emit elapsedTimeChanged();
}

void DataPlayer::setElapsedTime(double position)
{
	setPosition(m_sessionStartTime.addSecs(position));
}

void DataPlayer::moveToBegin()
{
	setPosition(m_sessionStartTime);
}

void DataPlayer::onMoved(double value)
{
	setPosition(m_sessionStartTime.addMSecs(value));

}

void DataPlayer::playParametersInTimeRange(const QDateTime& startTime, const QDateTime& endTime)
{
	if (!m_storage)
	{
		return;
	}

	if (startTime == endTime) return;

	auto _startTime = startTime;
	auto _endTime = endTime;
	auto isReverse = _startTime > _endTime;
	if (isReverse)
	{
		std::swap(_startTime, _endTime);
	}

	auto paramsInRange = m_storage->getParametersInTimeRange(_startTime, _endTime);
	if (isReverse)
	{
		std::reverse(paramsInRange.begin(), paramsInRange.end());
	}

	for (BoardParameterSingle* param : paramsInRange)
	{
		if (param)
		{
			emit parameterPlayed(param, isReverse);
		}
	}
}

void DataPlayer::startPlayback()
{
	// Запускаем асинхронный поток проигрывания
	m_playbackFuture = QtConcurrent::run(this, &DataPlayer::playbackLoop);
}

void DataPlayer::stopPlayback()
{
	// Ожидаем завершения потока проигрывания
	if (m_playbackFuture.isRunning())
	{
		m_playbackFuture.waitForFinished();
	}
}

void DataPlayer::playbackLoop()
{
	while (!m_shouldStop.loadAcquire())
	{
		if (m_isPlaying)
		{
			updatePlaybackPosition();
			
			// Проверяем, не достигли ли конца сессии
			if (m_currentPosition >= m_sessionEndTime)
			{
				// Завершаем проигрывание
				m_shouldStop = 1;
				emit playbackFinished();
				break;
			}
		}
		
		// Небольшая пауза, чтобы не нагружать CPU
		QThread::msleep(100);
	}
}

void DataPlayer::updatePlaybackPosition()
{
	// Увеличиваем позицию на 0.1 секунды (100мс)
	m_currentPosition = QDateTime::currentDateTime();// m_currentPosition.addMSecs(100);
	emit currentPositionChanged();
	emit elapsedTimeChanged();
}

void DataPlayer::clearStorage()
{
	if (m_storage)
	{
		m_storage->clear();
	}
}

double DataPlayer::sessionDuration() const
{
	if (m_sessionStartTime.isNull() || m_sessionEndTime.isNull())
	{
		return 0.0;
	}
	
	qint64 durationMs = m_sessionStartTime.msecsTo(m_sessionEndTime);
	return durationMs / 1000.0; // Конвертируем в секунды
}

double DataPlayer::elapsedTime() const
{
	if (m_sessionStartTime.isNull() || m_currentPosition.isNull())
	{
		return 0.0;
	}
	
	qint64 elapsedMs = m_sessionStartTime.msecsTo(m_currentPosition);
	return elapsedMs / 1000.0; // Конвертируем в секунды
}
