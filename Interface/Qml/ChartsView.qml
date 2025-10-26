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
                checked: true

                Connections
                {
                    target: qmlMainWindow
                    onWidthChanged:
                    {
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
                visible: chartModel.isCanMergeCharts
                Layout.alignment: Qt.AlignRight
            }

            Connections
            {
                target: chartModel
                onIsCanMergeChartsChanged:
                {
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
                columns: rowsCountSwitch.checked ? 2 : 1

                Repeater
                {
                    id: repeater
                    model: chartModel

                    delegate: Item
                    {
                        visible: true
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredWidth: calculatePreferedSize()
                        Layout.preferredHeight: width

                        function calculatePreferedSize()
                        {
                            return chartsView.width/ (rowsCountSwitch.checked ? 2 : 1)
                        }

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

                            ChartView
                            {
                                id: chartView
                                anchors.fill: parent
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
                                    target: chartModel
                                    function onParameterAdded(chartIndex, parameterLabel, parameterColor)
                                    {
                                        console.log(chartIndex)
                                        console.log(parameterLabel)
                                        if(chartIndex === index)
                                        {
                                            if(!chartModel.isSeriesCreated(parameterLabel))
                                            {
                                                var series = chartView.createSeries(ChartView.SeriesTypeLine, parameterLabel, timeAxis, valueAxis)
                                                chartModel.addSeriesToChart(index, parameterLabel, parameterColor, series, timeAxis, valueAxis)
                                                updateTimeMarker(parametersPlayer.currentPosition.getTime())
                                            }
                                        }
                                    }
                                }

                                Connections
                                {
                                    target: parametersPlayer
                                    function onCurrentPositionChanged()
                                    {
                                        updateTimeMarker(parametersPlayer.currentPosition.getTime())
                                    }
                                }
                            }

                            Rectangle
                            {
                                id: timeMarker
                                width: 7
                                border.width: 0
                                border.color: Qt.hsva(0.6, 0.2, 0.99, 0.85)
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
                                    border.color: "brown"
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
                            color: hover ? Qt.hsva(0.15, 0.7, 0.9, 0.1) : "transparent"
                            radius: 2
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked:
                            {
                                chartModel.selectElement(index, true)
                            }
                            onEntered:
                            {
                                chartModel.hoverElement(index)
                            }
                            onExited:
                            {
                                chartModel.clearHover()
                            }
                        }
                    }
                }
            }
        }
    }
}
