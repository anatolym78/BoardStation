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
    
    property alias isPlaying: playPauseButton.isPlaying
    property alias currentPosition: positionSlider.value
    property alias maxPosition: positionSlider.to
    
    // Привязки к DriverDataPlayer
    Binding
    {
        target: playPauseButton
        property: "isPlaying"
        value:  parametersPlayer ?  parametersPlayer.isPlaying : false
        when:  parametersPlayer !== null
    }
    
    signal playClicked()
    signal pauseClicked()
    signal stopClicked()
    signal positionChanged(real position)
    
    RowLayout 
    {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15
        
        // Кнопка воспроизведения/паузы
        Button 
        {
            id: playPauseButton
            property bool isPlaying: false
            
            Layout.preferredWidth: 80
            Layout.preferredHeight: 40
            
            text: isPlaying ? qsTr("Pause") : qsTr("Play")
            font.pixelSize: 14

            visible: parametersPlayer ? parametersPlayer.isPlayable() : false

            background: Rectangle 
            {
                color: parent.pressed ? 
                       (playPauseButton.isPlaying ? "#ffc107" : "#28a745") :
                       (playPauseButton.isPlaying ? "#ffc107" : "#28a745")
                radius: 5
                border.color: playPauseButton.isPlaying ? "#ffc107" : "#28a745"
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
                if (isPlaying) 
                {
                    sessionPlayerPanel.pauseClicked()
                }
                else 
                {
                    sessionPlayerPanel.playClicked()
                }
            }
        }
        
        // Кнопка остановки
        Button 
        {
            id: stopButton
            
            Layout.preferredWidth: 80
            Layout.preferredHeight: 40
            
            text: qsTr("Stop")
            font.pixelSize: 14

            visible: parametersPlayer ? parametersPlayer.isPlayable() : false

            background: Rectangle 
            {
                color: parent.pressed ? "#dc3545" : "#dc3545"
                radius: 5
                border.color: "#dc3545"
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
                sessionPlayerPanel.stopClicked()
            }
        }
        
        // Слайдер позиции
        Slider 
        {
            id: positionSlider
            Layout.fillWidth: true
            Layout.fillHeight: true
            //live: false
            
            from: 0
            to:  parametersPlayer ?  parametersPlayer.sessionDuration : 100
            value:  parametersPlayer ?  parametersPlayer.elapsedTime : 0
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
