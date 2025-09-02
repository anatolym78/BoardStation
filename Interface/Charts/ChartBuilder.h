#ifndef CHARTSBUILDER_H
#define CHARTSBUILDER_H

#include <QObject>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include "./../../Model/Parameters/BoardParameter.h"

QT_CHARTS_USE_NAMESPACE

class ChartBuilder : public QObject
{
    Q_OBJECT

public:
    explicit ChartBuilder(QObject *parent = nullptr);
    
    // Создание нового графика по параметру
    QChartView* createChart(const BoardParameter &parameter);
    
    // Обновление существующего графика по параметру
    void updateChart(QChartView *chartView, const BoardParameter &parameter);
    
    // Проверка, можно ли создать график для параметра
    static bool canCreateChart(const BoardParameter &parameter);
    
    // Добавление новых точек к существующему графику
    void appendNewPoints(QChartView *chartView, const BoardParameter &parameter);

private:
    // Единая функция создания графика (используется и для создания, и для обновления)
    void buildChart(QChart *chart, const BoardParameter &parameter);
    
    // Создание серии данных для параметра
    QLineSeries* createSeries(const BoardParameter &parameter);
    
    // Настройка осей графика
    void setupAxes(QChart *chart, QLineSeries *series, const BoardParameter &parameter);
    
    // Настройка внешнего вида графика
    void setupChartAppearance(QChart *chart, const BoardParameter &parameter);
};

#endif // CHARTSBUILDER_H
