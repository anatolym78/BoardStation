#include "Session.h"
#include "Model/Parameters/Tree/ParameterTreeStorage.h"

Session::Session(QObject *parent)
	: QObject(parent)
	, m_treeStorage(new ParameterTreeStorage(this))
	, m_parametersModel(new BoardParametersTreeModel(parent))
	, m_chartsModel(new ChatViewGridModel(parent))
	, m_player(nullptr)
	, m_opened(false)
{
}

bool Session::operator<(const Session& other) const
{
	return getCreatedAt() < other.getCreatedAt();
}

bool Session::operator==(const Session& other) const
{
	return getId() == other.getId() && getType() == other.getType();
}

BoardParametersTreeModel* Session::parametersModel() const
{
	return m_parametersModel;
}

void Session::updateMessageCount(int count)
{
	emit messageCountChanged(count);
}

void Session::updateParameterCount(int count)
{
	emit parameterCountChanged(count);
}

ParameterTreeStorage* Session::storage() const
{
	return m_treeStorage;
}
