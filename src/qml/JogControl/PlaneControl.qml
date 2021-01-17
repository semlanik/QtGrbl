import QtQuick 2.0
import QtQuick3D 1.15
import QtQuick3D.Materials 1.15

Node {
    id: root
    ListModel {
        id: coneModel
        ListElement {
            positionX: 0
            positionY: -1
            positionZ: 0
            marginMultiplier: 5
            rotationX: 180
            rotationY: 0
            rotationZ: 0
            color: "#759ae8"
        }
        ListElement {
            positionX: 0
            positionY: 1
            positionZ: 0
            marginMultiplier: 5
            rotationX: 0
            rotationY: 0
            rotationZ: 0
            color: "#759ae8"
        }
        ListElement {
            positionX: 1
            positionY: 0
            positionZ: 0
            marginMultiplier: 2
            rotationX: 0
            rotationY: 0
            rotationZ: -90
            color: "#e87575"
        }
        ListElement {
            positionX: -1
            positionY: 0
            positionZ: 0
            marginMultiplier: 2
            rotationX: 0
            rotationY: 0
            rotationZ: 90
            color: "#e87575"
        }
        ListElement {
            positionX: 0
            positionY: 0
            positionZ: 1
            marginMultiplier: 2
            rotationX: 0
            rotationY: 90
            rotationZ: 90
            color: "#75e875"
        }
        ListElement {
            positionX: 0
            positionY: 0
            positionZ: -1
            marginMultiplier: 2
            rotationX: 0
            rotationY: -90
            rotationZ: 90
            color: "#75e875"
        }
    }

    Node {
        Model {
            id: plane
            property real baseSize: 50
            //Assume cube size 50
            scale: Qt.vector3d(6, 0.5, 4)//TODO: might be aligned with parameters registred in Grbl working plane sizes
            source: "#Cube"
            materials: DefaultMaterial  {
                diffuseColor: "grey"
            }
        }


        Repeater3D {
            model: coneModel
            Node {
                eulerRotation: Qt.vector3d(model.rotationX, model.rotationY, model.rotationZ)
                position: Qt.vector3d(model.positionX*(plane.baseSize*plane.scale.x+plane.baseSize*model.marginMultiplier),
                                      model.positionY*(plane.baseSize*plane.scale.y+plane.baseSize*model.marginMultiplier),
                                      model.positionZ*(plane.baseSize*plane.scale.z+plane.baseSize*model.marginMultiplier))
                Model {
                    id: cone

                    signal clicked
                    signal hovered
                    signal endHovered

                    onClicked: root.clicked()
                    onHovered: root.hovered()
                    onEndHovered: root.isHovered = false

                    objectName: "cone"
                    source: "#Cone"
                    pickable: true
                    scale: Qt.vector3d(0.5, 1, 0.5)
                    materials: DefaultMaterial{
                        diffuseColor: model.color
                    }
                }
            }
        }
    }
}
