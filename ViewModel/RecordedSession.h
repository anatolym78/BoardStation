#ifndef RECORDEDSESSION_H
#define RECORDEDSESSION_H

#include "Session.h"
#include "Model/Parameters/BoardMessagesSqliteReader.h"

class BoardParameterHistoryStorage;

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

    // Реализация методов для работы с хранилищем
    BoardParameterHistoryStorage* getStorage() const override;
    void clearStorage() override;

    // Методы для загрузки данных из базы
    void loadDataFromDatabase(BoardMessagesSqliteReader* reader);

    // Методы для обновления данных сессии
    void updateSessionInfo(const BoardMessagesSqliteReader::SessionInfo& sessionInfo);
    void updateMessageCount(int count);
    void updateParameterCount(int count);

    // Получение полной информации о сессии
    const BoardMessagesSqliteReader::SessionInfo& getSessionInfo() const { return m_sessionInfo; }
    
    // Проверка, загружены ли данные в хранилище
    bool isDataLoaded() const;

	void open() override;

private:
    BoardMessagesSqliteReader::SessionInfo m_sessionInfo;
    BoardParameterHistoryStorage* m_storage;
};

#endif // RECORDEDSESSION_H
