#include "ChartPointsModel.h"
#include <QDebug>

ChartPointsModel::ChartPointsModel(const QString &parameterLabel, QObject *parent)
    : QAbstractListModel(parent)
    , m_parameterLabel(parameterLabel)
{
    qDebug() << "ChartPointsModel: Created for parameter" << parameterLabel;
}

ChartPointsModel::ChartPointsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_parameterLabel("")
{
    qDebug() << "ChartPointsModel: Created empty model";
}

int ChartPointsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_points.size();
}

QVariant ChartPointsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_points.size())
        return QVariant();

    const QPointF &point = m_points.at(index.row());

    switch (role) {
    case XRole:
        return point.x();
    case YRole:
        return point.y();
    case TimestampRole:
        if (index.row() < m_timestamps.size()) {
            return m_timestamps.at(index.row());
        }
        return QVariant();
    case ValueRole:
        if (index.row() < m_values.size()) {
            return m_values.at(index.row());
        }
        return QVariant();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ChartPointsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[XRole] = "x";
    roles[YRole] = "y";
    roles[TimestampRole] = "timestamp";
    roles[ValueRole] = "value";
    return roles;
}

void ChartPointsModel::addPoint(const QPointF &point)
{
    beginInsertRows(QModelIndex(), m_points.size(), m_points.size());
    m_points.append(point);
    endInsertRows();
    
    qDebug() << "ChartPointsModel: Added point" << point << "for parameter" << m_parameterLabel;
}

void ChartPointsModel::addPoint(double x, double y)
{
    addPoint(QPointF(x, y));
}

void ChartPointsModel::addPoint(double x, double y, const QDateTime &timestamp, const QVariant &value)
{
    beginInsertRows(QModelIndex(), m_points.size(), m_points.size());
    m_points.append(QPointF(x, y));
    m_timestamps.append(timestamp);
    m_values.append(value);
    endInsertRows();
    
    qDebug() << "ChartPointsModel: Added point" << QPointF(x, y) << "with timestamp" << timestamp << "for parameter" << m_parameterLabel;
}

void ChartPointsModel::clearPoints()
{
    if (m_points.isEmpty())
        return;
        
    beginResetModel();
    m_points.clear();
    m_timestamps.clear();
    m_values.clear();
    endResetModel();
    
    qDebug() << "ChartPointsModel: Cleared all points for parameter" << m_parameterLabel;
}
