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
    height: 480
      
    Component.onCompleted:
    {
        boardParametersList.parametersListModel = parametersListModel
        //droneControlPanel.outParametersModel = outParametersModel
    }
    
    // Main frame
    Frame
    {
        anchors.fill: parent


        Rectangle
        {

            anchors.fill: parent
            color:"#f7f7f7"
            radius: 4

            RowLayout
             {
                 anchors.fill: parent
                 spacing: 0

                // Отображает список параметров, полученных от дрона
                BoardParametersList
                {
                    id: boardParametersList
                    Layout.preferredWidth: 1.5
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    onParameterSelected:
                    {
                        // chartsListModel.toggleSeries(label, color)
                        // Пока просто добавляем график с новым параметром
                        simpleChartsPanel.addChart("График " + label)
                    }
                }

                // Панель с графиками
                // ChartsPanel
                // {
                //      id: chartsPanel
                //      //anchors.fill: parent
                //      Layout.preferredWidth: 5
                //      Layout.fillWidth: true
                //      Layout.fillHeight: true

                // }

                // Упрощенная панель с графиками
                SimpleChartsPanel
                {
                     id: simpleChartsPanel
                     Layout.preferredWidth: 5
                     Layout.fillWidth: true
                     Layout.fillHeight: true
                }

                 // // Панель управления параметрами дрона
                 // DroneControlPanel
                 // {
                 //     id: droneControlPanel
                 //     Layout.preferredWidth: 2.5
                 //     Layout.fillWidth: true
                 //     Layout.fillHeight: true

                 // }
             }

        }

 }

    
    // Альтернативные привязки с использованием Binding (если нужно)
    Binding
    {
        target: boardParametersList
        property: "parametersListModel"
        value: boardParametersList
        when: boardParametersList !== null
    }
    
    // Binding
    // {
    //     target: droneControlPanel
    //     property: "outParametersModel"
    //     value: outParametersModel
    //     when: outParametersModel !== null
    // }
}

