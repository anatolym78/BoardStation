#include "Session.h"
#include <QDebug>

Session::Session(QObject *parent)
	: QObject(parent),
	m_parametersModel(new BoardParametersTreeModel(parent)),
	m_chartsModel(new ChatViewGridModel(parent)),
	m_opened(false),
	m_player(nullptr)
{

}

bool Session::operator<(const Session& other) const
{
	// LiveSession всегда идут первыми
	if (getType() != other.getType())
	{
		return getType() == LiveSession;
	}
	
	// Для сессий одного типа сортируем по дате создания (убывание)
	return getCreatedAt() > other.getCreatedAt();
}

BoardParametersTreeModel* Session::parametersModel() const
{
	return m_parametersModel;
}

bool Session::operator==(const Session& other) const
{
	return getId() == other.getId() && getType() == other.getType();
}

void Session::updateMessageCount(int count)
{
	emit messageCountChanged(count);
}

void Session::updateParameterCount(int count)
{
	emit parameterCountChanged(count);
}
