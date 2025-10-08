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
    m_currentPosition = m_sessionStartTime;
    m_lastPlayedIndex = -1;
    
    emit currentSessionNameChanged();
    emit sessionStartTimeChanged();
    emit currentPositionChanged();
    emit elapsedTimeChanged();
    
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
        m_sessionEndTime = m_sessionStartTime;
    }
    
    emit sessionEndTimeChanged();
    emit sessionDurationChanged();
}
