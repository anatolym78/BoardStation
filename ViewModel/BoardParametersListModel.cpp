#include "BoardParametersListModel.h"

#include <random>

BoardParametersListModel::BoardParametersListModel(QObject* parent /*= nullptr*/)
	: QAbstractListModel{ parent }
{
	makeRandomColors();
}

int BoardParametersListModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_values.size();
}

QVariant BoardParametersListModel::data(const QModelIndex& index, int role) const
{
	auto keys = m_values.keys();
	const QString& label = keys.at(index.row());
	auto parameter = m_values[label];

	switch (static_cast<ParameterRole>(role))
	{
	case ParameterRole::LabelRole:
	{
		return label;
	}
	case ParameterRole::ValueRole:
	{
		bool ok;
		auto doubleValue = parameter->value().toDouble(&ok);

		if (!ok) return {};


		return QVariant::fromValue(QString::number(doubleValue, 'g', 3));
	}
	case ParameterRole::UntiRole:
		return QVariant::fromValue(parameter->unit());
	case ParameterRole::TimeRole:
		return QVariant::fromValue(parameter->timestamp());
	case ParameterRole::ChartVisibilityRole:
		return QVariant::fromValue(m_chartVisibilities[index.row()]);
	case ParameterRole::ColorRole:
		return QVariant::fromValue(m_colors[index.row()]);
	}

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

	if(!m_values.contains(label))
	{
		beginInsertRows(QModelIndex(), m_values.size(), m_values.size());
		m_chartVisibilities.append(false);
		m_values.insert(label, parameter);
		endInsertRows();
	}
	else
	{
		m_values[label] = parameter;

		emit dataChanged(index(0, 0), index(m_values.count() - 1, 0));
	}
}


void BoardParametersListModel::onParametersCleared()
{
	beginResetModel();
    m_chartVisibilities.clear();
	m_values.clear();
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

void BoardParametersListModel::makeRandomColors()
{
	int hue = 0;
	int hueStep = 60 + 5;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib_int(192, 255);
	for (auto i = 0; i < 200; i++)
	{
		auto sat = distrib_int(gen);
		auto val = distrib_int(gen);
		m_colors.append(QColor::fromHsv(hue, sat, val, 255));

		hue += hueStep;

		if (hue > 360)
		{
			hue -= 360;
		}
	}
}
