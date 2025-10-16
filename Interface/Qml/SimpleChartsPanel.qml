import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.15

Rectangle
{
    id: simpleChartsPanel
    color: "transparent"
    radius: 4
    
    function toggleParameter(label, color)
    {
        if(chartViewModel)
        {
            if(!chartViewModel.toggleParameter(label, color))
            {
                for (let i = 0; i < repeater.count; i++)
                {
                    const chartView = repeater.itemAt(i);

                    if(chartView.seriesMap.hasOwnProperty(label))
                    {
                        var series = chartView.seriesMap[label]

                        chartView.removeSeries(series)

                        delete chartView.seriesMap.label
                    }
                }
            }
        }
    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Text
        {
            text: "Charts"
            font.pointSize: 12
            font.bold: true
            color: "dimgray"
            Layout.alignment: Qt.AlignHCenter
        }

        Flow
        {
            id: chartsFlow
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10
        
        // Принудительно обновляем layout при изменении размера
        onWidthChanged:
        {
            // Небольшая задержка для корректного пересчета
            Qt.callLater(function()
            {
                chartsFlow.childrenChanged()
            })
        }

        Repeater
        {
            model: chartViewModel
            id: repeater

            // График
            delegate: ChartView
            {
                id: chartView
                property int chartIndex: index
                width: 600
                //width: Math.max(300, Math.min(400, chartsFlow.width / 3))
                height: 480
                backgroundColor: "white"
                z: depth
                theme: ChartView.ChartThemeLight
                
                // Простая настройка осей
                ValueAxis
                {
                    id: xAxis
                    min: 0
                    max: 30
                    tickCount: 3
                }

                ValueAxis
                {
                    id: yAxis
                    min: -600
                    max: 600
                    tickCount: 3
                    visible: true
                }

                axes: [xAxis, yAxis]
                antialiasing: true
                
                // Карта для хранения серий по меткам параметров
                property var seriesMap: ({})
                
                // Создаем серии при инициализации
                Component.onCompleted:
                {

                }
                

                property int timeX: 0
                property bool isStarted: false
                
                Connections
                {
                    target: chartViewModel
                    
                    function onNewParameterValueAdded(label, value, time)
                    {
                        if(label in seriesMap)
                        {
                            var series = seriesMap[label]

                            series.append(time, value)

                            if(!isStarted)
                            {
                                axes[0].min = time
                                axes[0].max = time + 30
                            }
                            else
                            {
                                if(time - axes[0].min > 30)
                                {
                                    // console.log(series.at(1).x)
                                    // console.log(axes[0].min)
                                    // console.log(axes[0].max)

                                    axes[0].min = series.at(1).x
                                    axes[0].max = axes[0].min + 30
                                    series.remove(0)
                                }
                            }

                            isStarted = true
                        }
                    }

                    function onParameterAdded(label, color)
                    {
                        if(label !== chartLabel) return

                        var series = createSeries(ChartView.SeriesTypeLine, chartLabel, xAxis, yAxis)
                        series.color = color
                        seriesMap[label] = series
                    }
                }

                MouseArea
                {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onPressed:
                    {
                        if(mouse.button == Qt.RightButton)
                        {
                            //chartViewModel.splitSeries(chartIndex)

                            //splitSeries(seriesMap)
                        }
                    }
                    onWheel:
                    {
                        if(wheel.angleDelta.y > 0)
                        {
                            zoom(1.1)
                        }
                        else
                        {
                            zoom(0.9)
                        }

                    }
                }

                function splitSeries(map)
                {
                    for(var key in map)
                    {
                         if(map.hasOwnProperty(key))
                         {
                             console.log(map[key])


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

                            chartViewModel.reorderChartsBeforeDrag(chartIndex)
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
                            var droppedChartView = drop.source
                            var targetChartView = chartView

                            var droppedChartLabels = chartViewModel.getChartSeriesLabels(droppedChartView.chartIndex);
                            var targetChartLabels = chartViewModel.getChartSeriesLabels(targetChartView.chartIndex)

                            var droppedSeries = droppedChartView.seriesMap[droppedChartLabels[0]]
                            var targetSeries = targetChartView.seriesMap[targetChartLabels[0]]

                            moveSeries(droppedChartView, targetChartView)

                            chartViewModel.mergeCharts(droppedChartView.chartIndex, targetChartView.chartIndex)

                            targetChartView.backgroundColor = "white"
                            targetChartView.opacity = 1

                            for(let i=0;i<2;i++)
                            {
                                chartView.axes[i].visible = true
                                chartView.axes[i].tickCount = 3
                            }
                        }

                        //chartViewModel.resetDepths()
                    }

                    function moveSeries(droppedChartView, targetChartView)
                    {
                        var droppedChartLabels = chartViewModel.getChartSeriesLabels(droppedChartView.chartIndex);

                        for(const droppedLabel of droppedChartLabels)
                        {
                            var droppedSeries = droppedChartView.seriesMap[droppedLabel]

                            var targetSeries = targetChartView.createSeries(ChartView.SeriesTypeLine, droppedLabel, xAxis, yAxis)
                            targetSeries.color = droppedSeries.color

                            for(var i = 0; i < droppedSeries.count; i++)
                            {
                                var point = droppedSeries.at(i);
                                var x = point.x;
                                var y = point.y;

                                targetSeries.append(x, y)
                            }

                            targetChartView.seriesMap[droppedLabel] = targetSeries
                        }
                    }

                    onEntered:
                    {
                        if(drag.source !== chartView)
                        {
                            chartView.backgroundColor = "lightskyblue"
                            chartView.opacity = 0.5
                            for(let i=0;i<2;i++)
                            {
                                chartView.axes[i].visible = false
                                chartView.axes[i].tickCount = 0
                            }
                        }

                    }

                    onExited:
                    {
                        chartView.backgroundColor = "white"
                        chartView.opacity = 1

                        for(let i=0;i<2;i++)
                        {
                            chartView.axes[i].visible = true
                            chartView.axes[i].tickCount = 3
                        }
                    }
                }
            }
        }
        }
    }
}
