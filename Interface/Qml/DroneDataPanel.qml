import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle
{
    id: droneDataPanel
    Layout.preferredWidth: 500
    Layout.fillHeight: true
    color: "#f0f0f0"
    border.color: "#cccccc"
    
    property var parametersModel: null
    
    // Сигнал для переключения графика
    signal toggleChart(int parameterIndex)
    
    // Таймер для принудительного обновления таблицы
    Timer {
        id: refreshTimer
        interval: 1000 // Обновляем каждую секунду
        repeat: true
        running: true
        onTriggered: {
            if (parametersTableView && parametersModel) {
                // Принудительно обновляем модель
                parametersTableView.model = null
                parametersTableView.model = parametersModel
            }
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        
        Text {
            text: "Drone Data"
            font.pointSize: 14
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Dynamic headers from model
        Row {
            Layout.fillWidth: true
            spacing: 0
            
            Repeater {
                model: parametersModel ? parametersModel.columnCount() : 0
                
                Rectangle {
                    width: 120
                    height: 30
                    color: "#e0e0e0"
                    border.width: 1
                    border.color: "#cccccc"
 
                    Text {
                        anchors.centerIn: parent
                        text: parametersModel ? parametersModel.headerData(index, Qt.Horizontal, Qt.DisplayRole) : ""
                        font.bold: true
                    }
                }
            }
        }
        
        TableView {
            id: parametersTableView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            model: parametersModel
            
            // Data delegate
            delegate: Rectangle {
                implicitWidth: 120
                implicitHeight: 40
                border.width: 1
                border.color: "#e0e0e0"
                color: row % 2 === 0 ? "#ffffff" : "#f8f8f8"
                
                Text {
                    anchors.centerIn: parent
                    text: {
                        // Принудительно обновляем текст при изменении данных модели
                        if (parametersModel) {
                            return parametersModel.data(parametersModel.index(row, column), Qt.DisplayRole) || ""
                        }
                        return ""
                    }
                    elide: Text.ElideRight
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        var value = parametersModel ? parametersModel.data(parametersModel.index(row, column), Qt.DisplayRole) : ""
                        console.log("Cell clicked:", row, column, "Value:", value)
                        
                        // Переключаем график для этого параметра
                        if (column === 0) { // Клик по названию параметра
                            console.log("Toggling chart for parameter row:", row)
                            toggleChart(row)
                        }
                    }
                }
            }
        }
    }
}
