#ifndef CHARTVIEWMODEL_H
#define CHARTVIEWMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QDateTime>
#include <QVariant>
#include <QColor>
#include "../Model/Parameters/BoardParameterHistoryStorage.h"

class ChartViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ChartViewRoles {
        ChartViewRole = Qt::UserRole + 1,
        ChartTitleRole,
        ChartIndexRole,
        HasDataRole
    };

    explicit ChartViewModel(QObject *parent = nullptr);
    explicit ChartViewModel(BoardParameterHistoryStorage *parametersStorage, QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Методы для работы с графиками
    Q_INVOKABLE QtCharts::QChartView* addChart(const QString &title);
    Q_INVOKABLE void removeChart(int index);
    Q_INVOKABLE void clearCharts();
    
    // Методы для работы с данными
    Q_INVOKABLE void addDataPoint(const QString &chartTitle, const QString &parameterLabel, 
                                  double x, double y, const QDateTime &timestamp, const QVariant &value);
    Q_INVOKABLE void addDataPoint(const QString &chartTitle, const QString &parameterLabel, 
                                  double x, double y);
    
    // Геттеры
    Q_INVOKABLE QtCharts::QChartView* getChartView(int index) const;
    Q_INVOKABLE QtCharts::QChartView* getChartView(const QString &title) const;
    Q_INVOKABLE QStringList chartTitles() const;
    
    // Проверки
    Q_INVOKABLE bool hasChart(const QString &title) const;
    Q_INVOKABLE bool hasChart(int index) const;
    Q_INVOKABLE int chartCount() const { return m_chartViews.size(); }
    
    // Установка хранилища параметров
    Q_INVOKABLE void setParametersStorage(BoardParameterHistoryStorage *storage);

signals:
    void chartDataAdded(const QString &chartTitle, const QString &parameterLabel);

private slots:
    void onNewParameterAdded(BoardParameterSingle* parameter);

private:
    struct ChartData {
        QtCharts::QChartView* chartView;
        QString title;
        QHash<QString, QtCharts::QLineSeries*> seriesMap; // Карта серий по меткам параметров
        QtCharts::QValueAxis* xAxis;
        QtCharts::QValueAxis* yAxis;
        QDateTime startTime;
    };

    QList<ChartData> m_chartViews;
    BoardParameterHistoryStorage *m_parametersStorage;
    
    // Вспомогательные методы
    ChartData* findChartData(const QString &title);
    ChartData* findChartData(int index);
    QtCharts::QLineSeries* getOrCreateSeries(ChartData* chartData, const QString &parameterLabel);
    void setupChart(ChartData* chartData);
};

#endif // CHARTVIEWMODEL_H
