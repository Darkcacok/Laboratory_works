import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3

ApplicationWindow{
    id: window

    width: 400
    height: 600
    minimumWidth: 400

    title: qsTr("Map Reduce Client")
    visible: true


    ConnectionComponent{
        x: 0
        y: 0
        width: parent.width
    }
}
