import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.15

Item
{
    id: chartsPanel

    // ScrollView
    // {
    //     Layout.fillWidth: true
    //     Layout.fillHeight: true
    //     //clip: true
    //     anchors.fill: parent

    Flow
    {
        id: chartsColumn
        //width: parent.width
        anchors.fill: parent
        spacing: 25

        Repeater
        {
            model: chartsListModel

            // График для серии
            delegate: ChartView
            {
                width: 360
                height: 240
                theme: ChartView.ChartThemeBlueIcy
                ValueAxis
                {
                    id: parameterValueAxis
                    min: -750
                    max: 700
                }

                ValueAxis
                {
                    id: timeAxis
                    min: 0
                    max: 100
                }

                axes: [parameterValueAxis, timeAxis]

                antialiasing: true

                Connections
                {
                    target: seriesModel

                    function onParameterValueAdded(label)
                    {
                        var chartSeries
                        var pointsModel = seriesModel.getPointsModel(label)
                        if(count == 0)
                        {
                            chartSeries = createSeries(ChartView.SeriesTypeLine, label, timeAxis, parameterValueAxis)
                            chartSeries.color = pointsModel.color2()

                            console.log(pointsModel.color2())
                        }
                        else
                        {
                            chartSeries = series(0)
                            chartSeries.append(pointsModel.elapsedTime(), pointsModel.lastY())
                        }
                    }
                }

                DragHandler
                {
                    onActiveChanged:
                    {
                        if(active)
                        {

                        }
                        else
                        {

                        }
                    }
                }

                Drag.hotSpot.x: parent.width/2
                Drag.hotSpot.y: parent.height/2
                Drag.active: DragHandler.active

                DropArea
                {
                    onDropped:
                    {

                    }
                }
            }
        }
    }
    //}
}
