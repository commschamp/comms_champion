import QtQuick 2.1
import QtQuick.Controls 1.1
import cc.GlobalConstants 1.0
import cc.WidgetProxy 1.0
import cc.GuiAppMgr 1.0

SplitView {
    orientation: Qt.Vertical

    Rectangle {
        id: msgDetails
        color: GlobalConstants.background
        height: (parent.height * 2) / 3
        width: parent.width
        
        GroupBox {
            title: "Message Details"
            anchors.fill: parent
        
            /*Rectangle {
                anchors.fill: parent
                color: GlobalConstants.background
            }*/
            
            WidgetProxy {
                id: msgDetailsWidget
                anchors.fill: parent
                
                Connections {
                    target: GuiAppMgr
                    onSigDisplayMsgDetailsWidget: {
                        msgDetailsWidget.updateWidget(widget)
                    }
                }
            }
        }
    }

    Rectangle {
        id: protocolStackDetails
        color: GlobalConstants.background
        width: parent.width
        
        GroupBox {
            title: "Protocol Details"
            anchors.fill: parent
        
            Rectangle {}
        }
    }
}
