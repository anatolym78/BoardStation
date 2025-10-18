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
                return

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
                DateTimeAxis
                {
                    id: timeAxis
                    format: "hh:mm:ss"
                    min: new Date(0)
                    max: new Date(0)
                    tickCount: 3
                }

                ValueAxis
                {
                    id: valueAxis
                    min: -600
                    max: 600
                    tickCount: 3
                    visible: true
                }

                axes: [timeAxis, valueAxis]
                antialiasing: true
                
                // Карта для хранения серий по меткам параметров
                property var seriesMap: ({})
                
                // Создаем серии при инициализации
                Component.onCompleted:
                {

                }
                

                property int timeX: 0
                property bool isStarted: false

                function firstOf(series)
                {
                    return series.count > 0 ? series.at(0) : null
                }

                function lastOf(series)
                {
                    return series.count > 0 ? series.at(series.count - 1) : null
                }

                function initializeAxes(parameter, timeAxis, valueAxis)
                {
                    var parameterValue = parameter.value
                    var parameterDate = new Date(parameter.timestamp)

                    timeAxis.min = new Date(parameterDate.getTime());
                    timeAxis.max = new Date(parameterDate.getTime() + 60*1000)

                    valueAxis.min = parameterValue - 1
                    valueAxis.max = parameterValue + 1
                }

                function fitAxes(parameter, timeAxis, valueAxis, series)
                {
                    // корректируем если более одной точки
                    var countPoints = series.count
                    if(countPoints < 2) return

                    var parameterValue = parameter.value
                    var parameterDate = new Date(parameter.timestamp)
                    var parameterTime = parameterDate.getTime()

                    // корректируем ось значений (сейчас логика такова, что диапазон может только расти, но не может сужаться)
                    var valueMin = valueAxis.min
                    var valueMax = valueAxis.max

                    if(valueMax < parameterValue)
                    {
                        valueAxis.max = parameterValue + 1
                    }

                    if(valueMin > parameterValue)
                    {
                        valueAxis.min = parameterValue - 1
                    }

                    // корректируем временную шкалу
                    var timeMin = timeAxis.min
                    var timeMax = timeAxis.max
                    var timeRange = timeMax - timeMin

                    if(parameterDate > timeMax)
                    {
                        timeAxis.max = parameterDate
                        timeAxis.min = new Date(parameterTime - timeRange)
                    }

                    if(parameterDate < timeMin)
                    {
                        timeAxis.min = parameterDate
                        timeAxis.max = new Date(parameterTime + timeRange)
                    }
                }

                function isChartHasLabel(chartLabels, label)
                {
                    return chartLabels.indexOf(label) >= 0
                }

                function seriesCreated(seriesMap, label)
                {
                    return label in seriesMap
                }

                Connections
                {
                    target: parametersPlayer

                    function onParameterPlayed(parameter)
                    {
                        var parameterLabel = parameter.label

                        if(labels.indexOf(parameterLabel) < 0) return

                        var parameterValue = parameter.value
                        var parameterDate = new Date(parameter.timestamp)
                        var parameterTimeMsec = parameterDate.getTime()

                        if(!(parameterLabel in seriesMap))
                        {
                            initializeAxes(parameter, timeAxis, valueAxis)

                            seriesMap[parameterLabel] = createSeries(ChartView.SeriesTypeLine, parameterLabel, timeAxis, valueAxis)
                        }

                        seriesMap[parameterLabel].append(parameterTimeMsec, parameterValue)

                        fitAxes(parameter, timeAxis, valueAxis, seriesMap[parameterLabel])
                    }
                }



                function addValue(value, dateTime, label)
                {
                    if(!(label in seriesMap))
                    {
                        seriesMap[parameterLabel] = createSeries(ChartView.SeriesTypeLine, label, timeAxis, valueAxis)
                    }

                    var series = seriesMap[parameterLabel]


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
