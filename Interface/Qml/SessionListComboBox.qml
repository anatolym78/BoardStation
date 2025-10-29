import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import "./MaterialDesign.js" as MD

ComboBox
{
    id: comboBox
    //currentIndex: 1
    model: sessionsListModel

    //property var sessionName: null
    background: null
    contentItem: null
    //contentItem: null
    // contentItem: ItemDelegate
    // {
    //     Rectangle
    //     {
    //         anchors.fill: parent
    //         color: "aliceblue"
    //         radius: 8
    //     }
    // }

        // onCurrentIndexChanged: function(index)
        // {
        //     console.log(index)
        // }

    Component.onCompleted:
    {
        //sessionName = sessionName
    }

    delegate: ItemDelegate
    {
        width: comboBox.width
        highlighted: index === comboBox.currentIndex
        height: 100
        Rectangle
        {
            anchors.margins: 10
            anchors.fill: parent
            color: "transparent"
            Text
            {
                text: sessionName
            }
        }
    }

    indicator: Item
    {
        anchors.fill: parent
        implicitHeight: 64// parent.height

        Rectangle
        {
            anchors.fill: parent
            radius: 4
            color: "gold"
            RowLayout
            {
                anchors.fill: parent
                Text
                {
                    Layout.alignment: Qt.AlignLeft
                    // anchors.left: parent.left
                    // anchors.verticalCenter: parent.verticalCenter
                    text: model.get(comboBox.currentIndex).sessionName
                    font.pointSize: 12

                    Component.onCompleted:
                    {
                        console.log(comboBox.currentIndex)
                    }
                }

                // Item {
                //     id: name
                // }

                Text
                {
                    Layout.alignment: Qt.AlignRight

                    // anchors.right: parent.right
                    // anchors.verticalCenter: parent.verticalCenter
                    text: MD.icons.expand_more
                    font.pointSize: 24
                    color: "dimgray"
                }
            }


        }
    }
}
