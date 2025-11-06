#include "ParameterTreeStorage.h"
#include "ParameterTreeHistoryItem.h"

ParameterTreeStorage::ParameterTreeStorage(QObject *parent)
	: ParameterTreeItem("root", nullptr)
{
	setParent(parent);
}

QList<ParameterTreeItem*> ParameterTreeStorage::findPath(ParameterTreeHistoryItem* item) const
{
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

int ParameterTreeStorage::topLevelItemIndex(ParameterTreeItem* item) const
{
	for (auto i = 0; i < m_childItems.count(); i++)
	{
		if (m_childItems[i] == item) return i;
	}

	return -1;
}

void ParameterTreeStorage::appendSnapshot(ParameterTreeStorage* snapshot)
{
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
	}
}
