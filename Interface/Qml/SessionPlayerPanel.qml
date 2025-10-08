import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle 
{
    id: sessionPlayerPanel
    width: parent.width
    height: 75
    color: "#f8f9fa"
    border.color: "transparent"
    //border.width: 1
    radius: 4
    
    property alias isPlaying: playStopButton.isPlaying
    property alias currentPosition: positionSlider.value
    property alias maxPosition: positionSlider.to
    
    // Привязки к DriverDataPlayer
    Binding
    {
        target: playStopButton
        property: "isPlaying"
        value: driverDataPlayer ? driverDataPlayer.isPlaying : false
        when: driverDataPlayer !== null
    }
    
    signal playClicked()
    signal stopClicked()
    signal positionChanged(real position)
    
    RowLayout 
    {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15
        
        // Кнопка воспроизведения/остановки
        Button 
        {
            id: playStopButton
            property bool isPlaying: false
            
            Layout.preferredWidth: 80
            Layout.preferredHeight: 40
            
            text: isPlaying ? qsTr("Stop") : qsTr("Play")
            font.pixelSize: 14
            
            background: Rectangle 
            {
                color: parent.pressed ? 
                       (playStopButton.isPlaying ? "#dc3545" : "#28a745") :
                       (playStopButton.isPlaying ? "#dc3545" : "#28a745")
                radius: 5
                border.color: playStopButton.isPlaying ? "#dc3545" : "#28a745"
                border.width: 1
            }
            
            contentItem: Text 
            {
                text: parent.text
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: parent.font.pixelSize
                font.bold: true
            }
            
            onClicked: 
            {
                isPlaying = !isPlaying
                if (isPlaying) 
                {
                    sessionPlayerPanel.playClicked()
                }
                else 
                {
                    sessionPlayerPanel.stopClicked()
                }
            }
        }
        
        // Слайдер позиции
        Slider 
        {
            id: positionSlider
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            from: 0
            to: driverDataPlayer ? driverDataPlayer.sessionDuration : 100
            value: driverDataPlayer ? driverDataPlayer.elapsedTime : 0
            stepSize: 0.1
            
            onValueChanged: 
            {
                sessionPlayerPanel.positionChanged(value)
            }
        }
        
        // Временные метки
        RowLayout 
        {
            Layout.fillWidth: true
            
            Text 
            {
                text: Qt.formatTime(new Date(positionSlider.value * 1000), "mm:ss") + "/"
                font.pointSize: 11
                color: "dimgray"
            }
            
            // Item
            // {
            //     Layout.fillWidth: true
            // }
            
            Text 
            {
                text: Qt.formatTime(new Date(positionSlider.to * 1000), "mm:ss")
                font.pointSize: 11
                color: "dimgray"
            }
        }
    }
}
