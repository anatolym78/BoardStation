#include "RecordedSession.h"
#include <QDebug>

RecordedSession::RecordedSession(const BoardMessagesSqliteReader::SessionInfo& sessionInfo, 
                               QObject *parent)
    : Session(parent)
    , m_sessionInfo(sessionInfo)
{
    qDebug() << "RecordedSession: Created session" << sessionInfo.id << "with name" << sessionInfo.name;
}

void RecordedSession::updateSessionInfo(const BoardMessagesSqliteReader::SessionInfo& sessionInfo)
{
    bool changed = false;
    
    if (m_sessionInfo.messageCount != sessionInfo.messageCount)
    {
        m_sessionInfo.messageCount = sessionInfo.messageCount;
        changed = true;
    }
    
    if (m_sessionInfo.parameterCount != sessionInfo.parameterCount)
    {
        m_sessionInfo.parameterCount = sessionInfo.parameterCount;
        changed = true;
    }
    
    if (m_sessionInfo.name != sessionInfo.name)
    {
        m_sessionInfo.name = sessionInfo.name;
        changed = true;
    }
    
    if (m_sessionInfo.description != sessionInfo.description)
    {
        m_sessionInfo.description = sessionInfo.description;
        changed = true;
    }
    
    if (changed)
    {
        emit sessionChanged();
    }
}

void RecordedSession::updateMessageCount(int count)
{
    if (m_sessionInfo.messageCount != count)
    {
        m_sessionInfo.messageCount = count;
        emit messageCountChanged(count);
        emit sessionChanged();
    }
}

void RecordedSession::updateParameterCount(int count)
{
    if (m_sessionInfo.parameterCount != count)
    {
        m_sessionInfo.parameterCount = count;
        emit parameterCountChanged(count);
        emit sessionChanged();
    }
}
