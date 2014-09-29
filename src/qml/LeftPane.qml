import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import "."

SplitView {
    orientation: Qt.Vertical

    Rectangle {
        id: recvArea
        color: GlobalConstants.background
        height: parent.height / 2
        
        GroupBox {
            title: "Incoming Messages"
            anchors.fill: parent

            RecvAreaToolBar {}
            Rectangle {}
        }
    }

    Rectangle {
        id: sendArea
        color: GlobalConstants.background
        
        GroupBox {
            title: "Outgoing Messages"
            anchors.fill: parent
            Rectangle {
            }
        }
    }
}