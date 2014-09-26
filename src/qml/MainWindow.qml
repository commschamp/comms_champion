import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1

ApplicationWindow {
    id: mainWindow
    visibility: Window.Maximized
    visible: true
    
    /* menuBar: MenuBar {
        Menu {
            title: "File"
             
            MenuItem {
                text: "Quit"
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit();
            } 
        }
    } */
    

    SplitView {
        anchors.fill: parent
        resizing : false

        LeftPane {
            width: parent.width / 2
        }
        
        RightPane {
            Layout.fillWidth: true
        }
    }
}
