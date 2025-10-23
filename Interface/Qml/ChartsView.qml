import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.15

Rectangle
{
    id: chartsView
    color: "transparent"
    radius: 4
    clip: true

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 50

        RowLayout
        {
            id: chartTitleLayout
            Layout.fillWidth: true
            //Layout.alignment: Qt.AlignHCenter
            //Layout.preferredHeight: 0

            TextEdit
            {
                id: chartsPanelTitle
                text: "Charts"
                color: "gray"
                font.pointSize: 13
                font.bold: true
                visible: true
            }

            Switch
            {
                id: rowsCountSwitch
                onToggled:
                {
                    var k = checked ? 2 : 1;
                    // Нет пересчёта, оставляем как было (пользователь может раскомментировать при необходимости)
                }
                checked: true

                Connections
                {
                    target: qmlMainWindow
                    onWidthChanged:
                    {
                        // Нет пересчёта, чтобы избежать поломок
                    }
                }
                text: checked ? "make one column" : "make two columns"
                font.pointSize: 13
                visible: repeater.count > 1
            }

            Button
            {
                id: mergeChartsButton
                text: "Merge charts"
                font.pointSize: 13
                visible: chartViewModel.isCanMergeCharts
                Layout.alignment: Qt.AlignRight
            }

            Connections
            {
                target: chartViewModel
                onIsCanMergeChartsChanged:
                {
                    console.log("force layout")
                    chartTitleLayout.forceLayout()
                }
            }
        }

        ScrollView
        {
            id: scrollView
            Layout.fillHeight: true
            Layout.fillWidth: true
            clip: true

            GridLayout
            {
                id: gridLayout
                width: parent.width
                //columns: Math.max(1, Math.floor(width / 300))  // Динамическое количество столбцов
                rowSpacing: 0
                columnSpacing: 0
                columns: rowsCountSwitch.checked ? 2 : 1

                Repeater
                {
                    id: repeater
                    model: chartViewModel

                    delegate: Item
                    {
                        //color: "yellow"
                        visible: true
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredWidth: calculatePreferedSize()  // рекомендуемая ширина
                        Layout.preferredHeight: width  // рекомендуемая высота

                        function calculatePreferedSize()
                        {
                            return chartsView.width/ (rowsCountSwitch.checked ? 2 : 1)
                        }

                        property var seriesMap: ({})

                        property real markerTimeMs: 0
                        function updateTimeMarker(timeMs)
                        {
                            var pos = chartView.mapToPosition(Qt.point(timeMs, 0))
                            timeMarker.x = pos.x
                            timeMarker.visible = true
                        }

                        Item
                        {
                            id: chartFrame
                            anchors.fill: parent
                            //visible: false

                            //background: Item { }
                            ChartView
                            {
                                id: chartView
                                anchors.fill: parent
                                z: depth
                                theme: ChartView.ChartThemeLight

                                DateTimeAxis
                                {
                                    id: timeAxis
                                    format: "hh:mm:ss"
                                    tickCount: 3
                                }

                                ValueAxis
                                {
                                    id: valueAxis
                                    tickCount: 3
                                    visible: true
                                }

                                axes: [timeAxis, valueAxis]
                                antialiasing: true
                                animationOptions: ChartView.NoAnimation

                                onWidthChanged:
                                {
                                    updateTimeMarker(markerTimeMs)
                                }

                                Connections
                                {
                                    target: parametersPlayer

                                    function onParameterPlayed(parameter)
                                    {
                                        var parameterLabel = parameter.label

                                        if(!chartHasLabel(labels, parameterLabel)) return

                                        var parameterValue = parameter.value
                                        var parameterDate = new Date(parameter.timestamp)
                                        var parameterTimeMsec = parameterDate.getTime()

                                        if(!isSeriesCreated(seriesMap, parameterLabel))
                                        {
                                            initializeAxes(parameter, timeAxis, valueAxis)

                                            seriesMap[parameterLabel] = chartView.createSeries(ChartView.SeriesTypeLine, parameterLabel, timeAxis, valueAxis)
                                            seriesMap[parameterLabel].useOpenGL = true
                                            //seriesMap[parameterLabel].color =
                                        }

                                        fitAxes(parameter, timeAxis, valueAxis, seriesMap[parameterLabel])

                                        insertForwardPlayedPoint(parameterTimeMsec, parameterValue, timeAxis, seriesMap[parameterLabel])

                                        updateTimeMarker(parameterTimeMsec)

                                        markerTimeMs = parameterTimeMsec
                                    }

                                    function onParameterBackPlayed(parameter)
                                    {
                                        var parameterLabel = parameter.label

                                        if(!chartHasLabel(labels, parameterLabel)) return

                                        var parameterValue = parameter.value
                                        var parameterDate = new Date(parameter.timestamp)
                                        var parameterTimeMsec = parameterDate.getTime()

                                        if(!isSeriesCreated(seriesMap, parameterLabel)) return

                                        fitAxes(parameter, timeAxis, valueAxis, seriesMap[parameterLabel])

                                        //
                                        insertBackPlayedPoint(parameterTimeMsec, parameterValue, timeAxis, seriesMap[parameterLabel])

                                        updateTimeMarker(parameterTimeMsec)

                                        markerTimeMs = parameterTimeMsec
                                    }
                                }
                            }

                            Rectangle
                            {
                                id: timeMarker
                                width: 7
                                border.width: 0
                                border.color: Qt.hsva(0.6, 0.2, 0.99, 0.85)
                                visible: false
                                z: 100
                                x: 0
                                y: chartView.plotArea.y
                                height: chartView.plotArea.height
                                color: Qt.hsva(0.6, 0.2, 0.99, 0.25)

                                Rectangle
                                {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                    width: 1
                                    color: "transparent"
                                    border.color: "red"
                                }
                            }
                        }

                        Rectangle
                        {
                            id: selectionRect
                            clip: true
                            anchors.fill: parent
                            color: selection ? Qt.hsva(0.6, 0.95, 0.85, 0.1) : "transparent"
                            radius: 2
                        }

                        Rectangle
                        {
                            id: hoverRect
                            anchors.fill: parent
                            color: hover ? Qt.hsva(0.15, 0.9, 0.7, 0.2) : "transparent"
                            radius: 2
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked:
                            {
                                //gridView.currentIndex = index
                                chartViewModel.selectElement(index, true)
                            }
                            onEntered:
                            {
                                chartViewModel.hoverElement(index)
                            }
                            onExited:
                            {
                                chartViewModel.clearHover()
                            }
                        }
                    }
                }
            }
        }
    }
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

    function insertBackPlayedPoint(parameterTimeMsec, parameterValue, timeAxis, series)
    {
        var timeMin = timeAxis.min
        var timeMax = timeAxis.max

        var firstPoint = series.at(0)
        var firstPointTime = firstPoint.x
        if(parameterTimeMsec < firstPointTime)
        {
            series.insert(0, parameterTimeMsec, parameterValue)
        }

        var lastPoint = series.at(series.count - 1)
        if(lastPoint.x > timeMax)
        {
            series.remove(series.count - 1)
        }
    }

    function insertForwardPlayedPoint(parameterTimeMsec, parameterValue, timeAxis, series)
    {
        if(series.count < 1)
        {
            series.append(parameterTimeMsec, parameterValue)

            return
        }

        var timeMin = timeAxis.min.getTime()
        var timeMax = timeAxis.max.getTime()

        var lastPoint = series.at(series.count - 1)
        var lastPointTime = lastPoint.x
        if(parameterTimeMsec > lastPointTime)
        {
            series.append(parameterTimeMsec, parameterValue)
        }

        while (series.count > 0 && series.at(0).x < timeMin)
        {
            series.remove(0)
        }
    }

    function chartHasLabel(chartLabels, label)
    {
        return chartLabels.indexOf(label) >= 0
    }

    function isSeriesCreated(seriesMap, label)
    {
        return label in seriesMap
    }

    function toggleParameter(label, color)
    {
        if(chartViewModel)
        {
            if(!chartViewModel.toggleParameter(label, color))
            {
                return

                for (let i = 0; i < tableView.count; i++)
                {
                    const chartView = tableView.itemAt(i);

                    if(chartView.seriesMap.hasOwnProperty(label))
                    {
                        var series = chartView.seriesMap[label]

                        chartView.removeSeries(series)

                        delete chartView.seriesMap.label
                    }
                }
            }

            chartsPanelTitle.visible = chartViewModel.countSeries() < 1// chartViewModel.isEmpty()
        }
    }
}
