import QtQuick 2.12
import QtCharts 2.12

ChartView {
    id: chartView
    antialiasing: true
    title: "Simple Chart"
    
    property string parameterName: ""
    
    Component.onCompleted: {
        setupSimpleChart()
    }
    
    function setupSimpleChart() {
        console.log("SimpleChart: Setting up chart for", parameterName)
        
        // Создаем серию данных
        var series = chartView.createSeries(ChartView.SeriesTypeLine, parameterName)
        if (!series) {
            console.warn("SimpleChart: Failed to create series")
            return
        }
        
        // Устанавливаем красный цвет линии
        series.color = "red"
        series.width = 2
        
        // Добавляем две точки: (0, 0) и (1, 1)
        series.append(0, 0)
        series.append(1, 1)
        
        console.log("SimpleChart: Added points (0,0) and (1,1)")
        
        // Создаем оси
        var axisX = chartView.createValueAxis()
        var axisY = chartView.createValueAxis()
        
        axisX.titleText = "X"
        axisY.titleText = "Y"
        
        // Привязываем оси к серии
        series.attachAxis(axisX)
        series.attachAxis(axisY)
        
        // Добавляем оси на график
        chartView.addAxis(axisX, Qt.AlignBottom)
        chartView.addAxis(axisY, Qt.AlignLeft)
        
        // Устанавливаем диапазон осей
        axisX.min = 0
        axisX.max = 1
        axisY.min = 0
        axisY.max = 1
        
        console.log("SimpleChart: Chart setup completed")
    }
}
