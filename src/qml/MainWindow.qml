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

        Rectangle {
            id: column
            width: 200
            Layout.minimumWidth: 100
            Layout.maximumWidth: 300
            color: "lightsteelblue"
        }

        SplitView {
            orientation: Qt.Vertical
            Layout.fillWidth: true

            Rectangle {
                id: row1
                height: 200
                color: "lightblue"
                Layout.minimumHeight: 1
            }

            Rectangle {
                id: row2
                color: "lightgray"
            }
        }
    }
}
