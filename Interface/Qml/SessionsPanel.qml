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
    
    signal sessionSelected(int sessionId, string sessionName)
    signal refreshRequested()
    
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
            
            Item 
            {
                Layout.fillWidth: true
            }
            
            // Button
            // {
            //     text: qsTr("Refresh")
            //     onClicked: sessionsPanel.refreshRequested()
                
            //     background: Rectangle
            //     {
            //         color: parent.pressed ? "#4CAF50" : "#5cb85c"
            //         radius: 3
            //     }
                
            //     contentItem: Text
            //     {
            //         text: parent.text
            //         color: "white"
            //         horizontalAlignment: Text.AlignHCenter
            //         verticalAlignment: Text.AlignVCenter
            //     }
            // }
        }
        
        // Список сессий
        ListView 
        {
            id: sessionsList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            delegate: Rectangle 
            {
                width: sessionsList.width
                height: 60
                radius: 2
                anchors.margins: 2
                color: sessionsList.currentIndex === index ? Qt.hsva(0.6, 0.99, 0.99, 0.3) : "transparent"
                
                MouseArea 
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: 
                    {
                        sessionsList.currentIndex = index
                        sessionsPanel.sessionSelected(sessionId, sessionName)
                        
                        // Загружаем сессию в плеер
                        if (sessionPlayer)
                        {
                            sessionPlayer.loadSession(sessionId)
                        }
                    }
                    
                    onEntered:
                    {
                        if (sessionsList.currentIndex !== index)
                        {
                            //parent.color = Qt.hsva(0.6, 0.99, 0.99, 0.2)
                        }
                    }
                    
                    onExited:
                    {
                        if (sessionsList.currentIndex !== index)
                        {
                            parent.color = "transparent"
                        }
                    }
                }
                
                ColumnLayout 
                {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 8
                    
                    Text 
                    {
                        Layout.fillWidth: true
                        text: sessionName
                        font.pixelSize: 14
                        font.bold: true
                        color: "#333333"
                        elide: Text.ElideRight
                    }
                    
                    RowLayout 
                    {
                        Layout.fillWidth: true
                        
                        Text 
                        {
                            text: qsTr("Created: ") + Qt.formatDateTime(createdAt, "dd.MM.yyyy hh:mm")
                            font.pixelSize: 11
                            color: "#666666"
                        }
                        
                        Item 
                        {
                            Layout.fillWidth: true
                        }
                        
                        Text 
                        {
                            text: messageCount + " " + qsTr("messages")
                            font.pixelSize: 11
                            color: "#666666"
                        }
                    }
                }
            }
            
            ScrollBar.vertical: ScrollBar 
            {
                active: true
            }
        }
        
        // // Информация о текущей сессии
        // Rectangle
        // {
        //     Layout.fillWidth: true
        //     height: 30
        //     color: "transparent"
            
        //     Text
        //     {
        //         anchors.centerIn: parent
        //         text: sessionsList.currentIndex >= 0 ?
        //               qsTr("Selected session: ") + (sessionsList.model ? sessionsList.model.get(sessionsList.currentIndex).sessionName : "") :
        //               qsTr("Select a session")
        //         font.pixelSize: 12
        //         color: "#495057"
        //     }
        // }
    }
}
