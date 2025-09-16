#ifndef CHARTSERIESMODEL_H
#define CHARTSERIESMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QStringList>
#include <QList>
#include "ChartPointsModel.h"
#include "../Model/Parameters/BoardParameterHistoryStorage.h"

class ChartSeriesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ChartSeriesRoles {
        ParameterLabelRole = Qt::UserRole + 1,
        PointsModelRole,
        PointCountRole,
        HasPointsRole
    };

    explicit ChartSeriesModel(QObject *parent = nullptr, int depth = 0);
    explicit ChartSeriesModel(const QStringList &parameterLabels, QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Методы для работы с сериями
    Q_INVOKABLE void addSeries(const QString &parameterLabel);
    Q_INVOKABLE void addSeries(const QStringList &parameterLabels);
    Q_INVOKABLE void removeSeries(const QString &parameterLabel);
    Q_INVOKABLE void removeSeries(int index);
    Q_INVOKABLE void clearSeries();
    Q_INVOKABLE void copySeriesFrom(ChartSeriesModel* sourceModel);

    Q_INVOKABLE int countSeries() { return rowCount(); }
    
    // Методы для работы с точками
    void addPoint(const QString &parameterLabel, const QPointF &point);
    void addPoint(const QString &parameterLabel, double x, double y);
    void addPoint(const QString &parameterLabel, double x, double y, const QDateTime &timestamp, const QVariant &value);

	void createColorMap();

    void setDepth(int depth);

    int depth() const;

    // Геттеры
    Q_INVOKABLE ChartPointsModel* getPointsModel(const QString &parameterLabel) const;
    Q_INVOKABLE ChartPointsModel* getPointsModel(int index) const;
    Q_INVOKABLE QStringList parameterLabels() const;
    
    // Проверки
    bool hasSeries(const QString &parameterLabel) const;
    bool hasSeries(int index) const;
    int seriesCount() const { return m_seriesModels.size(); }
    
    // Установка хранилища параметров
    void setParametersStorage(BoardParameterHistoryStorage *storage);
    
    // Публичный метод для обработки добавления параметра
    void handleParameterAdded(const QString &label);

signals:
    void parameterValueAdded(const QString& label);

private slots:
    void onParameterAdded(const QString &label);
    void onParameterUpdated(const QString& label);

private:
    QList<ChartPointsModel*> m_seriesModels;
    QStringList m_parameterLabels;
    BoardParameterHistoryStorage *m_parametersStorage;
    QMap<QString, QColor> m_colorMap;
    int m_depth = 0;
};

#endif // CHARTSERIESMODEL_H
