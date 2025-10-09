#ifndef RECORDEDSESSION_H
#define RECORDEDSESSION_H

#include "Session.h"
#include "Model/Parameters/BoardMessagesSqliteReader.h"

/**
 * @brief Класс для представления записанной сессии из базы данных
 */
class RecordedSession : public Session
{
    Q_OBJECT

public:
    explicit RecordedSession(const BoardMessagesSqliteReader::SessionInfo& sessionInfo, 
                            QObject *parent = nullptr);
    ~RecordedSession() = default;

    // Реализация виртуальных методов
    int getId() const override { return m_sessionInfo.id; }
    QString getName() const override { return m_sessionInfo.name; }
    QDateTime getCreatedAt() const override { return m_sessionInfo.createdAt; }
    QString getDescription() const override { return m_sessionInfo.description; }
    int getMessageCount() const override { return m_sessionInfo.messageCount; }
    int getParameterCount() const override { return m_sessionInfo.parameterCount; }
    SessionType getType() const override { return Session::RecordedSession; }
    bool isRecording() const override { return false; }

    // Методы для обновления данных сессии
    void updateSessionInfo(const BoardMessagesSqliteReader::SessionInfo& sessionInfo);
    void updateMessageCount(int count);
    void updateParameterCount(int count);

    // Получение полной информации о сессии
    const BoardMessagesSqliteReader::SessionInfo& getSessionInfo() const { return m_sessionInfo; }

private:
    BoardMessagesSqliteReader::SessionInfo m_sessionInfo;
};

#endif // RECORDEDSESSION_H
