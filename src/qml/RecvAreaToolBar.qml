import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import "."

ToolBar {
    id: thisToolbar
    signal startClicked
    signal stopClicked
    signal saveClicked
    
    state: GlobalConstants.waitingState
    
    states: [
        State {
            name: GlobalConstants.waitingState
            PropertyChanges { 
                target: startStopButton
                tooltip: "Start"
                iconSource: GlobalConstants.startIconPathFromQml
            }
            
            PropertyChanges { 
                target: saveButton
                enabled: true
            }
        },
        
        State {
            name: GlobalConstants.runningState
            PropertyChanges{
                target: startStopButton
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
            implicitHeight: 100
            color: GlobalConstants.background
        }
    }
    
    RowLayout {
        ToolButton {
            id: startStopButton
            onClicked: {
                if (thisToolbar.state == GlobalConstants.waitingState) {
                    thisToolbar.startClicked()
                }
                else {
                    console.assert(
                        thisToolbar.state == GlobalConstants.runningState, 
                        "Unknown state" + thisToolbar.state);
                    thisToolbar.stopClicked()
                }
            }
        }
        
        ToolButton {
            id: saveButton
            tooltip: "Save Messages"
            iconSource: GlobalConstants.saveIconPathFromQml
            
            onClicked: {
                thisToolbar.saveClicked()
            }
        }
    }
}
