import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.1 as Plarform
import QtQuick.Controls.Styles 1.4

//import MyComponents 1.0

import Connection 1.0

ApplicationWindow{
    id: window

    width: 400
    height: 600
    minimumWidth: 400

    title: qsTr("Map Reduce Client")
    visible: true

    ConnectionModel{
        id: dmodel
        connections: connectionList
    }

    property RegExpValidator ipAddress: RegExpValidator{regExp: /^(\d|[1-9]\d|1\d\d|2([0-4]\d|5[0-5]))\.(\d|[1-9]\d|1\d\d|2([0-4]\d|5[0-5]))\.(\d|[1-9]\d|1\d\d|2([0-4]\d|5[0-5]))\.(\d|[1-9]\d|1\d\d|2([0-4]\d|5[0-5]))$/}
    signal fileChoose(string filePath)

    Component{
        id: delegateConnetion
        Pane{
            width: parent.width

            GridLayout{
                width: parent.width

                rows: 2
                columns: 4

                TextField{
                    id: _address
                    Layout.fillWidth: true
                    placeholderText: qsTr("Enter address")
                    validator: ipAddress
                    onEditingFinished: address = text;
                    text: address
                }
                Label {
                    text: status
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Label {
                    text: {
                        switch(workState)
                        {
                        case 0:
                            qsTr("None");
                            break;
                        case 1:
                            qsTr("Free");
                            break;
                        case 2:
                            qsTr("Busy");
                            break;
                        }
                    }
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Label {
                    text: {
                        if(workState === 0 || workState === 1)
                            "";
                        else
                            precent + "%"
                    }
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField{
                    id: _port
                    Layout.fillWidth: true
                    placeholderText: qsTr("Enter port")
                    onEditingFinished: port = text
                    text: port
                }
                Button{
                    Layout.fillWidth: true;
                    Layout.columnSpan: 3
                    onClicked: dmodel.onConnectClicked(index, _address.text, _port.text)
                    enabled: stateConnection === 1? false : true
                    text: {
                        switch(stateConnection)
                        {
                        case 0:
                            qsTr("Connect");
                            break;
                        case 1:
                            qsTr("Connect");
                            break;
                        case 2:
                            qsTr("Disconnect");
                            break;
                        }
                    }
                }
            }
        }
    }

    FileDialog {
        id: openfileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        onAccepted: {
            console.log("You chose: " + openfileDialog.fileUrl)
            worker.openFile(openfileDialog.fileUrl)
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    Plarform.FileDialog {
        id: savefileDialog
        title: "Please choose a file"
        //folder: shortcuts.home
        fileMode: Plarform.FileDialog.SaveFile
        onAccepted: {
            console.log("You chose: " + savefileDialog.fileUrl)
            worker.saveResultToFile(savefileDialog.file)
        }
        onRejected: {
            console.log("Canceled")
        }
    }


    Connections{
        target: worker
        onWorkExistChanged: {
            _sendServerBtn.enabled = worker.workExist
        }
        onVisibleFileDialogChanged:{
            _progressDialog.visible = worker.visibleFileLoadDialog
        }
        onProgressLoadChanged:{
            _progressBar.value = worker.progressLoad / 100.0
        }
        onWorkersFinished:{
            savefileDialog.visible = true;
        }
        onSendingDataChanged:{
            _seindidDataDialog.visible = worker.sendingData;
        }
    }

    Dialog{
        id: _progressDialog
        standardButtons:StandardButton.NoButton
        //visible: worker.visibleFileLoadDialog

        title: qsTr("File Loading")

        ColumnLayout{
            anchors.fill: parent
            Label{
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true

                text: qsTr("File Loading")
            }

            ProgressBar{
                id:_progressBar
                Layout.fillWidth: true
                //value: worker.getProgressLoad / 100.0

            }
        }
    }

    Dialog{
        id: _seindidDataDialog
        standardButtons:StandardButton.NoButton
        //visible: worker.visibleFileLoadDialog

        title: qsTr("Sending Data")

        ColumnLayout{
            anchors.fill: parent
            Label{
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true

                text: qsTr("Sending Data")
            }

            ProgressBar{
                id:_sendingDataBar
                Layout.fillWidth: true
                indeterminate: true;

            }
        }
    }

    ColumnLayout{
        anchors.fill: parent
        Button{
            id: _openFileBtn
            text: qsTr("Open file with task")
            Layout.fillWidth: true
            Layout.margins: 2
            onClicked: openfileDialog.open()
        }

        Button{
            id: addConnection
            onClicked: dmodel.appendConnectionItem()

            Layout.margins: 2

            text: qsTr("Add Connection to Client")
            autoRepeat: false
            highlighted: false
            flat: false
            Layout.fillWidth: true
        }

        ListView{
            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: 2
            model: dmodel
            delegate: delegateConnetion
        }

        Button{
            id: _sendServerBtn
            text: qsTr("Send task to server")
            Layout.fillWidth: true
            Layout.margins: 2
            enabled: worker.workExist
            onClicked: worker.sendWorkToServer()
        }
    }
}
