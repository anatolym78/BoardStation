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

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 40
        TextEdit
        {
            text: "Charts"
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredHeight: 0
            font.pointSize: 13
            font.bold: true
        }

        TableView
        {
            model: chartViewModel
            id: repeater
            Layout.alignment: Qt.AlignHCenter
            Layout.fillHeight: true
            Layout.fillWidth: true
            columnSpacing: 20
            rowSpacing: 20

            columnWidthProvider: function() { return repeater.width/2 - 40 }
            rowHeightProvider: function() { return 300 }

            Component.onCompleted:
            {
                console.log(chartViewModel)
            }

            delegate: Rectangle
            {
                color: "red"
                visible: exists
                TextEdit
                {
                    text: label
                }
            }
        }
    }


    function toggleParameter(label, color)
    {
        if(chartViewModel)
        {
            if(!chartViewModel.toggleParameter(label, color))
            {
                return

                for (let i = 0; i < repeater.count; i++)
                {
                    const chartView = repeater.itemAt(i);

                    if(chartView.seriesMap.hasOwnProperty(label))
                    {
                        var series = chartView.seriesMap[label]

                        chartView.removeSeries(series)

                        delete chartView.seriesMap.label
                    }
                }
            }
        }
    }
}
