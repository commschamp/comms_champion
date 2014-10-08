import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import "."

Rectangle {
    id: recvArea
    color: GlobalConstants.background
    height: parent.height / 2
    
    GroupBox {
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
                state = GlobalConstants.runningState
            }
            
            onStopClicked: {
                state = GlobalConstants.waitingState
            }
        }
        
        MsgList {
            anchors.top: recvAreaToolBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom:parent.bottom
        }
    }
}
