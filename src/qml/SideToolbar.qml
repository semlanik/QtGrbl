import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {
    id: root
    property alias content: toolList.model
    property string position: "left" //might be left or right, better to use enum here
    color: "#cccccc"
    anchors {
        left: position == "left" ? parent.left : undefined
        right: position == "right" ? parent.right : undefined
        top: parent.top
        bottom: parent.bottom
    }

    width: 500

    Row {
        Button {

        }
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right
        Repeater {
            id: toolList
            anchors.fill: parent
        }
    }
}
