#include "ChartPointsModel.h"
#include <QDebug>

ChartPointsModel::ChartPointsModel(const QString &parameterLabel, QObject *parent)
    : QAbstractListModel(parent)
    , m_parameterLabel(parameterLabel)
{
    //qDebug() << "ChartPointsModel: Created for parameter" << parameterLabel;
}

ChartPointsModel::ChartPointsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_parameterLabel("")
{
    //qDebug() << "ChartPointsModel: Created empty model";
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
        if (index.row() < m_timestamps.size()) 
        {
            return m_timestamps.at(index.row());
        }
        return QVariant();
    case ValueRole:
        if (index.row() < m_values.size()) {
            return m_values.at(index.row());
        }
        return QVariant();
    case ColorRole:
        return QVariant::fromValue(m_color);
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
    roles[ColorRole] = "color";
    return roles;
}

void ChartPointsModel::addPoint(const QPointF &point)
{
    beginInsertRows(QModelIndex(), m_points.size(), m_points.size());
    m_points.append(point);
    if(m_points.count() == 0)
    {
	    
    }
    endInsertRows();
    
    //qDebug() << "ChartPointsModel: Added point" << point << "for parameter" << m_parameterLabel;
}

void ChartPointsModel::addPoint(double x, double y)
{
    addPoint(QPointF(x, y));
}

void ChartPointsModel::addPoint(double x, double y, const QDateTime &timestamp, const QVariant &value)
{
    beginInsertRows(QModelIndex(), m_points.size(), m_points.size());
    if(m_points.isEmpty())
    {
        m_startTime = timestamp;
    }
    m_points.append(QPointF(x, y));
    m_timestamps.append(timestamp);
    m_values.append(value);
    endInsertRows();
    
    //qDebug() << "ChartPointsModel: Added point" << QPointF(x, y) << "with timestamp" << timestamp << "for parameter" << m_parameterLabel;
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
    
    //qDebug() << "ChartPointsModel: Cleared all points for parameter" << m_parameterLabel;
}

Q_INVOKABLE qreal ChartPointsModel::lastX() const
{
    if (rowCount() == 0) return 0;

    return m_points.last().x();
}

Q_INVOKABLE qreal ChartPointsModel::firstX() const
{
	if (rowCount() == 0) return 0;

	return m_points.first().x();
}

Q_INVOKABLE qreal ChartPointsModel::lastY() const
{
	if (rowCount() == 0) return 0;

    return m_points.last().y();
}

Q_INVOKABLE QVariant ChartPointsModel::lastPoint() const
{
	if (rowCount() == 0) return QVariant::fromValue(QPointF());

	return QVariant::fromValue(m_points.last());
}

Q_INVOKABLE QVariant ChartPointsModel::lastTimeStamp() const
{
    if (rowCount() == 0) return (int)0;

	return m_timestamps.last();
}

Q_INVOKABLE qreal ChartPointsModel::elapsedTime() const
{
    if (m_timestamps.isEmpty()) return 0;

    return (qreal)m_startTime.secsTo(m_timestamps.last());
}

Q_INVOKABLE QVariant ChartPointsModel::pointsColor() const
{
    return QVariant::fromValue(m_color);
}

void ChartPointsModel::setColor(QColor rgb)
{
    m_color = rgb;
}
