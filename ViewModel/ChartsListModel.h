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
        SeriesModelRole = Qt::UserRole + 1,
        HasSeriesRole
    };

    explicit ChartsListModel(BoardParametersStorage *parametersStorage, QObject *parent = nullptr);
    explicit ChartsListModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Методы для работы с сериями
    Q_INVOKABLE ChartSeriesModel* addSeries(const QString &parameterLabel);
    Q_INVOKABLE void addSeries(const QStringList &parameterLabels);
    Q_INVOKABLE void removeSeries(const QString &parameterLabel);
    Q_INVOKABLE void removeSeries(int index);
    Q_INVOKABLE void clearSeries();
    
    // Геттеры
    Q_INVOKABLE ChartSeriesModel* getSeriesModel(const QString &parameterLabel) const;
    Q_INVOKABLE ChartSeriesModel* getSeriesModel(int index) const;
    Q_INVOKABLE QStringList parameterLabels() const;
    
    // Проверки
    Q_INVOKABLE bool hasSeries(const QString &parameterLabel) const;
    Q_INVOKABLE bool hasSeries(int index) const;
    Q_INVOKABLE int seriesCount() const { return m_chartsModels.size(); }
    
    // Установка хранилища параметров
    Q_INVOKABLE void setParametersStorage(BoardParametersStorage *storage);
    
    // Получение данных серии для QML (возвращает QVariantList)
    Q_INVOKABLE QVariantList getSeriesData(const QString &parameterLabel) const;


signals:
    void seriesDataChanged(const QString &parameterLabel);
    void parameterAdded(const QString &parameterLabel);
    void parameterUpdate(const QString& parameterLabel);
    void parameterValueAdded(const QString& parameterLabel);

private slots:
	void onParameterAdded(const QString& label);
	void onParameterUpdated(const QString& label);

private:
    QList<ChartSeriesModel*> m_chartsModels;
    BoardParametersStorage *m_parametersStorage;
};

#endif // CHARTSLISTMODEL_H
