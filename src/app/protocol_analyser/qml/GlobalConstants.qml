pragma Singleton
import QtQuick 2.0
QtObject {
    property color background: "lightgray"
    
    property string waitingState: "WAITING"
    property string runningState: "RUNNING"
    property string sendingState: "SENDING"
    property string sendingAllState: "SENDING_ALL"
    
    property string startIconPathFromQml: "../image/start.png"
    property string startAllIconPathFromQml: "../image/start_all.png"
    property string stopIconPathFromQml: "../image/stop.png"
    property string saveIconPathFromQml: "../image/save.png"
    property string configIconPathFromQml: "../image/config.png"
}
