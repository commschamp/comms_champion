import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import "."

ToolBar {
    style: ToolBarStyle {
        background: Rectangle {
            color: GlobalConstants.background
        }
    }
    
    RowLayout {
        Button {
            text: "Start"
        }
    }
}
