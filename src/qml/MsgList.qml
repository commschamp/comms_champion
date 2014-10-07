import QtQuick 2.1
import QtQuick.Controls 1.1

ScrollView {
    ListView {
        id: msgList
        anchors.fill: parent
        
        Component {
            id: listDelegate
            
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 20
                Row {
                    Text { text: name + ": {...}" }
                }
                
                MouseArea {
                    anchors.fill: parent
                  
                    onClicked:{
                        msgList.currentIndex = index;
                    }
                }
            }
        }
        
        model: ListModel {
            // TODO: remove
            ListElement {
                name: "Ack"
            }
            ListElement {
                name: "Nack"
            }
            ListElement {
                name: "Heartbeat"
            }
        }
        
        delegate: listDelegate
        
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        highlightFollowsCurrentItem: true
    }
}
