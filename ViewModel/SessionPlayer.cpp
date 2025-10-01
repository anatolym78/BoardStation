#include "SessionPlayer.h"
#include <QDebug>
#include <QDateTime>

SessionPlayer::SessionPlayer(QObject *parent)
    : QObject(parent)
    , m_reader(nullptr)
    , m_storage(nullptr)
    , m_playbackTimer(new QTimer(this))
    , m_isPlaying(false)
    , m_currentPosition(0.0)
    , m_maxPosition(0.0)
    , m_currentSessionId(-1)
    , m_currentDataIndex(0)
{
    // Настройка таймера воспроизведения (каждые 100мс для плавности)
    m_playbackTimer->setInterval(100);
    connect(m_playbackTimer, &QTimer::timeout, this, &SessionPlayer::onPlaybackTimer);
}

SessionPlayer::~SessionPlayer()
{
    stop();
}

void SessionPlayer::setReader(BoardMessagesSqliteReader* reader)
{
    m_reader = reader;
}

void SessionPlayer::setStorage(BoardParameterHistoryStorage* storage)
{
    m_storage = storage;
}

void SessionPlayer::loadSession(int sessionId)
{
    if (!m_reader)
    {
        qWarning() << "SessionPlayer: Reader is not set";
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
    m_sessionStartTime = sessionInfo.createdAt;
    
    // Загружаем данные сессии
    loadSessionData(sessionId);
    
    // Устанавливаем курсор на начало
    m_currentPosition = 0.0;
    m_currentDataIndex = 0;
    
    emit currentSessionNameChanged();
    emit sessionStartTimeChanged();
    emit currentPositionChanged();
    emit sessionLoaded(sessionId, sessionInfo.name);
    
    qDebug() << "SessionPlayer: Loaded session" << sessionId << ":" << sessionInfo.name;
}

void SessionPlayer::play()
{
    if (m_currentSessionId == -1)
    {
        qWarning() << "SessionPlayer: No session loaded";
        return;
    }
    
    if (!m_isPlaying)
    {
        m_isPlaying = true;
        m_playbackTimer->start();
        emit isPlayingChanged();
        qDebug() << "SessionPlayer: Started playback";
    }
}

void SessionPlayer::stop()
{
    if (m_isPlaying)
    {
        m_isPlaying = false;
        m_playbackTimer->stop();
        emit isPlayingChanged();
        qDebug() << "SessionPlayer: Stopped playback";
    }
    
    // Сбрасываем позицию на начало
    m_currentPosition = 0.0;
    m_currentDataIndex = 0;
    emit currentPositionChanged();
    
    // Очищаем хранилище
    clearStorage();
}

void SessionPlayer::pause()
{
    if (m_isPlaying)
    {
        m_isPlaying = false;
        m_playbackTimer->stop();
        emit isPlayingChanged();
        qDebug() << "SessionPlayer: Paused playback";
    }
}

void SessionPlayer::setPosition(double position)
{
    if (m_currentSessionId == -1)
    {
        return;
    }
    
    // Ограничиваем позицию в пределах сессии
    position = qBound(0.0, position, m_maxPosition);
    
    m_currentPosition = position;
    
    // Находим соответствующий индекс данных
    if (!m_sessionData.isEmpty())
    {
        double timePerDataPoint = m_maxPosition / m_sessionData.size();
        m_currentDataIndex = static_cast<int>(position / timePerDataPoint);
        m_currentDataIndex = qBound(0, m_currentDataIndex, m_sessionData.size() - 1);
    }
    
    emit currentPositionChanged();
    qDebug() << "SessionPlayer: Position set to" << position << "seconds";
}

void SessionPlayer::onPlaybackTimer()
{
    if (!m_isPlaying || m_currentSessionId == -1)
    {
        return;
    }
    
    updatePlaybackPosition();
    
    // Проверяем, не достигли ли конца сессии
    if (m_currentPosition >= m_maxPosition)
    {
        stop();
        emit playbackFinished();
        qDebug() << "SessionPlayer: Playback finished";
    }
}

void SessionPlayer::updatePlaybackPosition()
{
    if (m_sessionData.isEmpty())
    {
        return;
    }
    
    // Увеличиваем позицию на 0.1 секунды (100мс)
    m_currentPosition += 0.1;
    
    // Обновляем данные в хранилище
    if (m_currentDataIndex < m_sessionData.size())
    {
        auto parameter = m_sessionData[m_currentDataIndex];
        if (parameter && m_storage)
        {
            m_storage->addParameter(parameter);
        }
        
        // Переходим к следующему элементу данных
        m_currentDataIndex++;
    }
    
    emit currentPositionChanged();
}

void SessionPlayer::loadSessionData(int sessionId)
{
    if (!m_reader)
    {
        return;
    }
    
    // Загружаем все параметры сессии
    m_sessionData = m_reader->getSessionParameters(sessionId, "");
    
    if (!m_sessionData.isEmpty())
    {
        // Определяем время начала и конца сессии
        auto firstParam = m_sessionData.first();
        auto lastParam = m_sessionData.last();
        
        if (firstParam && lastParam)
        {
            m_sessionStartTime = firstParam->timestamp();
            m_sessionEndTime = lastParam->timestamp();
            
            // Вычисляем общую длительность в секундах
            qint64 durationMs = m_sessionStartTime.msecsTo(m_sessionEndTime);
            m_maxPosition = durationMs / 1000.0; // Конвертируем в секунды
        }
    }
    else
    {
        m_maxPosition = 0.0;
        m_sessionEndTime = m_sessionStartTime;
    }
    
    emit sessionEndTimeChanged();
    emit maxPositionChanged();
    
    qDebug() << "SessionPlayer: Loaded" << m_sessionData.size() << "data points";
    qDebug() << "SessionPlayer: Duration:" << m_maxPosition << "seconds";
}

void SessionPlayer::clearStorage()
{
    if (m_storage)
    {
        m_storage->clear();
        qDebug() << "SessionPlayer: Cleared storage";
    }
}
