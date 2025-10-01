#include "SessionsListModel.h"
#include <QDebug>

SessionsListModel::SessionsListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_reader(nullptr)
{
}

int SessionsListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_sessions.size();
}

QVariant SessionsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_sessions.size())
    {
        return QVariant();
    }
    
    const BoardMessagesSqliteReader::SessionInfo &session = m_sessions.at(index.row());
    
    switch (role) 
    {
        case SessionIdRole:
            return session.id;
        case SessionNameRole:
            return session.name;
        case CreatedAtRole:
            return session.createdAt;
        case MessageCountRole:
            return session.messageCount;
        case ParameterCountRole:
            return session.parameterCount;
        case DescriptionRole:
            return session.description;
        case Qt::DisplayRole:
            return QString("%1 (%2 сообщений)").arg(session.name).arg(session.messageCount);
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> SessionsListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[SessionIdRole] = "sessionId";
    roles[SessionNameRole] = "sessionName";
    roles[CreatedAtRole] = "createdAt";
    roles[MessageCountRole] = "messageCount";
    roles[ParameterCountRole] = "parameterCount";
    roles[DescriptionRole] = "description";
    return roles;
}

void SessionsListModel::setReader(BoardMessagesSqliteReader *reader)
{
    if (m_reader != reader) 
    {
        m_reader = reader;
        refreshSessions();
    }
}

void SessionsListModel::refreshSessions()
{
    if (!m_reader) 
    {
        qWarning() << "SessionsListModel: Reader is not set";
        emit errorOccurred("Reader не установлен");
        return;
    }
    
    beginResetModel();
    
    try 
    {
        m_sessions = m_reader->getAvailableSessions();
        
        // Сортируем сессии по дате создания в убывающем порядке (новые сверху)
        std::sort(m_sessions.begin(), m_sessions.end(), 
                 [](const BoardMessagesSqliteReader::SessionInfo &a, 
                    const BoardMessagesSqliteReader::SessionInfo &b) 
                 {
                     return a.createdAt > b.createdAt;
                 });
        
        qDebug() << "SessionsListModel: Загружено" << m_sessions.size() << "сессий";
    }
    catch (const std::exception &e) 
    {
        qWarning() << "SessionsListModel: Ошибка при загрузке сессий:" << e.what();
        emit errorOccurred(QString("Ошибка загрузки сессий: %1").arg(e.what()));
        m_sessions.clear();
    }
    
    endResetModel();
    emit sessionsRefreshed();
}

BoardMessagesSqliteReader::SessionInfo SessionsListModel::getSessionInfo(int index) const
{
    if (index >= 0 && index < m_sessions.size()) 
    {
        return m_sessions.at(index);
    }
    
    BoardMessagesSqliteReader::SessionInfo emptyInfo;
    emptyInfo.id = -1;
    return emptyInfo;
}

int SessionsListModel::getSessionId(int index) const
{
    if (index >= 0 && index < m_sessions.size()) 
    {
        return m_sessions.at(index).id;
    }
    return -1;
}
