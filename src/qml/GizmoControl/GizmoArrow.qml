import QtQuick 2.15
import QtQuick3D 1.15

Node {
    id: root

    property color arrowColor: "gray"
    property color hoverColor: Qt.lighter(arrowColor)
    property var scale: 1.0

    signal clicked
    signal hovered

    property bool isHovered: false

    onClicked: {
        console.log("Click: " + root.objectName)
    }

    onHovered: {
        console.log("Hover: " + root.objectName)
        isHovered = true
    }

    Model {
        id: rowY

        property bool isHovered: false

        signal clicked
        signal hovered
        signal endHovered

        onClicked: root.clicked()
        onHovered: {
            isHovered = true
            root.hovered()
        }
        onEndHovered: {
            isHovered = false
            root.isHovered = false
        }

        objectName: "arrow"
        source: "#Cylinder"
        pickable: true
        scale: Qt.vector3d(0.3, 5, 0.3)
        position: Qt.vector3d(0, 250, 0)
        materials: DefaultMaterial {
            diffuseColor: isHovered ? hoverColor : arrowColor
        }
    }

    Model {
        id: coneY

        signal clicked
        signal hovered
        signal endHovered

        onClicked: root.clicked()
        onHovered: root.hovered()
        onEndHovered: root.isHovered = false

        objectName: "cone"
        source: "#Cone"
        pickable: true
        position: Qt.vector3d(0, 500, 0)
        materials: DefaultMaterial {
            diffuseColor: isHovered ? hoverColor : arrowColor
        }
    }
}
