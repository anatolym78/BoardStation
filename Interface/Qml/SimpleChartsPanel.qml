import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.15

Item
{
    id: simpleChartsPanel
    
    // Функция для добавления нового графика
    function addChart(chartTitle) 
    {
        console.log("SimpleChartsPanel: Adding chart:", chartTitle)
        if (chartViewModel) 
        {
            chartViewModel.addChart(chartTitle)
            console.log("SimpleChartsPanel: Chart added successfully:", chartTitle)
        }
        else 
        {
            console.warn("SimpleChartsPanel: chartViewModel is not available")
        }
    }

    Flow
    {
        id: chartsFlow
        anchors.fill: parent
        spacing: 10

        Repeater
        {
            model: chartViewModel
            id: repeater

            // График
            delegate: ChartView
            {
                id: chartView
                property string chartTitle: model.chartTitle
                property int chartIndex: model.chartIndex
                width: 320
                height: 240
                theme: ChartView.ChartThemeLight
                
                title: chartTitle
                
                // Простая настройка осей
                ValueAxis
                {
                    id: xAxis
                    min: 0
                    max: 50
                    titleText: "Время (сек)"
                }

                ValueAxis
                {
                    id: yAxis
                    min: -750
                    max: 750
                    titleText: "Значение"
                }

                axes: [xAxis, yAxis]
                antialiasing: true
            }
        }
    }
}
