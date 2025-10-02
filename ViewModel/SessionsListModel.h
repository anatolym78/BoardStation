#ifndef SESSIONSLISTMODEL_H
#define SESSIONSLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QDateTime>
#include "Model/Parameters/BoardMessagesSqliteReader.h"

class SessionsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum SessionRoles 
    {
        SessionIdRole = Qt::UserRole + 1,
        SessionNameRole,
        CreatedAtRole,
        CreatedAtFormattedRole,
        MessageCountRole,
        ParameterCountRole,
        DescriptionRole,
        RecordingRole,
        RecordedSessionRole,
    };

    explicit SessionsListModel(QObject *parent = nullptr);

    Q_INVOKABLE bool isRecordingState() const { return m_recording;}
    Q_INVOKABLE void startRecording ();
    Q_INVOKABLE void stopRecording();
    Q_INVOKABLE void setRecordingState(bool enable);
    
    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // Методы для работы с данными
    void setReader(BoardMessagesSqliteReader *reader);
    Q_INVOKABLE void refreshSessions();
    Q_INVOKABLE void addSession(const BoardMessagesSqliteReader::SessionInfo &sessionInfo);
    Q_INVOKABLE void updateSessionMessageCount(int sessionId, int messageCount);
    Q_INVOKABLE void removeSession(int index);
    
    // Получение информации о сессии
    BoardMessagesSqliteReader::SessionInfo getSessionInfo(int index) const;
    int getSessionId(int index) const;
    int findSessionIndex(int sessionId) const;

signals:
    void sessionsRefreshed();
    void errorOccurred(const QString &error);

private:
    BoardMessagesSqliteReader *m_reader;
    bool m_recording = false;
    QList<BoardMessagesSqliteReader::SessionInfo> m_sessions;
};

#endif // SESSIONSLISTMODEL_H
