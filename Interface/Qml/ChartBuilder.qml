import QtQuick 2.12
import QtCharts 2.12

// Простой Chart Builder для создания базовых графиков
QtObject {
    id: chartBuilder
    
    // Создает простой график с двумя точками (0,0) и (1,1)
    function createChart(parameter) {
        console.log("ChartBuilder: Creating simple chart for parameter", parameter.label)
        
        // Создаем контейнер для графика
        var chartContainer = Qt.createQmlObject('
            import QtQuick 2.12
            import QtQuick.Controls 2.12
            import QtQuick.Layouts 1.12
            
            Rectangle {
                id: chartContainer
                width: 300
                height: 200
                border.color: "#aaaaaa"
                border.width: 1
                color: "white"
                
                property string parameterName: "' + parameter.label + '"
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 5
                    
                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: "Chart for " + chartContainer.parameterName
                            font.bold: true
                            Layout.fillWidth: true
                        }
                        Button {
                            text: "Remove"
                            onClicked: {
                                chartContainer.destroy()
                            }
                        }
                    }
                    
                    SimpleChart {
                        id: simpleChart
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        parameterName: chartContainer.parameterName
                    }
                }
            }
        ', chartsPanel)
        
        if (chartContainer) {
            console.log("ChartBuilder: Simple chart created successfully")
        } else {
            console.warn("ChartBuilder: Failed to create chart")
        }
        
        return chartContainer
    }
    
    // Всегда разрешаем создание графика
    function canCreateChart(parameter) {
        return true
    }
}

