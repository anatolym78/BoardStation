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
                theme: ChartView.ChartThemeLight
                
                title: chartLabel
                
                // Простая настройка осей
                ValueAxis
                {
                    id: xAxis
                    min: 0
                    max: 50
                    tickCount: 2
                    //titleText: "Время (сек)"
                }

                ValueAxis
                {
                    id: yAxis
                    min: -600
                    max: 600
                    tickCount: 3
                    //titleText: "Значение"
                }

                axes: [xAxis, yAxis]
                antialiasing: true
                
                // Карта для хранения серий по меткам параметров
                property var seriesMap: ({})
                
                // Создаем серии при инициализации
                Component.onCompleted:
                {
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

                // Drag.hotSpot.x: chartView.width/2
                // Drag.hotSpot.y: chartView.height/2
                // Drag.source: chartView
                // Drag.active: dragHandler.active
                DragHandler
                {
                    id: dragHandler
                    onActiveChanged:
                    {
                        console.log("drag active: ", chartView.chartLabel)
                    }
                }

                DropArea
                {
                    onDropped:
                    {
                        console.log("dropped")
                    }

                    onEntered:
                    {
                        //console.log("entered ", drag.source)
                    }

                    onExited:
                    {
                        console.log("exited")
                    }



                }
            }
        }
    }
}
