#include "BoardParametersListModel.h"

#include <random>

BoardParametersListModel::BoardParametersListModel(BoardParameterHistoryStorage* storage, QObject *parent)
	: QAbstractListModel{parent},
	m_pParametersStorage(nullptr)
{
	setParametersStorage(storage);

    int hue = 0;
    int hueStep = 60 + 5;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib_int(192, 255);
    for(auto i=0;i<200;i++)
    {
		auto sat = distrib_int(gen);
		auto val = distrib_int(gen);
        m_colors.append(QColor::fromHsv(hue, sat, val, 255));

        hue += hueStep;

        if(hue > 360)
        {
            hue -= 360;
        }
    }
}

void BoardParametersListModel::setParametersStorage(BoardParameterHistoryStorage* boardParametersStorage)
{
	m_pParametersStorage = boardParametersStorage;


	connect(m_pParametersStorage, &BoardParameterHistoryStorage::newParameterAdded,
		this, &BoardParametersListModel::onNewParameterAdded);
}

int BoardParametersListModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_parameterLabels.size();
}

QVariant BoardParametersListModel::data(const QModelIndex& index, int role) const
{
	const QString& label = m_parameterLabels.at(index.row());
	BoardParameterHistory* parameterHistory = m_pParametersStorage->getParameterHistory(label);

	if (parameterHistory->valueCount() == 0) return {};

	switch (static_cast<ParameterRole>(role))
	{
	case ParameterRole::LabelRole:
	{
		return parameterHistory->label();
	}
	case ParameterRole::ValueRole:
	{
		auto value = parameterHistory->lastValueData().toDouble();

		auto stringDoubleValue = QString::number(value, 'g', 3);

		return QVariant::fromValue(stringDoubleValue);
	}
	case ParameterRole::UntiRole:
		return QVariant::fromValue(parameterHistory->unit());
	case ParameterRole::TimeRole:
		return QVariant::fromValue(parameterHistory->lastTimestamp());
    case ParameterRole::ChartVisibilityRole:
        return QVariant::fromValue(m_chartVisibilities[index.row()]);
    case ParameterRole::ColorRole:
        return QVariant::fromValue(m_colors[index.row()]);	}

	return {};
}

QHash<int, QByteArray> BoardParametersListModel::roleNames() const
{
	QHash<int, QByteArray> rolesHash;
	rolesHash[(int)ParameterRole::LabelRole] = "label";
	rolesHash[(int)ParameterRole::ValueRole] = "value";
	rolesHash[(int)ParameterRole::UntiRole] = "unit";
	rolesHash[(int)ParameterRole::TimeRole] = "timestamp";
    rolesHash[(int)ParameterRole::ChartVisibilityRole] = "chartVisibility";
    rolesHash[(int)ParameterRole::ColorRole] = "parameterColor";

	return rolesHash;
}

int BoardParametersListModel::getCountParameters() const
{
	return rowCount(QModelIndex());
}

void BoardParametersListModel::clearParameters()
{
    onParametersCleared();
}

void BoardParametersListModel::onNewParameterAdded(BoardParameterSingle* parameter)
{
	if (parameter == nullptr) return;

	auto label = parameter->label();
	if (m_parameterLabels.contains(label))
	{
		onParameterUpdated(label);
	}
	else
	{
		onParameterAdded(label);
	}
}

void BoardParametersListModel::onParameterAdded(const QString& label)
{
    //
	beginInsertRows(QModelIndex(), m_parameterLabels.size(), m_parameterLabels.size());
	m_parameterLabels.append(label);
    m_chartVisibilities.append(false);
	endInsertRows();
}

void BoardParametersListModel::onParameterUpdated(const QString& label)
{
	int row = m_parameterLabels.indexOf(label);
	if (row < 0) return;

	auto topLeft = this->index(row, 0);
	auto bottomRight = this->index(row, 0);

	emit dataChanged(topLeft, bottomRight);
}

void BoardParametersListModel::onParametersCleared()
{
    //
	beginResetModel();
	m_parameterLabels.clear();
    m_chartVisibilities.clear();
    //m_colors.clear();
	endResetModel();
}

bool BoardParametersListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()) return false;

    if(role == (int)ParameterRole::ChartVisibilityRole)
    {
        m_chartVisibilities[index.row()] = value.toBool();
    }

    return false;
}
