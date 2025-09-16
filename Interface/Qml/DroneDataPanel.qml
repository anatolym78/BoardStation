import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle
{
    id: droneDataPanel

    color: "#f4f4f0"

    property var parametersModel: null
    
    // Сигнал для переключения графика
    signal toggleChart(int parameterIndex)
    
    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        
        Text
        {
            text: "Drone Data"
            font.pointSize: 14
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }
           
        TableView
        {
            id: parametersTableView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: false
            
            model: parametersModel

            // Component.onCompleted:
            // {
            //     console.log(parametersModel)
            // }
            
            // Data delegate
            delegate: Rectangle 
            {
                id: parameterCell
                implicitWidth: 85
                implicitHeight: 40
                border.width: 2
                border.color: "#f4f4f0"
                //color: row % 2 === 0 ? "#ffffff" : "#f8f8f8"
                
                Text 
                {
                    anchors.centerIn: parent
                    font.pointSize: 11
                    text: 
                    {
                        // Принудительно обновляем текст при изменении данных модели
                        if (parametersModel) 
                        {
                            return parametersModel.data(parametersModel.index(row, column), Qt.DisplayRole) || ""
                        }
                        return ""
                    }
                    elide: Text.ElideRight
                }

                Connections
                {
                    target: parametersModel
                    // function onParameterUpdated(label)
                    // {
                    //     parametersTableView.model = null
                    //     parametersTableView.model = parametersModel
                    // }
                }

                Component.onCompleted:
                {
                }
                
                states:
                [
                    State
                    {
                        name: "hovered"
                        PropertyChanges
                        {
                            target: parameterCell;
                            color: "#55ccff"
                        }
                    }
                ]

                transitions: Transition
                {
                    from: ""
                    to: "hovered"
                    ColorAnimation
                    {
                        duration: 250
                    }
                }

                MouseArea 
                {
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: 
                    {
                        // Переключаем график для этого параметра
                        toggleChart(row)

                        console.log(row)
                    }

                    onEntered:
                    {
                        if(column == 0)
                        {
                            parameterCell.state = "hovered"
                        }
                    }

                    onExited:
                    {
                        if(column == 0)
                        {
                            parameterCell.state = ""
                        }
                    }
                }
            }
        }
    }
}
