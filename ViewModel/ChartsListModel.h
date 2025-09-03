#ifndef CHARTSLISTMODEL_H
#define CHARTSLISTMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QList>
#include "ChartSeriesModel.h"
#include "../Model/Parameters/BoardParametersStorage.h"

class ChartsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ChartsListRoles {
        ChartNameRole = Qt::UserRole + 1,
        SeriesModelRole,
        SeriesCountRole,
        HasSeriesRole
    };

    explicit ChartsListModel(BoardParametersStorage *parametersStorage, QObject *parent = nullptr);
    explicit ChartsListModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Методы для работы с графиками
    void addChart(const QString &chartName);
    void addChart(const QString &chartName, const QStringList &parameterLabels);
    void removeChart(const QString &chartName);
    void removeChart(int index);
    void clearCharts();
    
    // Методы для работы с сериями
    void addSeriesToChart(const QString &chartName, const QString &parameterLabel);
    void addSeriesToChart(const QString &chartName, const QStringList &parameterLabels);
    void removeSeriesFromChart(const QString &chartName, const QString &parameterLabel);
    
    // Геттеры
    ChartSeriesModel* getSeriesModel(const QString &chartName) const;
    ChartSeriesModel* getSeriesModel(int index) const;
    QStringList chartNames() const;
    
    // Проверки
    bool hasChart(const QString &chartName) const;
    bool hasChart(int index) const;
    int chartCount() const { return m_chartsModels.size(); }
    
    // Установка хранилища параметров
    void setParametersStorage(BoardParametersStorage *storage);

private slots:
    void onParameterAdded(const QString &label);

private:
    QList<ChartSeriesModel*> m_chartsModels;
    QStringList m_chartNames;
    BoardParametersStorage *m_parametersStorage;
};

#endif // CHARTSLISTMODEL_H
