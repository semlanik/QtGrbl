import QtQuick 2.0

import QtGrbl 1.0

//ToDo: SidebarItem should be here
Rectangle {
    color: "#dddddd"
    border.width: 1
    width: parent.width
    height: childrenRect.height

    Text {
        id: infoItem
        anchors.left: parent.left
        anchors.right: parent.right
        height: infoItem.implicitHeight
        text: "Motion Mode: " + (GrblEngine.gcodeState.isValid ? GrblEngine.gcodeState.motionMode : "-")
    }
}
