import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.13
import QtGraphicalEffects 1.13

GridLayout{
    id: root

    rows: 2
    columns: 2


    Layout.alignment: Qt.AlignVCenter

    TextField{
        id: address
        Layout.fillWidth: true
        placeholderText: qsTr("Enter address")
    }
    Label {
        id: status
        text: qsTr("Status")
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    }

    TextField{
        id: port
        Layout.fillWidth: true
        placeholderText: qsTr("Enter port")
    }
    Button{
        id: btn_connect
        text: qsTr("Connect")

    }
}


/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
