import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import cc.GuiAppMgr 1.0
import cc.GlobalConstants 1.0

Rectangle {
    id: recvAreaRect
    signal msgClicked(int index)
    
    color: GlobalConstants.background
    height: parent.height / 2
    
    GroupBox {
        id: recvArea
        title: "Incoming Messages"
        anchors.fill: parent
        
        state: GuiAppMgr.recvState
        
        RecvAreaToolBar {
            id: recvAreaToolBar
        }
        
        MsgList {
            id: msgList
            anchors.top: recvAreaToolBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom:parent.bottom
            
            onMsgClicked: {
                recvAreaRect.msgClicked(index);
            }
        }
        
        Connections {
            target: GuiAppMgr
            onSigAddRecvMsg: {
                console.log("Message received: " + msg.name)
                if (recvArea.state == GlobalConstants.runningState) {
                    msgList.addMsg(msg)
                }
            }
        }
        
        Connections {
            target: GuiAppMgr
            onSigSetRecvState: {
                recvArea.state = state
            }
    }

    }
}
