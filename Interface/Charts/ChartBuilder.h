#ifndef CHARTSBUILDER_H
#define CHARTSBUILDER_H

#include <QObject>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include "./../../Model/Parameter.h"

QT_CHARTS_USE_NAMESPACE

class ChartBuilder : public QObject
{
    Q_OBJECT

public:
    explicit ChartBuilder(QObject *parent = nullptr);
    
    // Создание нового графика по параметру
    QChartView* createChart(const Parameter &parameter);
    
    // Обновление существующего графика по параметру
    void updateChart(QChartView *chartView, const Parameter &parameter);
    
    // Проверка, можно ли создать график для параметра
    static bool canCreateChart(const Parameter &parameter);
    
    // Добавление новых точек к существующему графику
    void appendNewPoints(QChartView *chartView, const Parameter &parameter);

private:
    // Единая функция создания графика (используется и для создания, и для обновления)
    void buildChart(QChart *chart, const Parameter &parameter);
    
    // Создание серии данных для параметра
    QLineSeries* createSeries(const Parameter &parameter);
    
    // Настройка осей графика
    void setupAxes(QChart *chart, QLineSeries *series, const Parameter &parameter);
    
    // Настройка внешнего вида графика
    void setupChartAppearance(QChart *chart, const Parameter &parameter);
};

#endif // CHARTSBUILDER_H
