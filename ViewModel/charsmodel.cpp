#include "charsmodel.h"

CharsModel::CharsModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

int CharsModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

QVariant CharsModel::data(const QModelIndex &index, int role) const
{
    return {};
}
