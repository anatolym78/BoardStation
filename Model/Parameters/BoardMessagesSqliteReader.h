#ifndef BOARDMESSAGESSQLITEREADER_H
#define BOARDMESSAGESSQLITEREADER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QList>
#include "BoardParameterSingle.h"

class BoardMessagesSqliteReader : public QObject
{
    Q_OBJECT

public:
    explicit BoardMessagesSqliteReader(const QString &databasePath = "BoardStationData.db", 
                                      QObject *parent = nullptr);
    ~BoardMessagesSqliteReader();
    
    // Получение списка доступных сессий
    struct SessionInfo 
    {
        int id;
        QString name;
        QDateTime createdAt;
        QString description;
        int messageCount;
        int parameterCount;
    };
    
    QList<SessionInfo> getAvailableSessions();
    
    // Получение данных сессии
    QList<BoardParameterSingle*> getSessionParameters(int sessionId, 
                                                     const QString &parameterLabel = QString());
    
    // Получение данных за период времени
    QList<BoardParameterSingle*> getSessionParametersByTimeRange(int sessionId,
                                                                const QDateTime &startTime,
                                                                const QDateTime &endTime,
                                                                const QString &parameterLabel = QString());
    
    // Получение списка параметров в сессии
    QStringList getSessionParameterLabels(int sessionId);
    
    // Получение статистики сессии
    SessionInfo getSessionInfo(int sessionId);
    
    // Экспорт данных сессии в JSON (для совместимости)
    QString exportSessionToJson(int sessionId);
    
    // Удаление сессии
    bool removeSession(int sessionId);

signals:
    void readError(const QString &error);

private:
    // Инициализация базы данных
    bool initializeDatabase();
    
    // Создание объекта BoardParameterSingle из данных БД
    BoardParameterSingle* createParameterFromQuery(const QSqlQuery &query) const;

private:
    QString m_databasePath;
    QSqlDatabase m_database;
};

#endif // BOARDMESSAGESSQLITEREADER_H
