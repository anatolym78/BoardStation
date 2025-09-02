import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    id: chartsPanel


    
    property var parametersModel: null
    property var chartBuilder: null
    
    // Список активных графиков
    property var activeCharts: []
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        
        Text {
            text: "Charts"
            font.pointSize: 14
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }
        
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            ColumnLayout {
                id: chartsColumn
                width: parent.width
                spacing: 10
                
                // Здесь будут добавляться графики
            }
        }
    }
    
    // Функция для добавления/удаления графика
    function toggleChart(parameterIndex) {
        console.log("ChartsPanel: toggleChart called with index", parameterIndex)
        
        if (!parametersModel) {
            console.warn("ChartsPanel: parametersModel not available")
            return
        }
        
        if (!chartBuilder) {
            console.warn("ChartsPanel: chartBuilder not available")
            return
        }
        
        // Проверяем, есть ли уже график для этого параметра
        var existingChart = findChartByIndex(parameterIndex)
        
        if (existingChart) {
            // Удаляем существующий график
            removeChart(existingChart)
            console.log("ChartsPanel: Removed chart for parameter index", parameterIndex)
        } else {
            // Создаем новый график
            var parameter = getParameterByIndex(parameterIndex)
            console.log("ChartsPanel: Created parameter object:", parameter)
            
            if (parameter && chartBuilder.canCreateChart(parameter)) {
                var chart = chartBuilder.createChart(parameter)
                console.log("ChartsPanel: Chart created:", chart)
                
                if (chart) {
                    addChart(chart, parameterIndex)
                    console.log("ChartsPanel: Added chart for parameter index", parameterIndex)
                } else {
                    console.warn("ChartsPanel: Chart creation returned null")
                }
            } else {
                console.warn("ChartsPanel: Cannot create chart for parameter index", parameterIndex)
            }
        }
    }
    
    // Функция для добавления графика в панель
    function addChart(chart, parameterIndex) {
        if (!chart) return
        
        // Добавляем график в layout
        chartsColumn.children.push(chart)
        
        // Добавляем в список активных графиков
        activeCharts.push({ index: parameterIndex, chartItem: chart })
        
        console.log("ChartsPanel: Chart added successfully")
    }
    
    // Функция для удаления графика из панели
    function removeChart(chartEntry) {
        if (!chartEntry || !chartEntry.chartItem) return
        
        // Удаляем график
        chartEntry.chartItem.destroy()
        
        // Удаляем из списка активных графиков
        var index = activeCharts.indexOf(chartEntry)
        if (index !== -1) {
            activeCharts.splice(index, 1)
        }
    }
    
    // Функция для поиска графика по индексу параметра
    function findChartByIndex(parameterIndex) {
        for (var i = 0; i < activeCharts.length; i++) {
            if (activeCharts[i].index === parameterIndex) {
                return activeCharts[i]
            }
        }
        return null
    }
    
    // Функция для получения параметра по индексу
    function getParameterByIndex(parameterIndex) {
        if (!parametersModel) return null
        
        // Создаем простой объект параметра
        var parameter = {
            label: parametersModel.data(parametersModel.index(parameterIndex, 0), Qt.DisplayRole) || "Parameter " + parameterIndex
        }
        
        return parameter
    }
}
