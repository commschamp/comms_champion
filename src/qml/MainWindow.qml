import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import "."

ApplicationWindow {
    id: mainWindow
    visibility: Window.Maximized
    visible: true
    title: "CommsChampion"
    
    /*MenuBar {
        Menu {
            title: "File"
             
            MenuItem {
                text: "Quit"
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit();
            } 
        }
    }*/ 
    
    Rectangle {
        anchors.fill: parent
        ColumnLayout {
            anchors.fill: parent
            ToolBar {
                id: toolbar
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                RowLayout {
                    anchors.fill: parent
                    ToolButton {
                        iconSource: GlobalConstants.configIconPathFromQml
                        tooltip: "Configuration"
                    }
                }
            }
            
            SplitView {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: toolbar.bottom
                anchors.bottom: parent.bottom
                resizing : false

                LeftPane {
                    width: parent.width / 2
                }
                
                RightPane {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
