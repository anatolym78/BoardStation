#include "DataPlayer.h"
#include <QDebug>
#include <QDateTime>

DataPlayer::DataPlayer(QObject *parent)
    : QObject(parent)
    , m_storage(nullptr)
    , m_playbackTimer(new QTimer(this))
    , m_isPlaying(false)
    , m_currentPosition(0.0)
    , m_maxPosition(0.0)
{
    // Настройка таймера воспроизведения (каждые 100мс для плавности)
    m_playbackTimer->setInterval(100);
    connect(m_playbackTimer, &QTimer::timeout, this, &DataPlayer::onPlaybackTimer);
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
        m_playbackTimer->start();
        emit isPlayingChanged();
    }
}

void DataPlayer::stop()
{
    if (m_isPlaying)
    {
        m_isPlaying = false;
        m_playbackTimer->stop();
        emit isPlayingChanged();
    }
    
    // Сбрасываем позицию на начало
    m_currentPosition = 0.0;
    emit currentPositionChanged();
    
    // Очищаем хранилище
    clearStorage();
}

void DataPlayer::pause()
{
    if (m_isPlaying)
    {
        m_isPlaying = false;
        m_playbackTimer->stop();
        emit isPlayingChanged();
    }
}

void DataPlayer::setPosition(double position)
{
    // Ограничиваем позицию в пределах сессии
    position = qBound(0.0, position, m_maxPosition);
    
    m_currentPosition = position;
    emit currentPositionChanged();
}

void DataPlayer::onPlaybackTimer()
{
    if (!m_isPlaying)
    {
        return;
    }
    
    updatePlaybackPosition();
    
    // Проверяем, не достигли ли конца сессии
    if (m_currentPosition >= m_maxPosition)
    {
        stop();
        emit playbackFinished();
    }
}

void DataPlayer::updatePlaybackPosition()
{
    // Увеличиваем позицию на 0.1 секунды (100мс)
    m_currentPosition += 0.1;
    emit currentPositionChanged();
}

void DataPlayer::clearStorage()
{
    if (m_storage)
    {
        m_storage->clear();
    }
}

QDateTime DataPlayer::getCurrentTimeFromPosition() const
{
    if (m_maxPosition <= 0.0)
    {
        return m_sessionStartTime;
    }
    
    // Вычисляем текущее время на основе позиции
    qint64 positionMs = static_cast<qint64>(m_currentPosition * 1000.0);
    return m_sessionStartTime.addMSecs(positionMs);
}
