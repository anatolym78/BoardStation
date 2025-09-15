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
            id: repeater

            // График для серии
            delegate: ChartView
            {
                id: chartView
                property var seriesModel: model.seriesModel
                property var chartIndex: model.index
                width: 320
                height: 240
                theme: ChartView.ChartThemeBlueIcy
                title: name
                property bool highlight: false
                opacity: highlight ? 0.8 : 1.0
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

                function mergeSeries(targetSeries, draggedSeries)
                {

                }

                DragHandler
                {
                    id: dragHandler
                    onActiveChanged:
                    {
                        if(active)
                        {
                            // Начало перетаскивания
                            chartView.Drag.start()
                        }
                        else
                        {
                            // Завершение перетаскивания
                            chartView.Drag.drop()
                        }
                    }
                }

                Drag.active: dragHandler.active
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2
                Drag.mimeData:
                {
                    "text/plain": "dragged-object",  // Указываем тип данных
                    "application/x-series-model": seriesModel, // Передаем модель
                    "model-index": index // Передаем индекс в repeater
                }
                Drag.dragType: Drag.Automatic

                DropArea
                {
                    id: dropArea
                    anchors.fill: parent
                    enabled: true

                    onEntered:
                    {
                        //console.log(dropArea.parent)
                    }

                    onExited:
                    {
                        //console.log("Exited")
                    }

                    onDropped:
                    {
                        if(chartsListModel.rowCount() < 2) return

                        console.log("dropped chart index: ", drop.source.chartIndex)
                        console.log("target chart index: ", chartView.chartIndex)

                        var targetSeries = chartView.seriesModel

                        var droppedSeries = drop.source.seriesModel

                        for(var i=0;i<droppedSeries.countSeries();i++)
                        {
                            var pointsModel = droppedSeries.getPointsModel(i)
                            console.log(pointsModel.parameterLabel())

                            var chartSeries = chartView.series(0)

                            for(var j=0;j<pointsModel.countPoints();j++)
                            {

                            }

                            console.log(chartSeries)
                        }

                        //mergeSeries()

                        chartsListModel.removeSeries(drop.source.chartIndex)
                    }
                }
            }
        }
    }
    //}
}
