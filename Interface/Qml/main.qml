import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import QtCharts 2.12
import BoardStation 1.0

Item
{
    id: mainWindow
    width: 1400
    height: 960
      
    Component.onCompleted:
    {
        boardParametersList.parametersListModel = parametersListModel
        droneControlPanel.uplinkParametersModel = uplinkParametersModel
        sessionsPanel.sessionsModel = sessionsListModel
    }

    // Main frame
    Frame
    {
        anchors.fill: parent

        ColumnLayout
        {
            anchors.fill: parent
            spacing: 8

            Rectangle
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
                //color:"#f7f7f7"
                radius: 4

                RowLayout
                {
                    anchors.fill: parent
                    spacing: 8

                ColumnLayout
                {
                    Layout.preferredWidth: 1.7
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 8

                    // Отображает список параметров, полученных от дрона
                    BoardParametersList
                    {
                        id: boardParametersList
                        Layout.fillWidth: true
                         Layout.fillHeight: true
                        Layout.preferredHeight: 3

                        onParameterSelected:
                        {
                            simpleChartsPanel.toggleParameter(label, color)
                        }
                    }

                    // Панель управления сессиями
                    SessionsPanel
                    {
                        id: sessionsPanel
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: 2
                        
                        // Передаем состояние записи и ID текущей сессии
                        isRecording: qmlMainWindow.isRecording()
                        currentRecordingSessionId: qmlMainWindow.isRecording() ? 
                            (boardMessagesWriter ? boardMessagesWriter.getCurrentSessionId() : -1) : -1

                        onSessionSelected:
                        {
                            //console.log("Выбрана сессия:", sessionId, sessionName)
                        }

                        onRefreshRequested:
                        {
                            //console.log("Запрос на обновление сессий")
                            sessionsListModel.refreshSessions()
                        }
                    }
                }

                ColumnLayout
                {
                    Layout.preferredWidth: 5
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 8

                    // Панель с графиками
                    Rectangle
                    {
                        Layout.preferredHeight: 3
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "#f0f0f0"
                        //border.color: "gainsboro"
                        radius: 4

                        SimpleChartsPanel
                        {
                             id: simpleChartsPanel
                             anchors.fill: parent
                             anchors.margins: 5
                        }
                    }

                    // // Панель с отладкой/логами
                    // Rectangle
                    // {
                    //     Layout.preferredHeight: 1.5
                    //     Layout.fillWidth: true
                    //     Layout.fillHeight: true
                    //     color: "#f0f0f0"
                    //     radius: 4

                    //     DebugLogsView
                    //     {
                    //         anchors.fill: parent
                    //         model: debugViewModel
                    //     }
                    // }
                }

                 // Панель управления параметрами дрона
                 DroneControlPanel
                 {
                     id: droneControlPanel
                     Layout.preferredWidth: 2.5
                     Layout.fillWidth: true
                     Layout.fillHeight: true

                 }
             }

            }
            
            // Панель плеера сессий в самом низу
            SessionPlayerPanel
            {
                id: sessionPlayerPanel
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                
                onPlayClicked:
                {
                    console.log("Play clicked")
                    if ( parametersPlayer)
                    {
                         parametersPlayer.play()
                    }
                }
                
                onStopClicked:
                {
                    console.log("Stop clicked")
                    if ( parametersPlayer)
                    {
                         parametersPlayer.stop()
                    }
                }
                
                onPositionChanged:
                {
                    if ( parametersPlayer)
                    {
                        // Конвертируем секунды в QDateTime
                        var newPosition = new Date( parametersPlayer.sessionStartTime.getTime() + position * 1000)
                         parametersPlayer.setPosition(newPosition)
                    }
                }
            }
        }

 }

    
    // Альтернативные привязки с использованием Binding (если нужно)
    Binding
    {
        target: boardParametersList
        property: "parametersListModel"
        value: boardParametersList
        when: boardParametersList !== null
    }
    
    Binding
    {
        target: droneControlPanel
        property: "uplinkParametersModel"
        value: uplinkParametersModel
        when: uplinkParametersModel !== null
    }
    
    Binding
    {
        target: sessionsPanel
        property: "sessionsModel"
        value: sessionsListModel
        when: sessionsListModel !== null
    }
}

