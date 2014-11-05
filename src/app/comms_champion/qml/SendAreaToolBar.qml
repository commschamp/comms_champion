import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import cc.GlobalConstants 1.0
import cc.GuiAppMgr 1.0

ToolBar {
    id: thisToolbar
    state: GuiAppMgr.sendState
    
    states: [
        State {
            name: GlobalConstants.waitingState
            PropertyChanges { 
                target: startStopButton
                tooltip: "Send selected"
                iconSource: GlobalConstants.startIconPathFromQml
            }
            
            PropertyChanges { 
                target: startStopAllButton
                tooltip: "Send all"
                iconSource: GlobalConstants.startAllIconPathFromQml
            }
            
            
            PropertyChanges { 
                target: saveButton
                enabled: true
            }
        },
        
        State {
            name: GlobalConstants.sendingState
            PropertyChanges{
                target: startStopButton
                tooltip: "Stop"
                iconSource: GlobalConstants.stopIconPathFromQml
            }
            
            PropertyChanges{
                target: startStopAllButton
                enabled: false
                iconSource: GlobalConstants.startAllIconPathFromQml
            }
            
            PropertyChanges { 
                target: saveButton
                enabled: false
            }
        },
        
        State {
            name: GlobalConstants.sendingAllState
            PropertyChanges{
                target: startStopButton
                enabled: false
                iconSource: GlobalConstants.startIconPathFromQml
            }
            
            PropertyChanges{
                target: startStopAllButton
                tooltip: "Stop"
                iconSource: GlobalConstants.stopIconPathFromQml
            }
            
            PropertyChanges { 
                target: saveButton
                enabled: false
            }
        }
        
    ]
    
    style: ToolBarStyle {
        background: Rectangle {
            color: GlobalConstants.background
        }
    }
    
    RowLayout {
        ToolButton {
            id: startStopButton
            onClicked: {
                if (thisToolbar.state == GlobalConstants.waitingState) {
                    GuiAppMgr.sendStartClicked()
                }
                else {
                    console.assert(
                        thisToolbar.state == GlobalConstants.sendingState, 
                        "Unknown state" + thisToolbar.state);
                    GuiAppMgr.sendStopClicked()
                }
            }
        }
        
        ToolButton {
            id: startStopAllButton
            onClicked: {
                if (thisToolbar.state == GlobalConstants.waitingState) {
                    GuiAppMgr.sendStartAllClicked()
                }
                else {
                    console.assert(
                        thisToolbar.state == GlobalConstants.sendingAllState, 
                        "Unknown state" + thisToolbar.state);
                    GuiAppMgr.sendStopClicked()
                }
            }
        }
        
        ToolButton {
            id: saveButton
            tooltip: "Save Messages"
            iconSource: GlobalConstants.saveIconPathFromQml
            
            onClicked: {
                GuiAppMgr.sendSaveClicked()
            }
        }
    }
    
    Connections {
        target: GuiAppMgr
        onSigSetSendState: {
            thisToolbar.state = state
        }
    }

}
