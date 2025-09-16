import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.15

Item
{
    id: chartsPanel
    
    // Функция для добавления нового графика
    function addChart(parameterLabel) 
    {
        console.log("ChartsPanel: Adding chart for parameter:", parameterLabel)
        if (chartsListModel) 
        {
            chartsListModel.addSeries(parameterLabel)
            console.log("ChartsPanel: Chart added successfully for", parameterLabel)
        }
        else 
        {
            console.warn("ChartsPanel: chartsListModel is not available")
        }
    }

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
                Component.onCompleted:
                {

                    //console.log(z)
                }

                id: chartView
                property var seriesModel: model.seriesModel
                property var chartIndex: model.index
                width: 320
                height: 240
                theme: ChartView.ChartThemeBlueIcy
                //title: name
                property bool highlight: false
                opacity: highlight ? 0.8 : 1.0
                z: depth
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
                    max: 50
                }

                axes: [parameterValueAxis, timeAxis]

                antialiasing: true



                // Карта для хранения серий по меткам параметров
                property var seriesMap: ({})

                Connections
                {
                    target: seriesModel

                    function onParameterValueAdded(label)
                    {
                        var chartSeries
                        var pointsModel = seriesModel.getPointsModel(label)

                        // Проверяем, есть ли уже серия для данной метки
                        if (!seriesMap.hasOwnProperty(label))
                        {
                            // Создаем новую серию для этой метки
                            chartSeries = createSeries(ChartView.SeriesTypeLine, label, timeAxis, parameterValueAxis)
                            seriesMap[label] = chartSeries
                            
                            if (pointsModel)
                            {
                                var color = pointsModel.pointsColor()
                                chartSeries.color = color
                            }
                        }
                        else
                        {
                            // Используем существующую серию для этой метки
                            chartSeries = seriesMap[label]
                        }
                        
                        // Добавляем точку в соответствующую серию
                        if (pointsModel && chartSeries)
                        {
                            chartSeries.append(pointsModel.elapsedTime(), pointsModel.lastY())
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

                        var sourceIndex = drop.source.chartIndex
                        var targetIndex = chartView.chartIndex

                        console.log(sourceIndex)
                        console.log(targetIndex)

                        // Используем новый метод mergeSeries из ChartsListModel
                        chartsListModel.mergeSeries(targetIndex, sourceIndex)
                        
                        // Очищаем карту серий для пересоздания после слияния
                        chartView.seriesMap = ({})
                        
                        // console.log("Series merged successfully")
                    }
                }
            }
        }
    }
    //}
}
