import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import cc.GlobalConstants 1.0
import cc.GuiAppMgr 1.0

Rectangle {
    id: sendArea
    color: GlobalConstants.background
    
    GroupBox {
        title: "Outgoing Messages"
        anchors.fill: parent
        
        //state: GuiAppMgr.sendState
        
        SendAreaToolBar {
            id: sendAreaToolBar
        }
        
        MsgList {
            anchors.top: sendAreaToolBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom:parent.bottom
        }
    }
}
