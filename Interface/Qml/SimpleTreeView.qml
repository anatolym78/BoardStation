import QtQuick 2.12
import QtQuick.Controls 1.4 // Используем Controls 1.4 для TreeView в Qt 5
import BoardStation 1.0

Rectangle
{
    width: 400
    height: 600

    // Создаем экземпляр нашей C++ модели
    SimpleTreeModel
    {
        id: myTreeModel
    }

    TreeView
    {
        id: treeView
        anchors.fill: parent
        anchors.margins: 10
        model: myTreeModel

        TableViewColumn
        {
            role: "display" // По умолчанию используем DisplayRole
            title: "Title"
            width: 200
        }
    }
}
