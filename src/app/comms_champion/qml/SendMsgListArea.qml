import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import cc.GlobalConstants 1.0

Rectangle {
    id: sendArea
    color: GlobalConstants.background
    
    GroupBox {
        title: "Outgoing Messages"
        anchors.fill: parent
        
        state: GlobalConstants.waitingState
        states: [
            State {
                name: GlobalConstants.waitingState
                PropertyChanges { 
                    target: sendAreaToolBar
                    state: GlobalConstants.waitingState
                }
            },
            
            State {
                name: GlobalConstants.sendingState
                PropertyChanges { 
                    target: sendAreaToolBar
                    state: GlobalConstants.sendingState
                }
            },
            
            State {
                name: GlobalConstants.sendingAllState
                PropertyChanges { 
                    target: sendAreaToolBar
                    state: GlobalConstants.sendingAllState
                }
            }
        ]
        
        SendAreaToolBar {
            id: sendAreaToolBar
            onStartClicked: {
                state = GlobalConstants.sendingState
            }
            
            onStartAllClicked: {
                state = GlobalConstants.sendingAllState
            }
            
            
            onStopClicked: {
                state = GlobalConstants.waitingState
            }
        }
        
        MsgList {
            anchors.top: sendAreaToolBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom:parent.bottom
        }
    }
}
