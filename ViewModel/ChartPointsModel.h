#ifndef CHARTPOINTSMODEL_H
#define CHARTPOINTSMODEL_H

#include <QAbstractListModel>
#include <QPointF>
#include <QString>
#include <QList>
#include <QDateTime>
#include <QVariant>
#include <QColor>

class ChartPointsModel : public QAbstractListModel
{
    Q_OBJECT
	Q_ENUMS(ChartPointsRoles)
public:
    enum ChartPointsRoles {
        XRole = Qt::UserRole + 1,
        YRole,
        TimestampRole,
        ValueRole,
        ColorRole,
    };

    explicit ChartPointsModel(const QString &parameterLabel, QObject *parent = nullptr);
    explicit ChartPointsModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Методы для работы с точками
    void addPoint(const QPointF &point);
    void addPoint(double x, double y);
    void addPoint(double x, double y, const QDateTime &timestamp, const QVariant &value);
    void clearPoints();
    Q_INVOKABLE int countPoints() const { return m_points.count(); }
    
    // Геттеры
    Q_INVOKABLE QString parameterLabel() const { return m_parameterLabel; }
    Q_INVOKABLE QList<QPointF> points() const { return m_points; }
    QList<QDateTime> timestamps() const { return m_timestamps; }
    QList<QVariant> values() const { return m_values; }

	Q_INVOKABLE qreal lastX() const;

    Q_INVOKABLE qreal firstX() const;
	Q_INVOKABLE qreal lastY() const;
    Q_INVOKABLE QVariant lastPoint() const;
	Q_INVOKABLE QVariant lastTimeStamp() const;
    Q_INVOKABLE qreal elapsedTime() const;

    Q_INVOKABLE QVariant pointsColor() const;
    void setColor(QColor rgb);

    //Q_INVOKABLE


    // Проверки
    bool hasPoints() const { return !m_points.isEmpty(); }
    int pointCount() const { return m_points.size(); }

private:
    QString m_parameterLabel;      // Метка параметра
    QList<QPointF> m_points;      // Точки графика
    QList<QDateTime> m_timestamps; // Временные метки
    QList<QVariant> m_values;    // Значения параметра
    QDateTime m_startTime;
    QColor m_color = Qt::white;
};

#endif // CHARTPOINTSMODEL_H
