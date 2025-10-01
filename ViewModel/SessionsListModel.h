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
        MessageCountRole,
        ParameterCountRole,
        DescriptionRole
    };

    explicit SessionsListModel(QObject *parent = nullptr);
    
    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // Методы для работы с данными
    void setReader(BoardMessagesSqliteReader *reader);
    void refreshSessions();
    
    // Получение информации о сессии
    BoardMessagesSqliteReader::SessionInfo getSessionInfo(int index) const;
    int getSessionId(int index) const;

signals:
    void sessionsRefreshed();
    void errorOccurred(const QString &error);

private:
    BoardMessagesSqliteReader *m_reader;
    QList<BoardMessagesSqliteReader::SessionInfo> m_sessions;
};

#endif // SESSIONSLISTMODEL_H
