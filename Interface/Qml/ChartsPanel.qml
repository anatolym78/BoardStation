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
        spacing: 10

        Repeater
        {
            model: chartsListModel

            // График для серии
            delegate: ChartView
            {
                width: 240
                height: 240
                theme: ChartView.ChartThemeBlueIcy
                ValueAxis
                {
                    id: parameterValueAxis
                    min: -750
                    max: 750
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
                            if (pointsModel)
                            {
                                var color = pointsModel.pointsColor()
                                chartSeries.color = color
                            }
                        }
                        else
                        {
                            chartSeries = series(0)
                            if (pointsModel)
                            {
                                chartSeries.append(pointsModel.elapsedTime(), pointsModel.lastY())
                            }
                        }
                    }
                }

                DragHandler
                {
                    id: dragHandler
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
                Drag.active: dragHandler.active

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
