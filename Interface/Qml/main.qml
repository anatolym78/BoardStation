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
    height: 960
      
    Component.onCompleted:
    {
        boardParametersList.parametersListModel = parametersListModel
        droneControlPanel.uplinkParametersModel = uplinkParametersModel
    }
    
    // Main frame
    Frame
    {
        anchors.fill: parent


        Rectangle
        {
            anchors.fill: parent
            //color:"#f7f7f7"
            radius: 4

            RowLayout
            {
                 anchors.fill: parent
                 spacing: 8

                // Отображает список параметров, полученных от дрона
                BoardParametersList
                {
                    id: boardParametersList
                    Layout.preferredWidth: 1.5
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    onParameterSelected:
                    {
                        simpleChartsPanel.toggleParameter(label, color)
                    }
                }

                ColumnLayout
                {
                    Layout.preferredWidth: 5
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 8

                    // Панель с графиками
                    Rectangle
                    {
                        Layout.preferredHeight: 3
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "gainsboro"
                        //border.color: "gainsboro"
                        radius: 4

                        SimpleChartsPanel
                        {
                             id: simpleChartsPanel
                             anchors.fill: parent
                             anchors.margins: 5
                        }
                    }

                    // Панель с отладкой/логами
                    Rectangle
                    {
                        Layout.preferredHeight: 1.5
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "white"
                        radius: 4

                        DebugLogsView
                        {
                            anchors.fill: parent
                            model: debugViewModel
                        }
                    }
                }

                 // Панель управления параметрами дрона
                 DroneControlPanel
                 {
                     id: droneControlPanel
                     Layout.preferredWidth: 2.5
                     Layout.fillWidth: true
                     Layout.fillHeight: true

                 }
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
    
    Binding
    {
        target: droneControlPanel
        property: "uplinkParametersModel"
        value: uplinkParametersModel
        when: uplinkParametersModel !== null
    }
}

