import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import BoardStation 1.0

Rectangle {
    id: boardParametersTree
    radius: 4
    color: "#f0f0f0"

    property var parametersModel: null

    signal parameterSelected(string label, variant color)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Text {
            text: qsTr("Drone Parameters")
            font.pointSize: 12
            font.bold: true
            color: "gray"
            Layout.alignment: Qt.AlignHCenter
        }

        TreeView {
            id: parametersTreeView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            model: parametersModel

            delegate: ItemDelegate {
                width: parametersTreeView.width
                height: 40
                
                readonly property bool isBranch: styleData.hasChildren
                
                background: Rectangle {
                    color: "transparent"
                }

                contentItem: RowLayout {
                    id: rowLayout
                    spacing: 0
                    Rectangle { width: 20 * styleData.depth; height: 1; color: "transparent"}

                    Rectangle {
                        width: 16; height: 16
                        color: "transparent"
                        visible: isBranch
                        Layout.alignment: Qt.AlignVCenter
                        Text {
                            text: styleData.isExpanded ? "▼" : "►"
                            anchors.centerIn: parent
                            font.pointSize: 10
                            color: "dimgray"
                        }
                    }

                    Rectangle {
                        id: labelCell
                        radius: 2
                        width: 120
                        Layout.fillHeight: true
                        Layout.margins: 4
                        color: Qt.hsva(0.6, 0.3, 0.4, 0.7)
                        Text {
                            anchors.centerIn: parent
                            text: model.label
                            font.pointSize: 11
                            color: "white"
                        }
                    }

                    Rectangle {
                        id: valueCell
                        radius: 2
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.margins: 4
                        color: "white"
                        visible: !isBranch

                        Text {
                            id: valueText
                            anchors.centerIn: parent
                            color: "dimgray"
                            text: {
                                var valStr = "";
                                if (typeof model.value === 'number') {
                                    valStr = model.value.toLocaleString(Qt.locale(), 'f', 2)
                                } else {
                                    valStr = model.value
                                }
                                return valStr + (model.unit ? " " + model.unit : "")
                            }
                            font.pointSize: 11
                        }

                        Rectangle {
                            id: statusRect
                            radius: 2
                            width: 4
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.rightMargin: 4
                            anchors.topMargin: 8
                            anchors.bottomMargin: 8
                            opacity: 0.75
                            color: model.chartVisibility ? model.parameterColor : "transparent"
                        }
                    }
                }

                hoverEnabled: true
                onHoveredChanged: {
                    labelCell.color = hovered ? Qt.hsva(0.6, 0.99, 0.99, 1.0) : Qt.hsva(0.6, 0.3, 0.4, 0.7)
                    valueText.color = hovered ? "midnightblue" : "dimgray"
                }
                
                onClicked: {
                    if (isBranch) {
                        styleData.isExpanded = !styleData.isExpanded
                    } else {
                        parameterSelected(model.fullPath, model.parameterColor)
                        model.chartVisibility = !model.chartVisibility
                    }
                }
            }
        }
    }
}
