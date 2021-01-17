import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick3D 1.15
import QtQuick3D.Helpers 1.15

View3D {
    id: view

    environment: {
        antialiasingMode: SceneEnvironment.ProgressiveAA
        antialiasingQuality: SceneEnvironment.VeryHigh
        temporalAAEnabled: true
        temporalAAStrength: 2.0
    }

    //                PerspectiveCamera {
    //                    id: mainCamera

    //                    position: Qt.vector3d(0, 0, 1500)
    //        //            eulerRotation: Qt.vector3d(-45, 0, 0)
    //                }

    OrthographicCamera {
        id: mainCamera

        position: Qt.vector3d(0, 0, 1500)
        clipNear: 0
        clipFar: 3000
    }

    DirectionalLight {
        position: Qt.vector3d(0, 0, 1500)
        //            eulerRotation: Qt.vector3d(-45, 45, 0)

        //            quadraticFade: 0
        brightness: 100
    }

    //        SimpleGizmo {}

    StaticGizmo {
        scale: Qt.vector3d(0.25, 0.25, 0.25)
        eulerRotation: Qt.vector3d(20, -45, -20)
        //            eulerRotation: Qt.vector3d(20, 0, 0)
    }
}
