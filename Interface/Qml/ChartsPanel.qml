import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.15

Item
{
    id: chartsPanel
    width: parent.width

    // Список активных графиков
    property var activeCharts: []
    property var parametersModel: null
    property var chartSeriesModel: null
    property var chartBuilder: null
    
    // Функция для переключения графика параметра
    function toggleChart(parameterIndex) {
        if (!parametersModel || !chartSeriesModel) {
            console.log("ChartsPanel: Models not available")
            return
        }
        
        var parameterLabel = parametersModel.data(parametersModel.index(parameterIndex, 0), Qt.DisplayRole)
        console.log("ChartsPanel: Toggling chart for parameter:", parameterLabel)
        
        if (chartSeriesModel.hasSeries(parameterLabel)) {
            // Удаляем серию
            chartSeriesModel.removeChartSeries(parameterLabel)
            console.log("ChartsPanel: Removed chart series for:", parameterLabel)
        } else {
            // Добавляем серию
            chartSeriesModel.addChartSeries(parameterLabel)
            console.log("ChartsPanel: Added chart series for:", parameterLabel)
        }
    }
    
    ColumnLayout
    {
        anchors.fill: parent
        spacing: 10
        
        Text
        {
            text: "Charts"
            font.pointSize: 14
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }
        
        ScrollView
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            

            Flow
            {
                id: chartsColumn
                width: parent.width
                spacing: 10

                Repeater
                {
                    model : chartSeriesModel
                    ChartView
                    {
                        width: 400
                        height: 300
                        antialiasing: true
                        theme: ChartView.ChartThemeQt

                        LineSeries 
                        {
                            name: "Speed"
                            color: "red"
                            width: 2
                            XYPoint { x: 0; y: 0 }
                            XYPoint { x: 1; y: 1 }
                        }
                                                
                        LineSeries 
                        {
                            name: "Altitude"
                            color: "blue"
                            width: 2
                            XYPoint { x: 0; y: 0.7 }
                            XYPoint { x: 1; y: 1 }
                        }
                    }
                }
            }
        }
    }
}
