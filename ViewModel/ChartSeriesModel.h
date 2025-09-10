#ifndef CHARTSERIESMODEL_H
#define CHARTSERIESMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QStringList>
#include <QList>
#include "ChartPointsModel.h"
#include "../Model/Parameters/BoardParametersStorage.h"

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

    explicit ChartSeriesModel(QObject *parent = nullptr);
    explicit ChartSeriesModel(const QStringList &parameterLabels, QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Методы для работы с сериями
    void addSeries(const QString &parameterLabel);
    void addSeries(const QStringList &parameterLabels);
    void removeSeries(const QString &parameterLabel);
    void removeSeries(int index);
    void clearSeries();

    Q_INVOKABLE int countSeries() { return rowCount(); }
    
    // Методы для работы с точками
    void addPoint(const QString &parameterLabel, const QPointF &point);
    void addPoint(const QString &parameterLabel, double x, double y);
    void addPoint(const QString &parameterLabel, double x, double y, const QDateTime &timestamp, const QVariant &value);

	void createColorMap();

    // Геттеры
    Q_INVOKABLE ChartPointsModel* getPointsModel(const QString &parameterLabel) const;
    Q_INVOKABLE ChartPointsModel* getPointsModel(int index) const;
    Q_INVOKABLE QStringList parameterLabels() const;
    
    // Проверки
    bool hasSeries(const QString &parameterLabel) const;
    bool hasSeries(int index) const;
    int seriesCount() const { return m_seriesModels.size(); }
    
    // Установка хранилища параметров
    void setParametersStorage(BoardParametersStorage *storage);
    
    // Публичный метод для обработки добавления параметра
    void handleParameterAdded(const QString &label);

signals:
    void parameterValueAdded(const QString& label);

private slots:
    void onParameterAdded(const QString &label);

private:
    QList<ChartPointsModel*> m_seriesModels;
    QStringList m_parameterLabels;
    BoardParametersStorage *m_parametersStorage;
    QMap<QString, QColor> m_colorMap;
};

#endif // CHARTSERIESMODEL_H
