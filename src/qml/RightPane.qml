import QtQuick 2.1
import QtQuick.Controls 1.1

SplitView {
    orientation: Qt.Vertical

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