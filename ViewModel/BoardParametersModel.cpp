#include "BoardParametersModel.h"
#include <QDebug>

BoardParametersModel::BoardParametersModel(BoardParameterHistoryStorage *storage, QObject *parent)
    : QAbstractTableModel(parent)
    , m_storage(storage)
{
    if (m_storage) 
    {
        setParametersStorage(m_storage);
    }
}

int BoardParametersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_parameterLabels.size();
}

int BoardParametersModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 4; // Четыре колонки: название, значение, единица измерения, время
}

QVariant BoardParametersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_parameterLabels.size() || !m_storage)
        return QVariant();

    const QString &label = m_parameterLabels.at(index.row());
    BoardParameterHistory *param = m_storage->getParameterHistory(label);

    if (role == Qt::DisplayRole && param) {
        switch (index.column()) {
        case LabelColumn:
            //qDebug() << "BoardParametersModel: Returning label for row" << index.row() << ":" << param->label();
            return param->label();
        case ValueColumn:
            //qDebug() << "BoardParametersModel: Returning value for row" << index.row() << ":" << param->lastValueData();
            return param->lastValueData();
        case UnitColumn:
            //qDebug() << "BoardParametersModel: Returning unit for row" << index.row() << ":" << param->unit();
            return param->unit();
        case TimestampColumn:
            //qDebug() << "BoardParametersModel: Returning timestamp for row" << index.row() << ":" << param->lastTimestamp().toString("hh:mm:ss");
            return param->lastTimestamp().toString("hh:mm:ss");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QVariant BoardParametersModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QHash<int, QByteArray> BoardParametersModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[LabelColumn] = "label";
    roles[ValueColumn] = "value";
    roles[UnitColumn] = "unit";
    roles[TimestampColumn] = "timestamp";
    return roles;
}

void BoardParametersModel::setParametersStorage(BoardParameterHistoryStorage *storage)
{
    if (m_storage) {
        disconnect(m_storage, nullptr, this, nullptr);
    }
    
    m_storage = storage;
    
    if (m_storage) 
    {
        //connect(m_storage, &BoardParameterHistoryStorage::parameterAdded, 
        //        this, &BoardParametersModel::onParameterAdded);

        //connect(m_storage, &BoardParameterHistoryStorage::parameterUpdated, 
        //        this, &BoardParametersModel::onParameterUpdated);

		connect(m_storage, &BoardParameterHistoryStorage::newParameterAdded,
			this, &BoardParametersModel::onNewParameterAdded);

        connect(m_storage, &BoardParameterHistoryStorage::parametersCleared, 
                this, &BoardParametersModel::onParametersCleared);
        
        refreshModel();
    }
}

void BoardParametersModel::refreshModel()
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

BoardParameterHistory* BoardParametersModel::getParameter(int index) const
{
    if (index >= 0 && index < m_parameterLabels.size() && m_storage) {
        return m_storage->getParameterHistory(m_parameterLabels.at(index));
    }
    return nullptr;
}

BoardParameterHistory* BoardParametersModel::getParameter(const QString &label) const
{
    if (m_storage) {
        return m_storage->getParameterHistory(label);
    }
    return nullptr;
}

BoardParameterValue* BoardParametersModel::getLastValue(int index) const
{
    if (index >= 0 && index < m_parameterLabels.size() && m_storage) {
        BoardParameterHistory *param = m_storage->getParameterHistory(m_parameterLabels.at(index));
        if (param) {
            return param->lastValue();
        }
    }
    return nullptr;
}

BoardParameterValue* BoardParametersModel::getLastValue(const QString &label) const
{
    if (m_storage) {
        BoardParameterHistory *param = m_storage->getParameterHistory(label);
        if (param) {
            return param->lastValue();
        }
    }
    return nullptr;
}

QList<BoardParameterValue*> BoardParametersModel::getValueHistory(int index) const
{
    if (index >= 0 && index < m_parameterLabels.size() && m_storage) {
        BoardParameterHistory *param = m_storage->getParameterHistory(m_parameterLabels.at(index));
        if (param) {
            return param->values();
        }
    }
    return QList<BoardParameterValue*>();
}

QList<BoardParameterValue*> BoardParametersModel::getValueHistory(const QString &label) const
{
    if (m_storage) {
        BoardParameterHistory *param = m_storage->getParameterHistory(label);
        if (param) {
            return param->values();
        }
    }
    return QList<BoardParameterValue*>();
}

void BoardParametersModel::onNewParameterAdded(BoardParameterSingle* parameter)
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

void BoardParametersModel::onParameterAdded(const QString &label)
{
    // Добавляем новую строку
    beginInsertRows(QModelIndex(), m_parameterLabels.size(), m_parameterLabels.size());
    m_parameterLabels.append(label);
    endInsertRows();
}

void BoardParametersModel::onParameterUpdated(const QString &label)
{
    // Находим индекс строки и обновляем данные
    int row = m_parameterLabels.indexOf(label);
    if (row >= 0) 
    {
        QModelIndex topLeft = this->index(row, 0);
        QModelIndex bottomRight = this->index(row, columnCount() - 1);
        emit dataChanged(topLeft, bottomRight);

        emit parameterUpdated(label);
    }
}

void BoardParametersModel::onParametersCleared()
{
    // Очищаем все строки
    beginResetModel();
    m_parameterLabels.clear();
    endResetModel();
}
