import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import "."
import cc.protocol.MsgMgr 1.0

Rectangle {
    color: GlobalConstants.background
    height: parent.height / 2
    
    GroupBox {
        id: recvArea
        title: "Incoming Messages"
        anchors.fill: parent
        
        state: GlobalConstants.waitingState
        states: [
            State {
                name: GlobalConstants.waitingState
                PropertyChanges { 
                    target: recvAreaToolBar
                    state: GlobalConstants.waitingState
                }
            },
            
            State {
                name: GlobalConstants.runningState
                PropertyChanges { 
                    target: recvAreaToolBar
                    state: GlobalConstants.runningState
                }
            }
        ]
        
        RecvAreaToolBar {
            id: recvAreaToolBar
            onStartClicked: {
                recvArea.state = GlobalConstants.runningState
            }
            
            onStopClicked: {
                recvArea.state = GlobalConstants.waitingState
            }
        }
        
        MsgList {
            id: msgList
            anchors.top: recvAreaToolBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom:parent.bottom
        }
        
        Connections {
            target: MsgMgr
            onMsgReceived: {
                console.log("Message received: " + msg.name)
                if (recvArea.state == GlobalConstants.runningState) {
                    msgList.addMsg(msg)
                }
            }
        }
    }
}
