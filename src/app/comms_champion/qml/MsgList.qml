import QtQuick 2.1
import QtQuick.Controls 1.1

ScrollView {
    id: listView
    signal msgClicked(int index)

    function addMsg(msg) {
        msgList.addMsg(msg)
    }
    
    ListView {
        id: msgList
        anchors.fill: parent
        
        function addMsg(msg) {
            model.append({"msg": msg})
            msgList.currentIndex = msgList.count - 1;            
        }
        
        Component {
            id: listDelegate
            
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 20
                Row {
                    Text { text: msg.name + ": {...}" }
                }
                
                MouseArea {
                    anchors.fill: parent
                  
                    onClicked:{
                        msgList.currentIndex = index;
                        listView.msgClicked(index);
                    }
                }
            }
        }
        
        model: ListModel {}
        
        delegate: listDelegate
        
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        highlightFollowsCurrentItem: true
    }
}
