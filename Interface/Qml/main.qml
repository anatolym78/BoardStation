import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import QtCharts 2.12
import BoardStation 1.0

Item
{
    id: mainWindow
    width: 1400
    height: 800
      
    Component.onCompleted:
    {
        // Явно присваиваем модели компонентам для избежания циклических привязок
        droneDataPanel.parametersListModel = parametersListModel
        droneControlPanel.outParametersModel = outParametersModel
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
            BoardParametersList
            {
                id: droneDataPanel
                Layout.preferredWidth: 1.5
                Layout.fillWidth: true
                Layout.fillHeight: true

                onParameterSelected:
                {
                    chartsListModel.addSeries(label)
                }
            }

             ChartsPanel
             {
                 id: chartsPanel
                 //anchors.fill: parent
                 Layout.preferredWidth: 5
                 Layout.fillWidth: true
                 Layout.fillHeight: true

             }

             // Drone control panel - управляющие параметры, отправляемые дрону
             DroneControlPanel
             {
                 id: droneControlPanel
                 Layout.preferredWidth: 3.5
                 Layout.fillWidth: true
                 Layout.fillHeight: true
                 
             }
         }
    }

    
    // // Альтернативные привязки с использованием Binding (если нужно)
    // Binding
    // {
    //     target: droneDataPanel
    //     property: "parametersListModel"
    //     value: parametersModel
    //     when: parametersModel !== null
    // }
    
    Binding
    {
        target: droneControlPanel
        property: "outParametersModel"
        value: outParametersModel
        when: outParametersModel !== null
    }
}

