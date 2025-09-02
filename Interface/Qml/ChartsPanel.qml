import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.15

Item {
    id: chartsPanel

    // Список активных графиков
    property var activeCharts: []
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        
        Text {
            text: "Charts"
            font.pointSize: 14
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }
        
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            ColumnLayout {
                id: chartsColumn
                width: parent.width
                spacing: 10


                ChartView {
                    width: 400
                    height: 300
                    theme: ChartView.ChartThemeQt

                    LineSeries {
                        name: "Высота"
                        XYPoint { x: 0; y: 0 }
                        XYPoint { x: 1; y: 2 }
                        XYPoint { x: 2; y: 3 }
                    }
                }
                // Здесь будут добавляться графики
            }
        }
    }
}
