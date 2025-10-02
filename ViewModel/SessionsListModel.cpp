#include "SessionsListModel.h"
#include <QDebug>

SessionsListModel::SessionsListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_reader(nullptr)
{
}

Q_INVOKABLE void SessionsListModel::startRecording ()
{
    setRecordingState(true);
}

Q_INVOKABLE void SessionsListModel::stopRecording()
{
    setRecordingState(false);
}

Q_INVOKABLE void SessionsListModel::setRecordingState(bool enable)
{
    m_recording = enable;

    emit dataChanged(this->index(0, 0), this->index(rowCount() - 1, 0));
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
        case CreatedAtFormattedRole:
            return session.createdAt.toString("dd.MM.yyyy hh:mm");
        case MessageCountRole:
            return session.messageCount;
        case ParameterCountRole:
            return session.parameterCount;
        case DescriptionRole:
            return session.description;
        case Qt::DisplayRole:
            return QString("%1 (%2 сообщений)").arg(session.name).arg(session.messageCount);
        case RecordedSessionRole:
            return index.row() == 0 && m_recording;
        case RecordingRole:
            return m_recording;
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
    roles[CreatedAtFormattedRole] = "createdAtFormatted";
    roles[MessageCountRole] = "messageCount";
    roles[ParameterCountRole] = "parameterCount";
    roles[DescriptionRole] = "description";
    roles[RecordingRole] = "recordingRole";
    roles[RecordedSessionRole] = "recordedSession";

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

void SessionsListModel::addSession(const BoardMessagesSqliteReader::SessionInfo &sessionInfo)
{
    // Проверяем, не существует ли уже сессия с таким ID
    for (int i = 0; i < m_sessions.size(); ++i)
    {
        if (m_sessions.at(i).id == sessionInfo.id)
        {
            qDebug() << "SessionsListModel: Session with ID" << sessionInfo.id << "already exists";
            return;
        }
    }
    
    // Добавляем новую сессию в начало списка
    beginInsertRows(QModelIndex(), 0, 0);
    m_sessions.prepend(sessionInfo);
    endInsertRows();
    
    qDebug() << "SessionsListModel: Added new session" << sessionInfo.id << "with name" << sessionInfo.name;
}

void SessionsListModel::updateSessionMessageCount(int sessionId, int messageCount)
{
    int index = findSessionIndex(sessionId);
    if (index >= 0 && index < m_sessions.size())
    {
        if (m_sessions[index].messageCount != messageCount)
        {
            m_sessions[index].messageCount = messageCount;
            
            // Уведомляем об изменении данных
            QModelIndex modelIndex = createIndex(index, 0);
            emit dataChanged(modelIndex, modelIndex, {MessageCountRole});
            
            qDebug() << "SessionsListModel: Updated message count for session" << sessionId << "to" << messageCount;
        }
    }
}

void SessionsListModel::removeSession(int index)
{
    if (index < 0 || index >= m_sessions.size())
    {
        qWarning() << "SessionsListModel: Invalid index for session removal:" << index;
        return;
    }
    
    // Получаем ID сессии для удаления из базы данных
    int sessionId = m_sessions.at(index).id;
    
    // Удаляем сессию из списка
    beginRemoveRows(QModelIndex(), index, index);
    m_sessions.removeAt(index);
    endRemoveRows();
    
    qDebug() << "SessionsListModel: Removed session at index" << index << "with ID" << sessionId;
    
    // Удаляем сессию из базы данных
    if (m_reader)
    {
        if (!m_reader->removeSession(sessionId))
        {
            qWarning() << "SessionsListModel: Failed to remove session" << sessionId << "from database";
            // Можно добавить сигнал об ошибке
            emit errorOccurred(QString("Не удалось удалить сессию %1 из базы данных").arg(sessionId));
        }
    }
}

int SessionsListModel::findSessionIndex(int sessionId) const
{
    for (int i = 0; i < m_sessions.size(); ++i)
    {
        if (m_sessions.at(i).id == sessionId)
        {
            return i;
        }
    }
    return -1;
}

