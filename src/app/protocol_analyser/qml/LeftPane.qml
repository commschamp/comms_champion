import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import "."

SplitView {
    orientation: Qt.Vertical
    
    RecvMsgListArea {}

    SendMsgListArea {}
}
