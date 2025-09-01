import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: droneControlPanel
    Layout.preferredWidth: 500
    Layout.fillHeight: true
    color: "#f0f0f0"  // Тот же фон, что и у левой панели
    border.color: "#cccccc"
    
    property var outParametersModel: null
    
    // Таймер для принудительного обновления таблицы
    Timer {
        id: refreshTimer
        interval: 1000 // Обновляем каждую секунду
        repeat: true
        running: true
        onTriggered: {
            if (outParametersTableView && outParametersModel) {
                // Принудительно обновляем модель
                outParametersTableView.model = null
                outParametersTableView.model = outParametersModel
            }
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        
        Text {
            text: "Drone Control"
            font.pointSize: 14
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Dynamic headers for out parameters
        Row {
            Layout.fillWidth: true
            spacing: 0
            
            Repeater {
                model: outParametersModel ? outParametersModel.columnCount() : 0
                
                Rectangle {
                    width: 150
                    height: 30
                    color: "#e0e0e0"
                    border.width: 1
                    border.color: "#cccccc"
 
                    Text {
                        anchors.centerIn: parent
                        text: outParametersModel ? outParametersModel.headerData(index, Qt.Horizontal, Qt.DisplayRole) : ""
                        font.bold: true
                    }
                }
            }
        }
        
        TableView {
            id: outParametersTableView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            model: outParametersModel
            
            // Data delegate with custom controls
            delegate: Rectangle {
                implicitWidth: 150
                implicitHeight: 50
                border.width: 1
                border.color: "#e0e0e0"
                color: row % 2 === 0 ? "#ffffff" : "#f8f8f8"
                
                // Different controls based on column
                Loader {
                    anchors.fill: parent
                    anchors.margins: 5
                    
                    sourceComponent: {
                        if (column === 0) {
                            // Label column - just text
                            return labelComponent
                        } else if (column === 1) {
                            // Value column - text
                            return valueComponent
                        } else if (column === 2) {
                            // Control column - dynamic control based on parameter type
                            return dynamicControlComponent
                        } else {
                            // Other columns - text
                            return textComponent
                        }
                    }
                }
                
                Component {
                    id: labelComponent
                    Text {
                        anchors.centerIn: parent
                        text: outParametersModel ? outParametersModel.data(outParametersModel.index(row, column), Qt.DisplayRole) : ""
                        font.bold: true
                        elide: Text.ElideRight
                    }
                }
                
                Component {
                    id: valueComponent
                    Text {
                        anchors.centerIn: parent
                        text: {
                            // Принудительно обновляем текст при изменении данных модели
                            if (outParametersModel) {
                                return outParametersModel.data(outParametersModel.index(row, column), Qt.DisplayRole)
                            }
                            return ""
                        }
                        elide: Text.ElideRight
                    }
                }
                
                Component {
                    id: textComponent
                    Text {
                        anchors.centerIn: parent
                        text: outParametersModel ? outParametersModel.data(outParametersModel.index(row, column), Qt.DisplayRole) : ""
                        elide: Text.ElideRight
                    }
                }
                
                Component {
                    id: dynamicControlComponent
                    Item {
                        property var paramData: outParametersModel ? outParametersModel.data(outParametersModel.index(row, column), Qt.UserRole) : null
                        
                        Loader {
                            anchors.fill: parent
                            sourceComponent: {
                                if (paramData && paramData.controlType) {
                                    console.log("Creating control for type:", paramData.controlType)
                                    if (paramData.controlType === "QComboBox") {
                                        return comboBoxComponent
                                    } else if (paramData.controlType === "QSlider") {
                                        return sliderComponent
                                    } else if (paramData.controlType === "QSpinBox") {
                                        return spinBoxComponent
                                    } else if (paramData.controlType === "QCheckBox") {
                                        return checkBoxComponent
                                    } else {
                                        return textEditComponent
                                    }
                                } else {
                                    return defaultComponent
                                }
                            }
                        }
                        
                        Component {
                            id: comboBoxComponent
                            ComboBox {
                                anchors.fill: parent
                                model: paramData ? paramData.values : []
                                currentIndex: {
                                    if (paramData && paramData.values) {
                                        var currentValue = paramData.currentValue
                                        for (var i = 0; i < paramData.values.length; i++) {
                                            if (paramData.values[i] === currentValue) {
                                                return i
                                            }
                                        }
                                    }
                                    return 0
                                }
                                onCurrentIndexChanged: {
                                    if (paramData && paramData.values && currentIndex >= 0) {
                                        var newValue = paramData.values[currentIndex]
                                        console.log("ComboBox value changed to:", newValue)
                                        outParametersModel.setData(outParametersModel.index(row, column), newValue, Qt.EditRole)
                                        // Принудительно обновляем отображение
                                        outParametersTableView.forceLayout()
                                    }
                                }
                            }
                        }
                        
                        Component {
                            id: sliderComponent
                            Slider {
                                anchors.fill: parent
                                from: paramData ? (paramData.values && paramData.values.length > 0 ? paramData.values[0] : 0) : 0
                                to: paramData ? (paramData.values && paramData.values.length > 1 ? paramData.values[1] : 100) : 100
                                value: paramData ? paramData.currentValue : 0
                                onValueChanged: {
                                    console.log("Slider value changed to:", value)
                                    outParametersModel.setData(outParametersModel.index(row, column), value, Qt.EditRole)
                                    // Принудительно обновляем отображение
                                    outParametersTableView.forceLayout()
                                }
                            }
                        }
                        
                        Component {
                            id: spinBoxComponent
                            SpinBox {
                                anchors.fill: parent
                                from: paramData ? (paramData.values && paramData.values.length > 0 ? paramData.values[0] : 0) : 0
                                to: paramData ? (paramData.values && paramData.values.length > 1 ? paramData.values[1] : 100) : 100
                                stepSize: paramData ? (paramData.step ? paramData.step : 1) : 1  // Используем шаг из параметра
                                value: paramData ? paramData.currentValue : 0
                                onValueChanged: {
                                    console.log("SpinBox value changed to:", value)
                                    outParametersModel.setData(outParametersModel.index(row, column), value, Qt.EditRole)
                                    // Принудительно обновляем отображение
                                    outParametersTableView.forceLayout()
                                }
                            }
                        }
                        
                        Component {
                            id: checkBoxComponent
                            CheckBox {
                                anchors.centerIn: parent
                                checked: paramData ? paramData.currentValue : false
                                onCheckedChanged: {
                                    console.log("CheckBox value changed to:", checked)
                                    outParametersModel.setData(outParametersModel.index(row, column), checked, Qt.EditRole)
                                    // Принудительно обновляем отображение
                                    outParametersTableView.forceLayout()
                                }
                            }
                        }
                        
                        Component {
                            id: textEditComponent
                            TextField {
                                anchors.fill: parent
                                text: paramData ? paramData.currentValue : ""
                                onTextChanged: {
                                    console.log("TextField value changed to:", text)
                                    outParametersModel.setData(outParametersModel.index(row, column), text, Qt.EditRole)
                                    // Принудительно обновляем отображение
                                    outParametersTableView.forceLayout()
                                }
                            }
                        }
                        
                        Component {
                            id: defaultComponent
                            Text {
                                anchors.centerIn: parent
                                text: "No control"
                                color: "#666666"
                            }
                        }
                    }
                }
            }
        }
        
        // Кнопки управления (перемещены под таблицу)
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Button {
                text: "Send to drone"
                Layout.preferredWidth: 120
                onClicked: {
                    console.log("Send to drone button clicked")
                    // Здесь будет логика отправки параметров дрону
                }
            }
            
            CheckBox {
                text: "Send immediately"
                checked: false
                onCheckedChanged: {
                    console.log("Send immediately changed to:", checked)
                    // Здесь будет логика автоматической отправки
                }
            }
            
            Item {
                Layout.fillWidth: true
            }
        }
    }
}
