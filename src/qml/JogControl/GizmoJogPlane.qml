import QtQuick 2.0
import QtQuick3D 1.15

Node {
    property alias color: rect.color
    pivot: Qt.vector3d(-350, -350, -24)
    Item {
        width: childrenRect.width
        height: childrenRect.height
        Row {
            spacing: 30
            height: 100
            Text {
                text: "<"
                font.pixelSize: 100
                color: "black"
                anchors.verticalCenter: parent.verticalCenter
            }

            Rectangle {
                id: rect
                width: 100
                height: 100
                color: "grey"
            }

            Text {
                text: ">"
                font.pixelSize: 100
                color: "black"
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
