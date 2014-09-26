import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1


ApplicationWindow {
    visibility: Window.Maximized
    visible: true

    SplitView {
        anchors.fill: parent
        resizing : false

        SplitView {
            orientation: Qt.Vertical
            width: parent.width / 2

            Rectangle {
                id: recvMsgs
                color: "lightgray"
                height: parent.height / 2
            }

            Rectangle {
                id: sendMsgs
                color: "lightgray"
            }
        }

        SplitView {
            orientation: Qt.Vertical
            Layout.fillWidth: true

            Rectangle {
                id: msgDetails
                color: "lightgray"
                height: (parent.height * 2) / 3
            }

            Rectangle {
                id: protocolStackDetails
                color: "lightgray"
                anchors.topMargin: 10
            }
        }
    }
}
