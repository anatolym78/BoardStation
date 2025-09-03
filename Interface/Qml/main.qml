import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import QtCharts 2.12
import BoardStation 1.0

Item
{
    id: mainWindow
    width: 1280
    height: 800
    
    // Chart Builder для создания графиков
    ChartBuilder {
        id: chartBuilder
    }
    
    Component.onCompleted:
    {
        console.log("QML: Application loaded")
        console.log("QML: parametersModel available:", parametersModel !== null)
        console.log("QML: outParametersModel available:", outParametersModel !== null)
        console.log("QML: chartSeriesModel available:", chartSeriesModel !== null)
        if (parametersModel)
        {
            console.log("QML: Number of parameters in model:", parametersModel.rowCount())
            console.log("QML: Number of columns in model:", parametersModel.columnCount())
        }
        if (outParametersModel)
        {
            console.log("QML: Number of out parameters in model:", outParametersModel.rowCount())
            console.log("QML: Number of out parameter columns in model:", outParametersModel.columnCount())
        }
        if (chartSeriesModel)
        {
            console.log("QML: Number of chart series in model:", chartSeriesModel.rowCount())
        }
        
        // Явно присваиваем модели компонентам для избежания циклических привязок
        droneDataPanel.parametersModel = parametersModel
        droneControlPanel.outParametersModel = outParametersModel
        chartsPanel.parametersModel = parametersModel
        chartsPanel.chartSeriesModel = chartSeriesModel
        chartsPanel.chartBuilder = chartBuilder
    }
    
    // Main frame
    Frame
    {
        anchors.fill: parent

         RowLayout
         {
             anchors.fill: parent
             spacing: 0

             // Drone data panel - отображает данные, полученные от дрона
             DroneDataPanel
             {
                 id: droneDataPanel
                 Layout.preferredWidth: 1
                 Layout.fillWidth: true
                 Layout.fillHeight: true
                 onToggleChart: {
                     chartsPanel.toggleChart(parameterIndex)
                 }
             }

             ChartsPanel
             {
                 id: chartsPanel
                 Layout.preferredWidth: 2
                 Layout.fillWidth: true
                 Layout.fillHeight: true

             }

             // Drone control panel - управляющие параметры, отправляемые дрону
             DroneControlPanel
             {
                 id: droneControlPanel
                 Layout.preferredWidth: 1
                 Layout.fillWidth: true
                 Layout.fillHeight: true
                 //Layout.minimumWidth: 250
             }
         }
    }

    
    // Альтернативные привязки с использованием Binding (если нужно)
    Binding
    {
        target: droneDataPanel
        property: "parametersModel"
        value: parametersModel
        when: parametersModel !== null
    }
    
    Binding
    {
        target: droneControlPanel
        property: "outParametersModel"
        value: outParametersModel
        when: outParametersModel !== null
    }
}

