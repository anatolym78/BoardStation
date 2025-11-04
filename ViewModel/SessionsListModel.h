#ifndef SESSIONSLISTMODEL_H
#define SESSIONSLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QDateTime>
#include <QList>
#include "Session.h"
#include "RecordedSessionsFactory.h"
#include "LiveSessionFactory.h"
#include "Model/Parameters/BoardMessagesSqliteReader.h"
#include "Model/Parameters/BoardParameterSingle.h"
#include "RecordedSession.h"
#include "SessionPlayer.h"
#include "DriverDataPlayer.h"
#include "LiveSession.h"

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
		SessionTypeRole,
		IsLiveSessionRole,
		ParametersModelRole,
		PlayerModelRole,
		ChartsModelRole,
		TestColorRole,
	};

	explicit SessionsListModel(QObject *parent = nullptr);
	~SessionsListModel();

	Q_INVOKABLE bool isRecordingState() const { return m_recording;}
	Q_INVOKABLE void startRecording ();
	Q_INVOKABLE void stopRecording();
	Q_INVOKABLE void setRecordingState(bool enable);
	
	// QAbstractListModel interface
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

	Q_INVOKABLE QVariantMap get(int index) const;

	// Методы для работы с данными
	void setReader(BoardMessagesSqliteReader *reader);
	Q_INVOKABLE void refreshSessions();
	Q_INVOKABLE void addRecordedSession(const BoardMessagesSqliteReader::SessionInfo &sessionInfo);
	Q_INVOKABLE void updateSessionMessageCount(int sessionId, int messageCount);
	Q_INVOKABLE void removeSession(int index);
	
	// Получение информации о сессии
	Session* getSession(int index) const;
	Session* getSessionById(int sessionId) const;
	LiveSession* liveSession() const;
	BoardMessagesSqliteReader::SessionInfo getSessionInfo(int index) const;
	int getSessionId(int index) const;
	int findSessionIndex(int sessionId) const;

	Q_INVOKABLE void selectSession(int sessionIndex);
	
	// Методы для работы с живой сессией
	bool createLiveSession();
	void updateLiveSessionCounters();
	void resetLiveSessionCounters();
	
	// Методы для переключения между сессиями
	Q_INVOKABLE void switchToSession(int sessionIndex);
	Q_INVOKABLE void switchToLiveSession();

	Q_INVOKABLE void resetLiveSession();
	
	// Получение текущей активной сессии
	Session* getCurrentActiveSession() const { return m_currentActiveSession; }

signals:
	void sessionsRefreshed();
	void errorOccurred(const QString &error);
	void sessionSwitched(Session* session);

private slots:
	void onRecordedSessionsCreated(const QList<Session*>& sessions);
	void onLiveSessionCreated(Session* session);
	void onSessionChanged();
	void onMessageCountChanged(int count);
	void onParameterCountChanged(int count);

public slots:
	void onNewParameterAdded(BoardParameterSingle* parameter);

private:
	void sortSessions();
	void addSessionToList(Session* session);
	void removeSessionFromList(Session* session);
	void updateSessionInList(Session* session);

private:
	BoardMessagesSqliteReader *m_reader;
	bool m_recording = false;
	QList<Session*> m_sessions;

	int m_selectedIndex = 0;
	
	// Фабрики для создания сессий
	RecordedSessionsFactory *m_recordedSessionsFactory;
	LiveSessionFactory *m_liveSessionFactory;
	
	// Указатель на живую сессию
	Session* m_liveSession;
	
	// Указатель на текущую активную сессию
	Session* m_currentActiveSession;
};

#endif // SESSIONSLISTMODEL_H
