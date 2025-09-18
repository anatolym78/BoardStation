import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle
{
    id: droneControlPanel

    color: "#f0f0f0"  // Тот же фон, что и у левой панели
    
    property var outParametersModel: null
    
    // Сигнал для добавления графика
    signal addChart(string parameterLabel)
     
    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        
        Text
        {
            text: "Drone Control"
            font.pointSize: 12
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }
        
        TableView
        {
            id: outParametersTableView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            model: outParametersModel
            
            // Data delegate with custom controls
            delegate: Rectangle
            {
                implicitWidth: 150
                implicitHeight: 50
                border.width: 1
                border.color: "#e0e0e0"
                color: row % 2 === 0 ? "#ffffff" : "#f8f8f8"
                    
                // Different controls based on column
                Loader
                {
                    anchors.fill: parent
                    anchors.margins: 5
                    
                    sourceComponent: 
                    {
                        sourceComponent: switch(column)
                        {
                            case 0:
                                // Label column - just text
                                return labelComponent
                            case 1:
                                // Value column - text
                                return valueComponent
                            case 2:
                                // Control column - dynamic control based on parameter type
                                return dynamicControlComponent
                            default:
                                // Other columns - text
                                return textComponent
                        }

                    }
                }
                
                Component
                {
                    id: labelComponent
                    Text
                    {
                        anchors.centerIn: parent
                        text: label
                        font.pointSize: 10
                        elide: Text.ElideRight
                    }
                }
                
                Component
                {
                    id: valueComponent
                    Text
                    {
                        anchors.centerIn: parent
                        text: value
                        font.pointSize: 10
                        elide: Text.ElideRight
                    }
                }
                
                Component
                {
                    id: textComponent
                    Text
                    {
                        anchors.centerIn: parent
                        text: controlType
                        font.pointSize: 10
                        elide: Text.ElideRight
                    }
                }
                
                Component
                {
                    id: dynamicControlComponent
                    Item
                    {
                        //property var paramData: controlType
                        
                        Loader
                        {
                            anchors.fill: parent
                            sourceComponent: 
                            {
                                console.log(controlType)

                                return spinBoxComponent
                                // if (paramData && paramData.controlType)
                                // {
                                //     if (paramData.controlType === "QComboBox")
                                //     {
                                //         return comboBoxComponent
                                //     }
                                //     else if (paramData.controlType === "QSlider")
                                //     {
                                //         return sliderComponent
                                //     }
                                //     else if (paramData.controlType === "QSpinBox")
                                //     {
                                //         console.log("Spin box")

                                //         return spinBoxComponent
                                //     }
                                //     else if (paramData.controlType === "QCheckBox")
                                //     {
                                //         return checkBoxComponent
                                //     }
                                //     else
                                //     {
                                //         return textEditComponent
                                //     }
                                // }
                                // else
                                // {
                                //     return defaultComponent
                                // }
                            }
                        }
                        Component
                        {
                            id: spinBoxComponent
                            SpinBox
                            {
                                anchors.fill: parent
                                // from: paramData ? (paramData.values && paramData.values.length > 0 ? paramData.values[0] : 0) : 0
                                // to: paramData ? (paramData.values && paramData.values.length > 1 ? paramData.values[1] : 100) : 100
                                // stepSize: paramData ? (paramData.step ? paramData.step : 1) : 1  // Используем шаг из параметра
                                // value: paramData ? paramData.currentValue : 0
                                onValueChanged:
                                {
                                    //outParametersModel.setData(outParametersModel.index(row, column), value, Qt.EditRole)
                                    //outParametersModel.setData(outParametersModel.index(row, 1), value, Qt.EditRole)

                                    // Принудительно обновляем отображение
                                    //outParametersTableView.forceLayout()

                                    //console.log("spinbox value changed ", value)
                                }
                            }
                        }

                        // Component
                        // {
                        //     id: comboBoxComponent
                        //     ComboBox
                        //     {
                        //         anchors.fill: parent
                        //         model: paramData ? paramData.values : []
                        //         currentIndex:
                        //         {
                        //             if (paramData && paramData.values)
                        //             {
                        //                 var currentValue = paramData.currentValue
                        //                 for (var i = 0; i < paramData.values.length; i++)
                        //                 {
                        //                     if (paramData.values[i] === currentValue)
                        //                     {
                        //                         return i
                        //                     }
                        //                 }
                        //             }
                        //             return 0
                        //         }
                        //         onCurrentIndexChanged:
                        //         {
                        //             if (paramData && paramData.values && currentIndex >= 0)
                        //             {
                        //                 var newValue = paramData.values[currentIndex]
                        //                 outParametersModel.setData(outParametersModel.index(row, column), newValue, Qt.EditRole)
                        //                 // Принудительно обновляем отображение
                        //                 //outParametersTableView.forceLayout()
                        //             }
                        //         }
                        //     }
                        // }
                        
                        // Component
                        // {
                        //     id: sliderComponent
                        //     Slider
                        //     {
                        //         anchors.fill: parent
                        //         from: paramData ? (paramData.values && paramData.values.length > 0 ? paramData.values[0] : 0) : 0
                        //         to: paramData ? (paramData.values && paramData.values.length > 1 ? paramData.values[1] : 100) : 100
                        //         value: paramData ? paramData.currentValue : 0
                        //         onValueChanged:
                        //         {
                        //             outParametersModel.setData(outParametersModel.index(row, column), value, Qt.EditRole)
                        //             console.log("slider value changed")
                        //             // Принудительно обновляем отображение
                        //             //outParametersTableView.forceLayout()
                        //         }
                        //     }
                        // }
                        
                        // Component
                        // {
                        //     id: spinBoxComponent
                        //     SpinBox
                        //     {
                        //         anchors.fill: parent
                        //         from: paramData ? (paramData.values && paramData.values.length > 0 ? paramData.values[0] : 0) : 0
                        //         to: paramData ? (paramData.values && paramData.values.length > 1 ? paramData.values[1] : 100) : 100
                        //         stepSize: paramData ? (paramData.step ? paramData.step : 1) : 1  // Используем шаг из параметра
                        //         value: paramData ? paramData.currentValue : 0
                        //         onValueChanged:
                        //         {
                        //             //outParametersModel.setData(outParametersModel.index(row, column), value, Qt.EditRole)
                        //             //outParametersModel.setData(outParametersModel.index(row, 1), value, Qt.EditRole)

                        //             // Принудительно обновляем отображение
                        //             //outParametersTableView.forceLayout()

                        //             console.log("spinbox value changed ", value)
                        //         }
                        //     }
                        // }
                        
                        // Component
                        // {
                        //     id: checkBoxComponent
                        //     CheckBox
                        //     {
                        //         anchors.centerIn: parent
                        //         checked: paramData ? paramData.currentValue : false
                        //         onCheckedChanged:
                        //         {
                        //             outParametersModel.setData(outParametersModel.index(row, column), checked, Qt.EditRole)
                        //             outParametersModel.setData(outParametersModel.index(row, 1), checked, Qt.EditRole)
                        //             console.log("checkbox value changed")
                        //             console.log(checked)
                        //             console.log(row)
                        //             console.log(column)
                        //             // Принудительно обновляем отображение
                        //             //outParametersTableView.forceLayout()
                        //         }
                        //     }
                        // }
                        
                        // Component
                        // {
                        //     id: textEditComponent
                        //     TextField {
                        //         anchors.fill: parent
                        //         text: paramData ? paramData.currentValue : ""
                        //         onTextChanged:
                        //         {
                        //             outParametersModel.setData(outParametersModel.index(row, column), text, Qt.EditRole)
                        //             // Принудительно обновляем отображение
                        //             //outParametersTableView.forceLayout()
                        //         }
                        //     }
                        // }
                        
                        // Component
                        // {
                        //     id: defaultComponent
                        //     Text {
                        //         anchors.centerIn: parent
                        //         text: "No control"
                        //         color: "#666666"
                        //     }
                        // }
                    }
                }
            }
        }
        
        // Кнопки управления (перемещены под таблицу)
        RowLayout
        {
            Layout.fillWidth: true
            spacing: 10
            
            Button
            {
                text: "Send to drone"
                Layout.preferredWidth: 120
                onClicked: 
                {
                    // Вызываем C++ метод через объект QmlMainWindow
                    if (qmlMainWindow) 
                    {
                        qmlMainWindow.sendParametersToBoard()
                    }
                }
            }
            
            CheckBox
            {
                text: "Send immediately"
                checked: false
                onCheckedChanged: 
                {
                    // Здесь будет логика автоматической отправки
                }
            }
            
            Item
            {
                Layout.fillWidth: true
            }
        }
    }
}
