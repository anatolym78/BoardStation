#include "LiveSession.h"
#include "Model/Parameters/BoardParameterHistoryStorage.h"
#include <QDebug>
#include <QDateTime>

LiveSession::LiveSession(QObject *parent)
    : Session(parent)
    , m_startTime(QDateTime::currentDateTime())
    , m_messageCount(0)
    , m_parameterCount(0)
    , m_isRecording(false)
    , m_storage(new BoardParameterHistoryStorage(this))
{
    connect(m_storage, &BoardParameterHistoryStorage::parameterEmitted,
        [this](BoardParameterSingle* parameter)
        {
            m_messageCount = m_storage->getMessagesCount();

			emit messageCountChanged(m_messageCount);
        });
}

void LiveSession::startSession()
{
    if (m_startTime.isNull())
    {
        m_startTime = QDateTime::currentDateTime();
        emit sessionStarted();
        qDebug() << "LiveSession: Session started at" << m_startTime.toString();
    }
}

void LiveSession::stopSession()
{
    if (!m_startTime.isNull())
    {
        qDebug() << "LiveSession: Session stopped at" << QDateTime::currentDateTime().toString();
        emit sessionStopped();
    }
}

void LiveSession::setRecording(bool recording)
{
    if (m_isRecording != recording)
    {
        m_isRecording = recording;
        emit recordingStateChanged(recording);
        emit sessionChanged();
        qDebug() << "LiveSession: Recording state changed to" << recording;
    }
}

void LiveSession::incrementMessageCount()
{
    m_messageCount++;
    emit messageCountChanged(m_messageCount);
    emit sessionChanged();
}

void LiveSession::incrementParameterCount()
{
    m_parameterCount++;
    emit parameterCountChanged(m_parameterCount);
    emit sessionChanged();
}

void LiveSession::resetCounters()
{
    m_messageCount = 0;
    m_parameterCount = 0;
    emit messageCountChanged(m_messageCount);
    emit parameterCountChanged(m_parameterCount);
    emit sessionChanged();
    qDebug() << "LiveSession: Counters reset";
}

BoardParameterHistoryStorage* LiveSession::getStorage() const
{
    return m_storage;
}

void LiveSession::clearStorage()
{
    if (m_storage)
    {
        m_storage->clear();
        qDebug() << "LiveSession: Storage cleared";
    }
}
