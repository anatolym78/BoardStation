#include "Session.h"
#include "Model/Parameters/BoardParameterHistoryStorage.h"

Session::Session(QObject *parent)
    : QObject(parent)
	, m_treeStorage(new ParameterTreeStorage(this))
	, m_chartsModel(new ChatViewGridModel(parent))
    , m_player(nullptr)
    , m_storage(nullptr)
    , m_opened(false)
{
    m_parametersModel = new BoardParametersTreeModel(parent);
    m_parametersModel->setStorage(m_treeStorage);
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

BoardParameterHistoryStorage* Session::getStorage() const
{
    return m_storage;
}

ParameterTreeStorage* Session::getTreeStorage() const
{
    return m_treeStorage;
}
