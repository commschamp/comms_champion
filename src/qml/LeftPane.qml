import QtQuick 2.1
import QtQuick.Controls 1.1

SplitView {
    orientation: Qt.Vertical

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