import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle 
{
    id: sessionsPanel
    width: parent.width
    height: 200
    color: "#f0f0f0"
    radius: 5
    
    property alias sessionsModel: sessionsList.model
    property alias currentIndex: sessionsList.currentIndex
    property bool isRecording: false
    property int currentRecordingSessionId: -1
    
    signal sessionSelected(int sessionId, string sessionName)
    signal refreshRequested()
    
    // Диалог подтверждения удаления
    property alias deleteDialog: deleteConfirmationDialog
    
    ColumnLayout 
    {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        
        // Заголовок панели
        RowLayout 
        {
            Layout.fillWidth: true
            
            Text 
            {
                text: qsTr("Sessions")
                font.pixelSize: 16
                font.bold: true
                color: "dimgray"
            }
        }
        
        // Список сессий
        ListView 
        {
            id: sessionsList
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 4
            clip: true
            enabled: !sessionsListModel.isRecordingState()
            
            delegate: Rectangle 
            {
                id: listItemRect
                width: sessionsList.width
                height: 70
                radius: 4
                anchors.margins: 2
                color:
                {
                    if(index == currentIndex)
                    {
                        return Qt.hsva(0.6, 0.99, 0.99, 0.2)
                    }
                    else
                    {
                        return "white"
                    }
                }
                
                // Определяем цвета в зависимости от состояния
                property bool isSelected: sessionsList.currentIndex === index
                property bool isRecordingSession: sessionId === sessionsPanel.currentRecordingSessionId
                property bool isDisabled: sessionsPanel.isRecording && !isRecordingSession
                
                // Отладочная информация
                Component.onCompleted:
                {
                    console.log("Session", sessionId, "isRecording:", sessionsPanel.isRecording, "currentRecordingSessionId:", sessionsPanel.currentRecordingSessionId, "isDisabled:", isDisabled)
                }
                
                // color:
                // {
                //     if (isSelected)
                //         return Qt.hsva(0.6, 0.99, 0.99, 0.3)
                //     else if (isDisabled)
                //         return "#e0e0e0"
                //     else
                //         return "transparent"
                // }
                
                // border.color:
                // {
                //     if (isSelected)
                //         return Qt.hsva(0.6, 0.99, 0.99, 0.8)
                //     else if (isDisabled)
                //         return "#cccccc"
                //     else
                //         return "transparent"
                // }
                // border.width: isSelected ? 1 : 0
                states:
                [
                    State
                    {
                        name: "hovered"
                        PropertyChanges
                        {
                            target: listItemRect;
                            color: Qt.hsva(0.6, 0.59, 0.99, 0.1)
                        }
                    }
                ]
                
                MouseArea 
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    enabled: !isDisabled
                    
                    onClicked: 
                    {
                        //currentIndex = index

                        //return
                        // Не загружаем сессию, если это текущая записываемая сессия
                        if (!isRecordingSession)
                        {
                            sessionsList.currentIndex = index
                            sessionsPanel.sessionSelected(sessionId, sessionName)
                            
                            // Загружаем сессию в плеер
                            if (sessionPlayer)
                            {
                                sessionPlayer.loadSession(sessionId)
                            }
                        }
                    }
                    
                    onEntered:
                    {
                        listItemRect.state = "hovered"
                    }
                    
                    onExited:
                    {
                        listItemRect.state = ""
                    }
                }
                
                ColumnLayout 
                {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 8
                    
                    RowLayout
                    {
                        Layout.fillWidth: true

                        // Красный кружок для текущей записываемой сессии
                        Rectangle
                        {
                            id: recordingIndicator
                            width: 10
                            height: 10
                            radius: 5
                            color: "red"
                            visible: recordedSession
                        }

                        Text
                        {
                            Layout.fillWidth: true
                            text: sessionName
                            font.pixelSize: 14
                            font.bold: true
                            color: isDisabled ? "#999999" : "#333333"
                            elide: Text.ElideRight
                        }

                        Button
                        {
                            id: deleteButton
                            Layout.alignment: right

                            background: Rectangle
                            {
                                id: deleteButtonBackground
                                height: 24
                                color: Qt.hsva(0.6, 0.3, 0.4, 0.7)
                                opacity: 0.9
                                radius: 3
                                states:
                                [
                                    State
                                    {
                                        name: "deleteButtonHovered"
                                        PropertyChanges
                                        {
                                            target: deleteButtonBackground
                                            color: "dimgray"
                                            opacity: 1
                                        }
                                    },
                                    State
                                    {
                                        name: "deleteButtonPressed"
                                        PropertyChanges
                                        {
                                            target: deleteButtonBackground
                                            color: "#d32f2f"// "darkslateblue"
                                            opacity: 1
                                        }
                                    }
                                ]

                                MouseArea
                                {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered:
                                    {
                                        deleteButtonBackground.state = "deleteButtonHovered"
                                    }

                                    onExited:
                                    {
                                        deleteButtonBackground.state = ""
                                    }

                                    onPressed:
                                    {
                                         deleteButtonBackground.state = "deleteButtonPressed"
                                    }

                                    onReleased:
                                    {
                                        deleteButtonBackground.state = "deleteButtonHovered"
                                    }
                                     onClicked:
                                     {
                                         // Показываем диалог подтверждения
                                         deleteConfirmationDialog.sessionIndex = index
                                         deleteConfirmationDialog.sessionName = sessionName
                                         deleteConfirmationDialog.open()
                                     }
                                }

                            }

                            contentItem: Text
                            {
                                text: qsTr("delete")
                                color: "white"
                                font.bold: true
                            }

                        }
                    }

                    RowLayout 
                    {
                        Layout.fillWidth: true
                        
                        Text 
                        {
                            text: qsTr("Created: ") + createdAtFormatted
                            font.pixelSize: 11
                            color: isDisabled ? "#bbbbbb" : "#666666"
                        }
                        
                        Item 
                        {
                            Layout.fillWidth: true
                        }
                        
                        Text 
                        {
                            text: messageCount + " " + qsTr("messages")
                            font.pixelSize: 11
                            color: isDisabled ? "#bbbbbb" : "#666666"
                        }
                    }
                }
            }
            
            ScrollBar.vertical: ScrollBar 
            {
                active: true
            }
        }
    }
    
    // Диалог подтверждения удаления
    Dialog
    {
        id: deleteConfirmationDialog
        title: qsTr("Подтверждение удаления")
        width: 400
        height: 200
        modal: true
        
        property int sessionIndex: -1
        property string sessionName: ""
        
        ColumnLayout
        {
            anchors.fill: parent
            spacing: 20
            
            Text
            {
                Layout.fillWidth: true
                text: qsTr("Вы уверены, что хотите удалить сессию \"%1\"?").arg(deleteConfirmationDialog.sessionName)
                wrapMode: Text.WordWrap
                font.pixelSize: 14
                color: "#333333"
            }
            
            Text
            {
                Layout.fillWidth: true
                text: qsTr("Это действие нельзя отменить. Все данные сессии будут удалены навсегда.")
                wrapMode: Text.WordWrap
                font.pixelSize: 12
                color: "#666666"
            }
            
            RowLayout
            {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignRight
                spacing: 10
                
                Button
                {
                    text: qsTr("Отмена")
                    onClicked: deleteConfirmationDialog.close()
                    
                    background: Rectangle
                    {
                        color: parent.pressed ? "#e0e0e0" : "#f0f0f0"
                        radius: 4
                        border.color: "#cccccc"
                        border.width: 1
                    }
                    
                    contentItem: Text
                    {
                        text: parent.text
                        color: "#333333"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                
                Button
                {
                    text: qsTr("Удалить")
                    onClicked:
                    {
                        if (deleteConfirmationDialog.sessionIndex >= 0)
                        {
                            sessionsListModel.removeSession(deleteConfirmationDialog.sessionIndex)
                        }
                        deleteConfirmationDialog.close()
                    }
                    
                    background: Rectangle
                    {
                        color: parent.pressed ? "#d32f2f" : "#f44336"
                        radius: 4
                    }
                    
                    contentItem: Text
                    {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                    }
                }
            }
        }
    }
}
