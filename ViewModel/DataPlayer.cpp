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
    stop();
}

void DataPlayer::setStorage(BoardParameterHistoryStorage* storage)
{
    m_storage = storage;
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
    
    // Очищаем хранилище
    clearStorage();
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
    
    m_currentPosition = position;
    emit currentPositionChanged();
    emit elapsedTimeChanged();
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
