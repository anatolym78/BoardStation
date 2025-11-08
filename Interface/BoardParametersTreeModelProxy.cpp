#include "BoardParametersTreeModelProxy.h"

BoardParametersTreeModelProxy::BoardParametersTreeModelProxy(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

QVariant BoardParametersTreeModelProxy::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0) {
            return sourceModel()->data(index, sourceModel()->roleNames().key("label"));
        }
        if (index.column() == 1) {
            return sourceModel()->data(index, sourceModel()->roleNames().key("value"));
        }
    }
    return QIdentityProxyModel::data(index, role);
}

QVariant BoardParametersTreeModelProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section < 2)
        return m_horizontalHeaders[section];

    return QIdentityProxyModel::headerData(section, orientation, role);
}

bool BoardParametersTreeModelProxy::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation == Qt::Horizontal && role == Qt::EditRole && section < 2) {
        m_horizontalHeaders[section] = value;
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return QIdentityProxyModel::setHeaderData(section, orientation, value, role);
}
