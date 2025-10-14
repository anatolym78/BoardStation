#include "SessionsListModel.h"
#include <QDebug>
#include <algorithm>

SessionsListModel::SessionsListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_reader(nullptr)
    , m_recordedSessionsFactory(new RecordedSessionsFactory(this))
    , m_liveSessionFactory(new LiveSessionFactory(this))
    , m_liveSession(nullptr)
    , m_currentActiveSession(nullptr)
{
    // Подключаем сигналы фабрик
    connect(m_recordedSessionsFactory, &RecordedSessionsFactory::sessionsCreated,
            this, &SessionsListModel::onRecordedSessionsCreated);
    connect(m_liveSessionFactory, &LiveSessionFactory::liveSessionCreated,
            this, &SessionsListModel::onLiveSessionCreated);
}

SessionsListModel::~SessionsListModel()
{
    // Очищаем список сессий
    qDeleteAll(m_sessions);
    m_sessions.clear();
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
    
    Session* session = m_sessions.at(index.row());
    if (!session)
    {
        return QVariant();
    }
    
    switch (role) 
    {
        case SessionIdRole:
            return session->getId();
        case SessionNameRole:
            return session->getName();
        case CreatedAtRole:
            return session->getCreatedAt();
        case CreatedAtFormattedRole:
            return session->getCreatedAt().toString("dd.MM.yyyy hh:mm");
        case MessageCountRole:
            return session->getMessageCount();
        case ParameterCountRole:
            return session->getParameterCount();
        case DescriptionRole:
            return session->getDescription();
        case Qt::DisplayRole:
            return QString("%1 (%2 сообщений)").arg(session->getName()).arg(session->getMessageCount());
        case RecordedSessionRole:
            return session->getType() == Session::RecordedSession;
        case RecordingRole:
            return session->isRecording();
        case SessionTypeRole:
            return static_cast<int>(session->getType());
        case IsLiveSessionRole:
            return session->getType() == Session::LiveSession;
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
    roles[SessionTypeRole] = "sessionType";
    roles[IsLiveSessionRole] = "isLiveSession";

    return roles;
}

Session* SessionsListModel::currentSession() const
{
    if (m_selectedIndex >= 0 && m_selectedIndex < m_sessions.count())
    {
        return m_sessions[m_selectedIndex];
    }

    return nullptr;
}


void SessionsListModel::setReader(BoardMessagesSqliteReader *reader)
{
    if (m_reader != reader) 
    {
        m_reader = reader;
        m_recordedSessionsFactory->setReader(reader);
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
    
    // Очищаем существующие сессии
    qDeleteAll(m_sessions);
    m_sessions.clear();
    
    // Создаем живую сессию, если она еще не создана
    if (!m_liveSession)
    {
        initializeLiveSession();
    }
    
    // Создаем записанные сессии через фабрику
    QList<Session*> recordedSessions = m_recordedSessionsFactory->createSessions();
    
    // Добавляем все сессии в список
    if (m_liveSession)
    {
        m_sessions.append(m_liveSession);
    }
    m_sessions.append(recordedSessions);
    
    // Сортируем сессии
    sortSessions();
    
    qDebug() << "SessionsListModel: Загружено" << m_sessions.size() << "сессий";
    
    endResetModel();
    emit sessionsRefreshed();
}

Session* SessionsListModel::getSession(int index) const
{
    if (index >= 0 && index < m_sessions.size())
    {
        return m_sessions.at(index);
    }
    return nullptr;
}

Session* SessionsListModel::getSessionById(int sessionId) const
{
    if (sessionId == -1)
    {
        return liveSession();
    }

    for (auto session : m_sessions)
    {
        if (session->getId() == sessionId)
        {
            return session;
        }
    }

    return nullptr;
}

LiveSession* SessionsListModel::liveSession() const
{
    if (m_sessions.isEmpty()) return nullptr;

    return dynamic_cast<LiveSession*>(m_sessions[0]);
}

BoardMessagesSqliteReader::SessionInfo SessionsListModel::getSessionInfo(int index) const
{
    if (index >= 0 && index < m_sessions.size()) 
    {
        Session* session = m_sessions.at(index);
        if (session && session->getType() == Session::RecordedSession)
        {
            RecordedSession* recordedSession = qobject_cast<RecordedSession*>(session);
            if (recordedSession)
            {
                return recordedSession->getSessionInfo();
            }
        }
    }
    
    BoardMessagesSqliteReader::SessionInfo emptyInfo;
    emptyInfo.id = -1;
    return emptyInfo;
}

int SessionsListModel::getSessionId(int index) const
{
    if (index >= 0 && index < m_sessions.size()) 
    {
        Session* session = m_sessions.at(index);
        if (session)
        {
            return session->getId();
        }
    }
    return -1;
}

void SessionsListModel::addRecordedSession(const BoardMessagesSqliteReader::SessionInfo &sessionInfo)
{
    // Проверяем, не существует ли уже сессия с таким ID
    for (int i = 0; i < m_sessions.size(); ++i)
    {
        Session* session = m_sessions.at(i);
        if (session && session->getId() == sessionInfo.id)
        {
            qDebug() << "SessionsListModel: Session with ID" << sessionInfo.id << "already exists";
            return;
        }
    }
    
    // Создаем новую записанную сессию через фабрику
    Session* newSession = m_recordedSessionsFactory->createSession(sessionInfo);
    if (newSession)
    {
        addSessionToList(newSession);
        qDebug() << "SessionsListModel: Added new recorded session" << sessionInfo.id << "with name" << sessionInfo.name;
    }
}

void SessionsListModel::updateSessionMessageCount(int sessionId, int messageCount)
{
    int index = findSessionIndex(sessionId);
    if (index >= 0 && index < m_sessions.size())
    {
        Session* session = m_sessions.at(index);
        if (session && session->getType() == Session::RecordedSession)
        {
            RecordedSession* recordedSession = qobject_cast<RecordedSession*>(session);
            if (recordedSession)
            {
                recordedSession->updateMessageCount(messageCount);
                qDebug() << "SessionsListModel: Updated message count for session" << sessionId << "to" << messageCount;
            }
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
    
    Session* session = m_sessions.at(index);
    if (!session)
    {
        qWarning() << "SessionsListModel: Session at index" << index << "is null";
        return;
    }
    
    // Получаем ID сессии для удаления из базы данных (только для записанных сессий)
    int sessionId = session->getId();
    
    // Удаляем сессию из списка
    beginRemoveRows(QModelIndex(), index, index);
    m_sessions.removeAt(index);
    endRemoveRows();
    
    qDebug() << "SessionsListModel: Removed session at index" << index << "with ID" << sessionId;
    
    // Удаляем сессию из базы данных только для записанных сессий
    if (session->getType() == Session::RecordedSession && m_reader && sessionId > 0)
    {
        if (!m_reader->removeSession(sessionId))
        {
            qWarning() << "SessionsListModel: Failed to remove session" << sessionId << "from database";
            emit errorOccurred(QString("Не удалось удалить сессию %1 из базы данных").arg(sessionId));
        }
    }
    
    // Удаляем объект сессии
    session->deleteLater();
}

int SessionsListModel::findSessionIndex(int sessionId) const
{
    for (int i = 0; i < m_sessions.size(); ++i)
    {
        Session* session = m_sessions.at(i);
        if (session && session->getId() == sessionId)
        {
            return i;
        }
    }
    return -1;
}

void SessionsListModel::initializeLiveSession()
{
    if (!m_liveSession)
    {
        m_liveSession = m_liveSessionFactory->createLiveSession();
        if (m_liveSession)
        {
            // Подключаем сигналы живой сессии
            connect(m_liveSession, &Session::sessionChanged,
                    this, &SessionsListModel::onSessionChanged);
            connect(m_liveSession, &Session::messageCountChanged,
                    this, &SessionsListModel::onMessageCountChanged);
            connect(m_liveSession, &Session::parameterCountChanged,
                    this, &SessionsListModel::onParameterCountChanged);
            
            // Устанавливаем живую сессию как активную по умолчанию
            m_currentActiveSession = m_liveSession;
            
            qDebug() << "SessionsListModel: Live session initialized and set as active";
        }
    }
}

void SessionsListModel::updateLiveSessionCounters()
{
    if (m_liveSession)
    {
        // Обновляем счетчик сообщений (каждое сообщение от драйвера)
        m_liveSessionFactory->incrementMessageCount();
        
        // Счетчик параметров будет обновляться автоматически через сигналы
        // когда параметры добавляются в хранилище
    }
}

void SessionsListModel::resetLiveSessionCounters()
{
    if (m_liveSession)
    {
        m_liveSessionFactory->resetCounters();
        
        // Обновляем отображение живой сессии в интерфейсе
        updateSessionInList(m_liveSession);
        
        qDebug() << "SessionsListModel: Reset live session counters";
    }
}

void SessionsListModel::onRecordedSessionsCreated(const QList<Session*>& sessions)
{
    // Этот метод вызывается фабрикой при создании записанных сессий
    // Сессии уже добавлены в список в методе refreshSessions
    qDebug() << "SessionsListModel: Received" << sessions.size() << "recorded sessions from factory";
}

void SessionsListModel::onLiveSessionCreated(Session* session)
{
    // Этот метод вызывается фабрикой при создании живой сессии
    qDebug() << "SessionsListModel: Live session created by factory";
}

void SessionsListModel::onSessionChanged()
{
    // Находим индекс изменившейся сессии и обновляем модель
    Session* changedSession = qobject_cast<Session*>(sender());
    if (changedSession)
    {
        updateSessionInList(changedSession);
    }
}

void SessionsListModel::onMessageCountChanged(int count)
{
    // Находим индекс сессии с изменившимся счетчиком сообщений
    Session* changedSession = qobject_cast<Session*>(sender());
    if (changedSession)
    {
        updateSessionInList(changedSession);
    }
}

void SessionsListModel::onParameterCountChanged(int count)
{
    // Находим индекс сессии с изменившимся счетчиком параметров
    Session* changedSession = qobject_cast<Session*>(sender());
    if (changedSession)
    {
        updateSessionInList(changedSession);
    }
}

void SessionsListModel::onNewParameterAdded(BoardParameterSingle* parameter)
{
    Q_UNUSED(parameter)
    
    // Обновляем счетчик параметров живой сессии
    if (m_liveSession)
    {
        m_liveSessionFactory->incrementParameterCount();
    }
}

void SessionsListModel::sortSessions()
{
    // Сортируем сессии: LiveSession первыми, затем RecordedSession по убыванию даты
    std::sort(m_sessions.begin(), m_sessions.end(),
              [](Session* a, Session* b)
              {
                  if (!a || !b) return false;
                  return *a < *b;
              });
}

void SessionsListModel::addSessionToList(Session* session)
{
    if (!session) return;
    
    // Добавляем сессию в список
    beginInsertRows(QModelIndex(), m_sessions.size(), m_sessions.size());
    m_sessions.append(session);
    endInsertRows();
    
    // Подключаем сигналы сессии
    connect(session, &Session::sessionChanged,
            this, &SessionsListModel::onSessionChanged);
    connect(session, &Session::messageCountChanged,
            this, &SessionsListModel::onMessageCountChanged);
    connect(session, &Session::parameterCountChanged,
            this, &SessionsListModel::onParameterCountChanged);
    
    // Пересортируем список после добавления новой сессии
    // Используем beginResetModel/endResetModel для корректного обновления интерфейса
    beginResetModel();
    sortSessions();
    endResetModel();
}

void SessionsListModel::removeSessionFromList(Session* session)
{
    if (!session) return;
    
    int index = m_sessions.indexOf(session);
    if (index >= 0)
    {
        beginRemoveRows(QModelIndex(), index, index);
        m_sessions.removeAt(index);
        endRemoveRows();
    }
}

void SessionsListModel::updateSessionInList(Session* session)
{
    if (!session) return;
    
    int index = m_sessions.indexOf(session);
    if (index >= 0)
    {
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
}

void SessionsListModel::switchToSession(int sessionIndex)
{
    if (sessionIndex < 0 || sessionIndex >= m_sessions.size())
    {
        qWarning() << "SessionsListModel: Invalid session index" << sessionIndex;
        return;
    }
    
    Session* session = m_sessions.at(sessionIndex);
    if (!session)
    {
        qWarning() << "SessionsListModel: Session at index" << sessionIndex << "is null";
        return;
    }
    
    // Очищаем хранилища всех других RecordedSession
    for (int i = 0; i < m_sessions.size(); ++i)
    {
        if (i != sessionIndex)
        {
            Session* otherSession = m_sessions.at(i);
            if (otherSession && otherSession->getType() == Session::RecordedSession)
            {
                otherSession->clearStorage();
            }
        }
    }
    
    // Устанавливаем текущую активную сессию
    m_currentActiveSession = session;
    
    qDebug() << "SessionsListModel: Switched to session" << session->getName() << "at index" << sessionIndex;
    
    // Эмитируем сигнал о смене сессии
    emit sessionSwitched(session);
}

void SessionsListModel::switchToLiveSession()
{
    if (!m_liveSession)
    {
        qWarning() << "SessionsListModel: Live session is not available";
        return;
    }
    
    // Очищаем хранилища всех RecordedSession
    for (Session* session : m_sessions)
    {
        if (session && session->getType() == Session::RecordedSession)
        {
            session->clearStorage();
        }
    }
    
    // Устанавливаем текущую активную сессию
    m_currentActiveSession = m_liveSession;
    
    qDebug() << "SessionsListModel: Switched to live session";
    
    // Эмитируем сигнал о смене сессии
    emit sessionSwitched(m_liveSession);
}

void SessionsListModel::selectSession(int sessionIndex)
{
    if (sessionIndex < 0 || sessionIndex >= m_sessions.size())
    {
        qWarning() << "SessionsListModel: Invalid session index" << sessionIndex;
        return;
    }

    Session* session = m_sessions.at(sessionIndex);
    if (!session)
    {
        qWarning() << "SessionsListModel: Session at index" << sessionIndex << "is null";
        return;
    }

    m_selectedIndex = sessionIndex;

    // Если это RecordedSession, загружаем данные из базы при первом выборе
    if (session->getType() == Session::RecordedSession)
    {
        RecordedSession* recordedSession = qobject_cast<RecordedSession*>(session);
        if (recordedSession && m_reader)
        {
            // Проверяем, загружены ли уже данные в хранилище
            if (!recordedSession->isDataLoaded())
            {
                qDebug() << "SessionsListModel: Loading data for recorded session" << recordedSession->getId();
                recordedSession->loadDataFromDatabase(m_reader);
            }
            else
            {
                qDebug() << "SessionsListModel: Data already loaded for session" << recordedSession->getId();
            }
            
            // Инициализируем плеер для установки временного диапазона
            SessionPlayer* sessionPlayer = qobject_cast<SessionPlayer*>(recordedSession->player());
            if (sessionPlayer)
            {
                qDebug() << "SessionsListModel: Initializing SessionPlayer for recorded session" << recordedSession->getId();
                
                // Если данные уже загружены, инициализируем плеер принудительно
                if (recordedSession->isDataLoaded())
                {
                    sessionPlayer->initializeWithLoadedData();
                }
                // Иначе плеер автоматически инициализируется при загрузке данных через сигнал sessionDataLoaded
            }
        }
    }
    //else if (session->getType() == Session::LiveSession)
    //{
    //    qDebug() << "SessionsListModel: Selected live session - no data loading needed";
    //    
    //    // Для живой сессии инициализируем DriverDataPlayer
    //    LiveSession* liveSession = qobject_cast<LiveSession*>(session);
    //    if (liveSession)
    //    {
    //        DriverDataPlayer* driverPlayer = qobject_cast<DriverDataPlayer*>(liveSession->player());
    //        if (driverPlayer)
    //        {
    //            qDebug() << "SessionsListModel: Initializing DriverDataPlayer for live session";
    //            // DriverDataPlayer инициализируется автоматически при получении первого параметра
    //            // Но можем сбросить состояние для чистой инициализации
    //            driverPlayer->resetState();
    //        }
    //    }
    //}

    qInfo() << "SessionsListModel: Selected session at index" << m_selectedIndex << ":" << session->getName();
}

