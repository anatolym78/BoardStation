#include "ParameterTreeStorage.h"
#include "ParameterTreeHistoryItem.h"
#include "ParameterTreeGroupItem.h"
#include "ParameterTreeArrayItem.h"
#include <QDebug>
#include <QMutexLocker>

ParameterTreeStorage::ParameterTreeStorage(QObject *parent)
	: ParameterTreeItem("root", nullptr)
{
	setParent(parent);
}

QList<ParameterTreeItem*> ParameterTreeStorage::findPath(ParameterTreeHistoryItem* item) const
{
	QMutexLocker locker(&m_mutex);
	QList<ParameterTreeItem*> path{ item };
	ParameterTreeItem* currentItem = item;
	// перебираем узлы снизу вверх до корневого (хранилище)
	while (currentItem->parentItem() != nullptr)
	{
		currentItem = currentItem->parentItem();

		path.append(currentItem);
	}

	// узел не принадлежит хранилищу
	if (currentItem != this) return QList<ParameterTreeItem*>();

	// интвертируем порядок
	// теперь элементы расположены от верхнего (данное хранилище) к нижнему (входной параметер)
	std::reverse(path.begin(), path.end());

	return path;
}

ParameterTreeStorage* ParameterTreeStorage::extractRange(const QDateTime& startTime, const QDateTime& endTime) const
{
	QMutexLocker locker(&m_mutex);
	auto subStorage = new ParameterTreeStorage();

	for (ParameterTreeItem* child : m_childItems)
	{
		extractNode(subStorage, child, startTime, endTime);
	}

	return subStorage;
}

QList<BoardParameterSingle*> ParameterTreeStorage::getParametersInTimeRange(const QDateTime& startTime, const QDateTime& endTime) const
{
	QMutexLocker locker(&m_mutex);
	QList<BoardParameterSingle*> params;
	collectParameters(const_cast<ParameterTreeStorage*>(this), startTime, endTime, params);
	return params;
}

void ParameterTreeStorage::clear()
{
	QMutexLocker locker(&m_mutex);
	qDeleteAll(m_childItems);
	m_childItems.clear();
}

int ParameterTreeStorage::topLevelItemIndex(ParameterTreeItem* item) const
{
	QMutexLocker locker(&m_mutex);
	for (auto i = 0; i < m_childItems.count(); i++)
	{
		if (m_childItems[i] == item) return i;
	}

	return -1;
}

ParameterTreeHistoryItem* ParameterTreeStorage::findHistoryItemByFullName(const QString& fullName) const
{
	QMutexLocker locker(&m_mutex);
	
	if (fullName.isEmpty())
	{
		return nullptr;
	}
	
	QStringList pathParts = fullName.split('.');
	pathParts.removeAll(QString()); // Удаляем пустые строки
	if (pathParts.isEmpty())
	{
		return nullptr;
	}
	
	ParameterTreeItem* currentItem = const_cast<ParameterTreeStorage*>(this);
	
	// Проходим по пути от корня до конечного узла
	for (int i = 0; i < pathParts.size(); ++i)
	{
		const QString& part = pathParts[i];
		ParameterTreeItem* foundChild = currentItem->findChildByLabel(part);
		
		if (!foundChild)
		{
			return nullptr;
		}
		
		// Если это последняя часть пути, проверяем, что это History узел
		if (i == pathParts.size() - 1)
		{
			if (foundChild->type() == ItemType::History)
			{
				return static_cast<ParameterTreeHistoryItem*>(foundChild);
			}
			return nullptr;
		}
		
		// Продолжаем поиск в следующем уровне
		currentItem = foundChild;
	}
	
	return nullptr;
}

void ParameterTreeStorage::appendSnapshot(ParameterTreeStorage* snapshot)
{
	QMutexLocker locker(&m_mutex);
	if (!snapshot)
	{
		return;
	}

	for (ParameterTreeItem* incomingChild : snapshot->children())
	{
		appendNode(this, incomingChild);
	}

	// Снимок - это временный объект, который должен быть удален
	snapshot->deleteLater();
}

void ParameterTreeStorage::setSnapshot(ParameterTreeStorage* snapshot)
{
	QMutexLocker locker(&m_mutex);
	if (!snapshot)
	{
		return;
	}

	for (ParameterTreeItem* incomingChild : snapshot->children())
	{
		setNode(this, incomingChild);
	}

	// Снимок - это временный объект, который должен быть удален
	snapshot->deleteLater();
}

void ParameterTreeStorage::appendNode(ParameterTreeItem* localParent, ParameterTreeItem* incomingNode)
{
	ParameterTreeItem* existingNode = localParent->findChildByLabel(incomingNode->label());

	if (!existingNode)
	{
		// Узел не существует, создаем новый и копируем данные
		if (incomingNode->type() == ItemType::History)
		{
			auto incomingHistory = static_cast<ParameterTreeHistoryItem*>(incomingNode);
			auto newHistoryItem = new ParameterTreeHistoryItem(incomingHistory->label(), localParent);

			const auto& values = incomingHistory->values();
			const auto& timestamps = incomingHistory->timestamps();
			for(int i = 0; i < values.size(); ++i) {
				newHistoryItem->addValue(values[i], timestamps[i]);
			}

			localParent->appendChild(newHistoryItem);
			emit parameterAdded(newHistoryItem);
			if (!values.isEmpty())
			{
				emit valueAdded(newHistoryItem);
			}
		}
		else if (incomingNode->type() == ItemType::Group)
		{
			auto newGroupItem = new ParameterTreeGroupItem(incomingNode->label(), localParent);
			localParent->appendChild(newGroupItem);
			emit parameterAdded(newGroupItem);

			for(ParameterTreeItem* incomingChild : incomingNode->children())
			{
				appendNode(newGroupItem, incomingChild);
			}
		}
		else if (incomingNode->type() == ItemType::Array)
		{
			auto newArrayItem = new ParameterTreeArrayItem(incomingNode->label(), localParent);
			localParent->appendChild(newArrayItem);
			emit parameterAdded(newArrayItem);

			for(ParameterTreeItem* incomingChild : incomingNode->children())
			{
				appendNode(newArrayItem, incomingChild);
			}
		}
	}
	else
	{
		// Узел существует
		if (existingNode->type() == ItemType::History && incomingNode->type() == ItemType::History)
		{
			auto existingHistory = static_cast<ParameterTreeHistoryItem*>(existingNode);
			auto incomingHistory = static_cast<ParameterTreeHistoryItem*>(incomingNode);

			const auto& values = incomingHistory->values();
			const auto& timestamps = incomingHistory->timestamps();
			bool hasNewValues = false;
			for(int i = 0; i < values.size(); ++i)
			{
				existingHistory->addValue(values[i], timestamps[i]);
				hasNewValues = true;
			}

			if (hasNewValues) 
			{
				 emit valueAdded(existingHistory);
			}
		}
		else
		{
			if (existingNode->type() == ItemType::Group && incomingNode->type() == ItemType::Group)
			{
				for (ParameterTreeItem* incomingChild : incomingNode->children())
				{
					appendNode(existingNode, incomingChild);
				}
			}
			else if (existingNode->type() == ItemType::Array && incomingNode->type() == ItemType::Array)
			{
				for (ParameterTreeItem* incomingChild : incomingNode->children())
				{
					appendNode(existingNode, incomingChild);
				}
			}
		}
	}
}

void ParameterTreeStorage::setNode(ParameterTreeItem* localParent, ParameterTreeItem* incomingNode)
{
	ParameterTreeItem* existingNode = localParent->findChildByLabel(incomingNode->label());

	if (!existingNode)
	{
		// Узел не существует, создаем новый и копируем данные
		if (incomingNode->type() == ItemType::History)
		{
			auto incomingHistory = static_cast<ParameterTreeHistoryItem*>(incomingNode);
			auto newHistoryItem = new ParameterTreeHistoryItem(incomingHistory->label(), localParent);

			newHistoryItem->setValues(incomingHistory->values(), incomingHistory->timestamps());

			localParent->appendChild(newHistoryItem);
			emit parameterAdded(newHistoryItem);
			if (!incomingHistory->values().isEmpty())
			{
				emit valueAdded(newHistoryItem);
			}
		}
		else if (incomingNode->type() == ItemType::Group)
		{
			auto newGroupItem = new ParameterTreeGroupItem(incomingNode->label(), localParent);
			localParent->appendChild(newGroupItem);
			emit parameterAdded(newGroupItem);

			for(ParameterTreeItem* incomingChild : incomingNode->children())
			{
				setNode(newGroupItem, incomingChild);
			}
		}
		else if (incomingNode->type() == ItemType::Array)
		{
			auto newArrayItem = new ParameterTreeArrayItem(incomingNode->label(), localParent);
			localParent->appendChild(newArrayItem);
			emit parameterAdded(newArrayItem);

			for(ParameterTreeItem* incomingChild : incomingNode->children())
			{
				setNode(newArrayItem, incomingChild);
			}
		}
	}
	else
	{
		// Узел существует
		if (existingNode->type() == ItemType::History && incomingNode->type() == ItemType::History)
		{
			auto existingHistory = static_cast<ParameterTreeHistoryItem*>(existingNode);
			auto incomingHistory = static_cast<ParameterTreeHistoryItem*>(incomingNode);

			existingHistory->setValues(incomingHistory->values(), incomingHistory->timestamps());
			emit valueChanged(existingHistory);
		}
		else if (existingNode->type() == ItemType::Group && incomingNode->type() == ItemType::Group)
		{
			for(ParameterTreeItem* incomingChild : incomingNode->children())
			{
				setNode(existingNode, incomingChild);
			}
		}
		else if (existingNode->type() == ItemType::Array && incomingNode->type() == ItemType::Array)
		{
			for(ParameterTreeItem* incomingChild : incomingNode->children())
			{
				setNode(existingNode, incomingChild);
			}
		}
	}
}

void ParameterTreeStorage::extractNode(ParameterTreeItem* localParent, ParameterTreeItem* incomingNode, const QDateTime& startTime, const QDateTime& endTime) const
{
	if (incomingNode->type() == ItemType::History)
	{
		auto incomingHistory = static_cast<ParameterTreeHistoryItem*>(incomingNode);
		auto newHistoryItem = new ParameterTreeHistoryItem(incomingHistory->label(), localParent);

		const auto& values = incomingHistory->values();
		const auto& timestamps = incomingHistory->timestamps();
		bool hasValuesInRange = false;
		for (int i = 0; i < values.size(); ++i)
		{
			if (timestamps[i] >= startTime && timestamps[i] <= endTime)
			{
				newHistoryItem->addValue(values[i], timestamps[i]);
				hasValuesInRange = true;
			}
		}

		if (hasValuesInRange)
		{
			localParent->appendChild(newHistoryItem);
		}
		else
		{
			delete newHistoryItem;
		}
	}
	else if (incomingNode->type() == ItemType::Group)
	{
		auto newGroupItem = new ParameterTreeGroupItem(incomingNode->label(), localParent);

		for (ParameterTreeItem* incomingChild : incomingNode->children())
		{
			extractNode(newGroupItem, incomingChild, startTime, endTime);
		}

		if (newGroupItem->childCount() > 0)
		{
			localParent->appendChild(newGroupItem);
		}
		else
		{
			delete newGroupItem;
		}
	}
	else if (incomingNode->type() == ItemType::Array)
	{
		auto newArrayItem = new ParameterTreeArrayItem(incomingNode->label(), localParent);

		for (ParameterTreeItem* incomingChild : incomingNode->children())
		{
			extractNode(newArrayItem, incomingChild, startTime, endTime);
		}

		if (newArrayItem->childCount() > 0)
		{
			localParent->appendChild(newArrayItem);
		}
		else
		{
			delete newArrayItem;
		}
	}
}

void ParameterTreeStorage::collectParameters(ParameterTreeItem* item, const QDateTime& startTime, const QDateTime& endTime, QList<BoardParameterSingle*>& params) const
{
	if (item->type() == ItemType::History)
	{
		auto historyItem = static_cast<ParameterTreeHistoryItem*>(item);
		const auto& timestamps = historyItem->timestamps();
		const auto& values = historyItem->values();
		const QString& label = historyItem->fullName();

		for (int i = 0; i < timestamps.size(); ++i)
		{
			if (timestamps[i] >= startTime && timestamps[i] <= endTime)
			{
				params.append(new BoardParameterSingle(label, values[i], timestamps[i]));
			}
		}
	}

	for (ParameterTreeItem* child : item->children())
	{
		collectParameters(child, startTime, endTime, params);
	}
}
