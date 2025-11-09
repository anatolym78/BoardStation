#include "Session.h"
#include "Model/Parameters/Tree/ParameterTreeStorage.h"
#include "Model/Parameters/Tree/ParameterTreeItem.h"
#include "Model/Parameters/Tree/ParameterTreeHistoryItem.h"
#include <QDebug>
#include <QMessageBox>

Session::Session(QObject *parent)
	: QObject(parent)
	, m_treeStorage(new ParameterTreeStorage(this))
	, m_parametersModel(new BoardParametersTreeModel(parent))
	, m_chartsModel(new ChatViewGridModel(parent))
	, m_player(nullptr)
	, m_opened(false)
{
	m_parametersSelectionModel = new QItemSelectionModel(m_parametersModel, this);
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

void Session::createChartFromSelectedParameter()
{
	if (!m_parametersSelectionModel || !m_chartsModel)
	{
		qWarning() << "Session::createChartFromSelectedParameter: selectionModel or chartsModel is null";
		return;
	}

	// Получаем текущий выбранный индекс
	QModelIndex currentIndex = m_parametersSelectionModel->currentIndex();
	if (!currentIndex.isValid())
	{
		qWarning() << "Session::createChartFromSelectedParameter: no parameter selected";
		return;
	}

	// Получаем элемент дерева из индекса
	auto treeItem = static_cast<ParameterTreeItem*>(currentIndex.internalPointer());
	if (!treeItem)
	{
		qWarning() << "Session::createChartFromSelectedParameter: treeItem is null";
		return;
	}

	//// Проверяем, что это History item (листовой элемент, для которого можно создать график)
	//if (treeItem->type() != ParameterTreeItem::ItemType::History)
	//{
	//	qWarning() << "Session::createChartFromSelectedParameter: selected item is not a History item";
	//	return;
	//}

	// Получаем label параметра
	QString parameterLabel = treeItem->fullName();
	if (parameterLabel.isEmpty())
	{

		qWarning() << "Session::createChartFromSelectedParameter: parameter label is empty";
		return;
	}

	//QMessageBox::information(nullptr, "BoardStation", treeItem->fullName());

	// Добавляем серию в модель графиков
	m_chartsModel->addChart(treeItem);
	qDebug() << "Session::createChartFromSelectedParameter: added chart for parameter" << parameterLabel;
}
