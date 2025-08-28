#include "OutParametersModel.h"
#include <QDebug>

OutParametersModel::OutParametersModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    setupParameters();
}

int OutParametersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 4; // Пока возвращаем 4 строки для совместимости
}

int OutParametersModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 3; // Три колонки: название, значение, управление
}

QVariant OutParametersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: // Название
            return QString("Параметр %1").arg(index.row() + 1);
        case 1: // Значение
            return QString("0");
        case 2: // Управление
            return QVariant(); // Пока пусто
        default:
            return QVariant();
        }
    }

    return QVariant();
}

bool OutParametersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(role)
    
    // Пока ничего не делаем
    return false;
}

QVariant OutParametersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
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

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void OutParametersModel::setupParameters()
{
    // Пока ничего не делаем
}
