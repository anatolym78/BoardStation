import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.15

Item
{
    id: simpleChartsPanel
    
    // Функция для добавления нового графика
    function addChart(chartLabel) 
    {
        console.log("SimpleChartsPanel: Adding chart:", chartLabel)
        if (chartViewModel) 
        {
            chartViewModel.addChart(chartLabel)
            console.log("SimpleChartsPanel: Chart added successfully:", chartLabel)
        }
        else 
        {
            console.warn("SimpleChartsPanel: chartViewModel is not available")
        }
    }

    Flow
    {
        id: chartsFlow
        anchors.fill: parent
        spacing: 10

        Repeater
        {
            model: chartViewModel
            id: repeater

            // График
            delegate: ChartView
            {
                id: chartView
                property string chartLabel: model.chartLabel
                property int chartIndex: model.chartIndex
                property var chartViewData: model.chartView
                width: 320
                height: 240
                backgroundColor: "white"
                theme: ChartView.ChartThemeLight
                
                //title: chartLabel
                
                // Простая настройка осей
                ValueAxis
                {
                    id: xAxis
                    min: 0
                    max: 50
                    tickCount: 3
                    //titleText: "Время (сек)"
                }

                ValueAxis
                {
                    id: yAxis
                    min: -600
                    max: 600
                    tickCount: 3
                    visible: true
                    //titleText: "Значение"
                }

                axes: [xAxis, yAxis]
                antialiasing: true
                
                // Карта для хранения серий по меткам параметров
                property var seriesMap: ({})
                
                // Создаем серии при инициализации
                Component.onCompleted:
                {
                    return

                    // Получаем список параметров для этого графика
                    var series = createSeries(ChartView.SeriesTypeLine, chartLabel, xAxis, yAxis)
                    seriesMap[chartLabel] = series

                    // Загружаем существующие данные
                    loadSeriesData(chartLabel)

                }
                
                // Функция для загрузки данных серии
                function loadSeriesData(parameterLabel)
                {
                    var seriesData = chartViewModel.getSeriesData(chartLabel, parameterLabel)
                    var series = seriesMap[parameterLabel]
                    
                    if (series && seriesData.length > 0)
                    {
                        // Очищаем существующие точки
                        series.clear()
                        
                        // Добавляем все точки
                        for (var i = 0; i < seriesData.length; i++)
                        {
                            var point = seriesData[i]
                            series.append(point.x, point.y)
                        }
                    }
                }
                
                // Обработка добавления новых данных
                Connections
                {
                    target: chartViewModel
                    
                    function onNewParameterValueAdded(label, value, time)
                    {
                        console.log(label)
                    }

                    function onChartDataAdded(chartLabelParam, parameterLabel)
                    {
                        return

                        if (chartLabelParam === chartLabel)
                        {
                            var series = seriesMap[parameterLabel]
                            if (!series)
                            {
                                // Создаем новую серию для параметра
                                series = createSeries(ChartView.SeriesTypeLine, parameterLabel, xAxis, yAxis)
                                seriesMap[parameterLabel] = series
                            }
                            
                            // Загружаем обновленные данные
                            loadSeriesData(parameterLabel)
                            
                            console.log("Chart data updated for parameter:", parameterLabel, "in chart:", chartLabel)
                        }
                    }
                }

                Drag.active: dragHandler.active
                Drag.hotSpot.x: width/2
                Drag.hotSpot.y: height/2

                DragHandler
                {
                    id: dragHandler
                    onActiveChanged:
                    {
                        if(active)
                        {
                            chartView.Drag.start()
                        }
                        else
                        {
                            chartView.Drag.drop()
                        }
                    }
                }

                DropArea
                {
                    anchors.fill: parent
                    onDropped:
                    {
                        if(drag.source !== chartView)
                        {
                            console.log("dropped chart ", drop.source.chartIndex, "target chart ", chartView.chartIndex)
                            chartView.backgroundColor = "white"

                            chartView.opacity = 1
                        }

                    }

                    onEntered:
                    {
                        if(drag.source !== chartView)
                        {
                            chartView.backgroundColor = "lightskyblue"
                            chartView.opacity = 0.25
                            chartView.axes[0] = chartView.axes[1] = false
                        }

                        //console.log("entered ", drag.source)
                    }

                    onExited:
                    {
                        chartView.backgroundColor = "white"
                        chartView.opacity = 1
                    }
                }
            }
        }
    }
}
