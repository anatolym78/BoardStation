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
	if (m_treeStorage)
	{
		m_treeStorage->clear();
		qDebug() << "RecordedSession: Tree storage cleared for session" << m_sessionInfo.id;
	}
}

void RecordedSession::loadDataFromDatabase(BoardMessagesSqliteReader* reader)
{
	if (!reader || !m_treeStorage || !m_parametersModel)
	{
		qWarning() << "RecordedSession: Reader, tree storage or model is not available";
		return;
	}

	qDebug() << "RecordedSession: Loading tree data from database for session" << m_sessionInfo.id;

	if (!reader->loadSessionToTree(m_sessionInfo.id, m_treeStorage))
	{
		qWarning() << "RecordedSession: Failed to load session into tree";
		return;
	}

	// Обновляем модель представления 
	// Это ошибка, и мы ее закомментируем, заменив на вызов played со стартовым интервалом
	//m_parametersModel->setSnapshot(m_treeStorage, /*isBackPlaying*/ false);
	player()->initialPlay();

	// Инициализируем временные границы плеера по загруженным данным,
	// чтобы PlayerView отобразил длительность и текущую позицию
	if (m_player)
	{
		static_cast<SessionPlayer*>(m_player)->initializeWithLoadedData();
	}

	m_dataLoaded = true;

	qDebug() << "RecordedSession: Tree data loaded for session" << m_sessionInfo.id;
}

bool RecordedSession::isDataLoaded() const
{
	 //return m_dataLoaded;

	if (!m_treeStorage)
	{
		return false;
	}
	return m_treeStorage->childCount() > 0;
}
