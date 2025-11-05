#include "ParameterTreeStorage.h"
#include "ParameterTreeGroupItem.h"
#include "ParameterTreeHistoryItem.h"

ParameterTreeStorage::ParameterTreeStorage(QObject *parent)
    : ParameterTreeItem("root", nullptr)
{
    setParent(parent);
}

void ParameterTreeStorage::merge(ParameterTreeStorage* snapshot)
{
    if (!snapshot)
    {
        return;
    }

    for (ParameterTreeItem* incomingChild : snapshot->children())
    {
        mergeNode(this, incomingChild);
    }

    // Снимок - это временный объект, который должен быть удален
    snapshot->deleteLater();
}

void ParameterTreeStorage::mergeNode(ParameterTreeItem* localParent, ParameterTreeItem* incomingNode)
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
                mergeNode(newGroupItem, incomingChild);
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
            for(int i = 0; i < values.size(); ++i) {
                existingHistory->addValue(values[i], timestamps[i]);
                hasNewValues = true;
            }

            if (hasNewValues) {
                 emit valueAdded(existingHistory);
            }
        }
        else if (existingNode->type() == ItemType::Group && incomingNode->type() == ItemType::Group)
        {
            for(ParameterTreeItem* incomingChild : incomingNode->children())
            {
                mergeNode(existingNode, incomingChild);
            }
        }
    }
}
