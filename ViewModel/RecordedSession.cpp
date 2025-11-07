#include "RecordedSession.h"
#include "Model/Parameters/BoardParameterHistoryStorage.h"
#include "Model/Parameters/Tree/ParameterTreeStorage.h"
#include <QDebug>
#include "SessionPlayer.h"

RecordedSession::RecordedSession(const BoardMessagesSqliteReader::SessionInfo& sessionInfo, QObject *parent)
	: Session(parent)
	, m_sessionInfo(sessionInfo)
{
	//m_storage = new BoardParameterHistoryStorage(this);

	m_player = new SessionPlayer(this);
	m_player->setStorage(m_treeStorage);

	connect(m_player, &SessionPlayer::played, m_parametersModel, &BoardParametersTreeModel::setSnapshot);

	m_chartsModel->setPlayer(m_player);
	//m_chartsModel->setStorage(m_storage);
}

void RecordedSession::open()
{
	if (!m_opened)
	{
		player()->initialPlay();
	}

	Session::open();
}

void RecordedSession::updateSessionInfo(const BoardMessagesSqliteReader::SessionInfo& sessionInfo)
{
	bool changed = false;
	
	if (m_sessionInfo.messageCount != sessionInfo.messageCount)
	{
		m_sessionInfo.messageCount = sessionInfo.messageCount;
		changed = true;
	}
	
	if (m_sessionInfo.parameterCount != sessionInfo.parameterCount)
	{
		m_sessionInfo.parameterCount = sessionInfo.parameterCount;
		changed = true;
	}
	
	if (m_sessionInfo.name != sessionInfo.name)
	{
		m_sessionInfo.name = sessionInfo.name;
		changed = true;
	}
	
	if (m_sessionInfo.description != sessionInfo.description)
	{
		m_sessionInfo.description = sessionInfo.description;
		changed = true;
	}
	
	if (changed)
	{
		emit sessionChanged();
	}
}

void RecordedSession::updateMessageCount(int count)
{
	if (m_sessionInfo.messageCount != count)
	{
		m_sessionInfo.messageCount = count;
		emit messageCountChanged(count);
		emit sessionChanged();
	}
}

void RecordedSession::updateParameterCount(int count)
{
	if (m_sessionInfo.parameterCount != count)
	{
		m_sessionInfo.parameterCount = count;
		emit parameterCountChanged(count);
		emit sessionChanged();
	}
}

void RecordedSession::clearStorage()
{
	//if (m_storage)
	//{
	//	m_storage->clear();
	//	qDebug() << "RecordedSession: Storage cleared for session" << m_sessionInfo.id;
	//}
	//if (m_treeStorage)
	//{
	//	delete m_treeStorage;
	//	m_treeStorage = new ParameterTreeStorage(this);
	//}
}

void RecordedSession::loadDataFromDatabase(BoardMessagesSqliteReader* reader)
{
	//if (!reader || !m_storage)
	//{
	//	qWarning() << "RecordedSession: Reader or storage is not available";
	//	return;
	//}
	//
	//qDebug() << "RecordedSession: Loading data from database for session" << m_sessionInfo.id;
	//
	//// Загружаем данные сессии из базы в хранилище
	//m_storage->loadSessionData(m_sessionInfo.id, reader);
	//
	//qDebug() << "RecordedSession: Data loaded for session" << m_sessionInfo.id;
}

bool RecordedSession::isDataLoaded() const
{
	return false;
	//if (!m_storage)
	//{
	//	return false;
	//}
	//
	//return !m_storage->getSessionParameters().isEmpty();
}
