#include "OutParametersModel.h"
#include <QDebug>
#include <QTableView>
#include "Model/Parameters/ListedRealOutParameter.h"
#include "Model/Parameters/RangedRealOutParameter.h"
#include "Model/Parameters/StringOutParameter.h"
#include "Model/Parameters/BooleanOutParameter.h"

OutParametersModel::OutParametersModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_storage(nullptr)
{
    setupParameters();
}

int OutParametersModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0;

	if (m_storage == nullptr) return 0;
    
    auto countParameters = m_storage->getAllParameters().count();

    return m_storage->getAllParameters().count();

    auto rangedParameters = m_storage->getParametersOfType<RangedRealOutParameter>();

    return rangedParameters.count();
}

int OutParametersModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;

    return 3; // Три колонки: название, значение, управление
}
QHash<int, QByteArray> OutParametersModel::roleNames() const
{
	QHash<int, QByteArray> roles;
    roles[(int)OutParameterRole::LabelRole] = "parameterLabel";
    roles[(int)OutParameterRole::ValueRole] = "parameterValue";
    roles[(int)OutParameterRole::DataControlRole] = "dataControl";
	return roles;
}

QVariantMap OutParametersModel::createParameterData(OutParameter* parameter) const
{
    QVariantMap paramData;

    auto rangedParameter = dynamic_cast<RangedRealOutParameter*>(parameter);

    if(rangedParameter == nullptr) return {};

    paramData["controlType"] = rangedParameter->getControlType();

    QVariantList variantRange;
    variantRange.append(QVariant(rangedParameter->getMinValue()));
    variantRange.append(QVariant(rangedParameter->getMaxValue()));
    paramData["range"] = variantRange;

    // Добавляем шаг для SpinBox
    paramData["step"] = rangedParameter->getStep();
    paramData["currentValue"] = rangedParameter->getValue();

    return paramData;
}

QVariant OutParametersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_storage) return QVariant();

    auto row = index.row();

    auto parameters = m_storage->getAllParameters();
    auto parameter = parameters[row];

    auto outParameterRole = (OutParameterRole)role;

    switch (outParameterRole)
    {
    case OutParameterRole::LabelRole:
        return parameter->getLabel();
    case OutParameterRole::ValueRole:
        return parameter->getValue();
    case OutParameterRole::DataControlRole:
        return createParameterData(parameter);
    }

    return {};
}

bool OutParametersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || !m_storage)
    {
        return false;
    }

    auto row = index.row();
    if(role == (int)OutParameterRole::ValueRole)
    {
        auto parameters = m_storage->getAllParameters();
        auto parameter = parameters[row];
        if(auto rangeParameter = dynamic_cast<RangedRealOutParameter*>(parameter) != nullptr)
        {
            //rangeParameter->setValue(value.toDouble());
        }
    }

    return true;
}

QVariant OutParametersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return tr("Название");
        case 1:
            return tr("Значение");
        case 2:
            return tr("Управление");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

Qt::ItemFlags OutParametersModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    
    // Разрешаем редактирование только для третьей колонки (управление)
    if (index.column() == 2)
    {
        flags |= Qt::ItemIsEditable;
    }
    
    return flags;
}

void OutParametersModel::setupParameters()
{
    // Пока ничего не делаем
}

void OutParametersModel::setStorage(OutParametersStorage *storage)
{
    if (m_storage != storage)
    {
        m_storage = storage;
        refreshModel();
    }
}

void OutParametersModel::refreshModel()
{
    beginResetModel();
    endResetModel();
}


