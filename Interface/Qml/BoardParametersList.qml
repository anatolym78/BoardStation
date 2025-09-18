import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle
{
    id: boardParemetersList

    color: "#f0f0f0"

    property var parametersListModel: null

    signal parameterSelected(string label)

    ColumnLayout
    {

        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Text
        {
            text: "Drone Data"
            font.pointSize: 12
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        ListView
        {
            id: parametersListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
            clip: false

            model: parametersListModel

            Component.onCompleted:
            {
            }

            // Data delegate
            delegate: Rectangle
            {
                id: parameterCell
                implicitWidth: 150
                implicitHeight: 40
                border.width: 2
                border.color: "transparent"
                //border.color: "#f4f4f0"
                color: "transparent"

                RowLayout
                {
                    anchors.fill: parent
                    spacing: 2

                    Rectangle
                    {
                        width: 75
                        height: 28
                        color: "midnightblue"
                        anchors.margins: 4
                        Text
                        {
                            anchors.centerIn: parent
                            //anchors.margins: 4
                            text: label
                            font.pointSize: 11
                            color: "white"
                        }
                    }

                    Rectangle
                    {
                        width: 65
                        height: 28
                        color: "gold"
                        anchors.margins: 4
                        Text
                        {
                            anchors.centerIn: parent
                            //anchors.margins: 4
                            text: value + " " + unit
                            font.pointSize: 11
                        }
                    }
                }



                // Connections
                // {
                //     target: parametersModel
                //     function onParameterUpdated(label)
                //     {

                //     }
                // }

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
                            color: "#77ddff"
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
                        parameterSelected(label)
                    }

                    onEntered:
                    {
                        parameterCell.state = "hovered"
                    }

                    onExited:
                    {
                        parameterCell.state = ""

                    }
                }
            }
        }

    }


}
