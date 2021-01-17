import QtQuick 2.0
import QtQuick3D 1.15

Node {
    id: root

    property var rectPos: Qt.vector3d(250, 250, 12)

    Model {
        position: Qt.vector3d(0, 0, 0)
        source: "#Sphere"
        materials: DefaultMaterial {
            diffuseColor: "grey"
        }
    }

    GizmoArrow {
        objectName: "z+"
        eulerRotation: Qt.vector3d(0, 0, 0)
        arrowColor: "blue"

        Node {
            position: rectPos

            Rectangle {
                width: 100
                height: 100
                color: "blue"
            }
        }
    }

    GizmoArrow {
        objectName: "x+"
        eulerRotation: Qt.vector3d(-90, 0, -90)
        arrowColor: "red"

        Node {
            position: rectPos

            Rectangle {
                width: 100
                height: 100
                color: "red"
            }
        }
    }

    GizmoArrow {
        objectName: "y+"
        eulerRotation: Qt.vector3d(0, 90, 90)
//        eulerRotation: Qt.vector3d(0, 45, 115)

        arrowColor: "green"
//        position: Qt.vector3d(0, 0, 40)

        Node {
            position: rectPos
//            eulerRotation: Qt.vector3d(0, -20, 0)

            Rectangle {
                width: 100
                height: 100
                color: "green"
            }
        }
    }
}
