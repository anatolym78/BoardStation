#include "boardparameterslistmodel.h"

BoardParametersListModel::BoardParametersListModel(BoardParameterHistoryStorage* storage, QObject *parent)
    : QAbstractListModel{parent},
	m_pParametersStorage(nullptr)
{
	setParametersStorage(storage);
}

void BoardParametersListModel::setParametersStorage(BoardParameterHistoryStorage* boardParametersStorage)
{
    m_pParametersStorage = boardParametersStorage;
}

int BoardParametersListModel::rowCount(const QModelIndex& parent) const
{
    return m_pParametersStorage->parameterHistoryCount();
}

QVariant BoardParametersListModel::data(const QModelIndex& index, int role) const
{
	//const QString& label = m_parameterLabels.at(index.row());
	auto parametersList = m_pParametersStorage->getAllParameterHistories();
	auto rowIndex = index.row();

	if (parametersList.count() <= rowIndex) return {};

	auto paraterHistory = parametersList[rowIndex];

	if (paraterHistory->valueCount() == 0) return {};

	switch (static_cast<ParameterRole>(role))
	{
	case ParameterRole::LabelRole:
		return QVariant::fromValue(paraterHistory->lastValue());
	case ParameterRole::ValueRole:
		return QVariant::fromValue(paraterHistory->lastTimestamp());
	}

	return {};
}

QHash<int, QByteArray> BoardParametersListModel::roleNames() const
{
	QHash<int, QByteArray> rolesHash;
	rolesHash[(int)ParameterRole::LabelRole] = "label";
	rolesHash[(int)ParameterRole::ValueRole] = "value";

    return rolesHash;
}
