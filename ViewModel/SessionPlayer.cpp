#include "SessionPlayer.h"
#include <QDebug>
#include <QDateTime>

SessionPlayer::SessionPlayer(QObject *parent)
    : DataPlayer(parent)
    , m_reader(nullptr)
    , m_currentSessionId(-1)
    , m_lastPlayedIndex(-1)
{
}

SessionPlayer::~SessionPlayer()
{
    stop();
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

void SessionPlayer::play()
{
    if (m_currentSessionId == -1)
    {
        qWarning() << "SessionPlayer: No session loaded";
        return;
    }
    
    DataPlayer::play();
}

void SessionPlayer::stop()
{
    DataPlayer::stop();
    
    // Сбрасываем индекс последнего проигранного параметра
    m_lastPlayedIndex = -1;
}

void SessionPlayer::pause()
{
    DataPlayer::pause();
}

void SessionPlayer::setPosition(double position)
{
    DataPlayer::setPosition(position);
    
    // Проигрываем параметры до текущей позиции
    playParametersToCurrentPosition();
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
    m_sessionStartTime = sessionInfo.createdAt;
    
    // Устанавливаем курсор на начало
    m_currentPosition = 0.0;
    m_lastPlayedIndex = -1;
    
    emit currentSessionNameChanged();
    emit sessionStartTimeChanged();
    emit currentPositionChanged();
    
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
}

void SessionPlayer::updatePlaybackPosition()
{
    // Увеличиваем позицию на 0.1 секунды (100мс)
    m_currentPosition += 0.1;
    
    // Проигрываем параметры до текущей позиции
    playParametersToCurrentPosition();
    
    emit currentPositionChanged();
}

void SessionPlayer::playParametersToCurrentPosition()
{
    if (!m_storage)
    {
        return;
    }
    
    QDateTime currentTime = getCurrentTimeFromPosition();
    QList<BoardParameterSingle*> sessionParams = m_storage->getSessionParameters();
    
    // Проигрываем все параметры от последнего проигранного до текущего времени
    for (int i = m_lastPlayedIndex + 1; i < sessionParams.size(); ++i)
    {
        BoardParameterSingle* param = sessionParams[i];
        if (param && param->timestamp() <= currentTime)
        {
            emit parameterPlayed(param);
            m_lastPlayedIndex = i;
        }
        else
        {
            break; // Прерываем, если достигли параметра с более поздним временем
        }
    }
}
