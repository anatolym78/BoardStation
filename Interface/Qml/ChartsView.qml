import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.15

Rectangle
{
    id: chartsView
    color: "aliceblue"
    radius: 4
    clip: true

    ListView
    {
        model: chartViewModel
        id: repeater
        orientation: ListView.Vertical
        anchors.fill: parent
        spacing: 10

        Component.onCompleted:
        {
            console.log(chartViewModel)
        }

        delegate: Rectangle
        {
            //width: 45
            height: 45
            color: "red"
        }
    }
}
