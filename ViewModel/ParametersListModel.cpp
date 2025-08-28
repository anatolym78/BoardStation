#include "ParametersListModel.h"
#include <QDebug>

ParametersListModel::ParametersListModel(ParametersStorage *storage, QObject *parent)
    : QAbstractTableModel(parent)
    , m_storage(storage)
{
    if (m_storage) {
        connect(m_storage, &ParametersStorage::parameterAdded, 
                this, &ParametersListModel::onParameterAdded);
        connect(m_storage, &ParametersStorage::parameterUpdated, 
                this, &ParametersListModel::onParameterUpdated);
        connect(m_storage, &ParametersStorage::parametersCleared, 
                this, &ParametersListModel::onParametersCleared);
        
        refreshModel();
    }
}

int ParametersListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_parameterLabels.size();
}

int ParametersListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 4; // Четыре колонки: название, значение, единица измерения, время
}

QVariant ParametersListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_parameterLabels.size() || !m_storage)
        return QVariant();

    const QString &label = m_parameterLabels.at(index.row());
    const Parameter &param = m_storage->getParameter(label);

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case LabelColumn:
            return param.label;
        case ValueColumn:
            return param.lastValueData();
        case UnitColumn:
            return param.unit;
        case TimestampColumn:
            return param.lastTimestamp().toString("hh:mm:ss");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QVariant ParametersListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case LabelColumn:
            return tr("Parameter Name");
        case ValueColumn:
            return tr("Value");
        case UnitColumn:
            return tr("Unit");
        case TimestampColumn:
            return tr("Time");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QHash<int, QByteArray> ParametersListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[LabelColumn] = "label";
    roles[ValueColumn] = "value";
    roles[UnitColumn] = "unit";
    roles[TimestampColumn] = "timestamp";
    return roles;
}

void ParametersListModel::setParametersStorage(ParametersStorage *storage)
{
    if (m_storage) {
        disconnect(m_storage, nullptr, this, nullptr);
    }
    
    m_storage = storage;
    
    if (m_storage) {
        connect(m_storage, &ParametersStorage::parameterAdded, 
                this, &ParametersListModel::onParameterAdded);
        connect(m_storage, &ParametersStorage::parameterUpdated, 
                this, &ParametersListModel::onParameterUpdated);
        connect(m_storage, &ParametersStorage::parametersCleared, 
                this, &ParametersListModel::onParametersCleared);
        
        refreshModel();
    }
}

void ParametersListModel::refreshModel()
{
    if (!m_storage) {
        beginResetModel();
        m_parameterLabels.clear();
        endResetModel();
        return;
    }
    
    beginResetModel();
    m_parameterLabels = m_storage->getParameterLabels();
    endResetModel();
}

Parameter ParametersListModel::getParameter(int index) const
{
    if (index >= 0 && index < m_parameterLabels.size() && m_storage) {
        return m_storage->getParameter(m_parameterLabels.at(index));
    }
    return Parameter();
}

Parameter ParametersListModel::getParameter(const QString &label) const
{
    if (m_storage) {
        return m_storage->getParameter(label);
    }
    return Parameter();
}

ParameterValue ParametersListModel::getLastValue(int index) const
{
    if (index >= 0 && index < m_parameterLabels.size() && m_storage) {
        const Parameter &param = m_storage->getParameter(m_parameterLabels.at(index));
        return param.lastValue();
    }
    return ParameterValue();
}

ParameterValue ParametersListModel::getLastValue(const QString &label) const
{
    if (m_storage) {
        const Parameter &param = m_storage->getParameter(label);
        return param.lastValue();
    }
    return ParameterValue();
}

QList<ParameterValue> ParametersListModel::getValueHistory(int index) const
{
    if (index >= 0 && index < m_parameterLabels.size() && m_storage) {
        const Parameter &param = m_storage->getParameter(m_parameterLabels.at(index));
        return param.values;
    }
    return QList<ParameterValue>();
}

QList<ParameterValue> ParametersListModel::getValueHistory(const QString &label) const
{
    if (m_storage) {
        const Parameter &param = m_storage->getParameter(label);
        return param.values;
    }
    return QList<ParameterValue>();
}

void ParametersListModel::onParameterAdded(const QString &label)
{
    // Добавляем новую строку
    beginInsertRows(QModelIndex(), m_parameterLabels.size(), m_parameterLabels.size());
    m_parameterLabels.append(label);
    endInsertRows();
}

void ParametersListModel::onParameterUpdated(const QString &label)
{
    // Находим индекс строки и обновляем данные
    int row = m_parameterLabels.indexOf(label);
    if (row >= 0) {
        QModelIndex topLeft = this->index(row, 0);
        QModelIndex bottomRight = this->index(row, columnCount() - 1);
        emit dataChanged(topLeft, bottomRight);
    }
}

void ParametersListModel::onParametersCleared()
{
    // Очищаем все строки
    beginResetModel();
    m_parameterLabels.clear();
    endResetModel();
}
