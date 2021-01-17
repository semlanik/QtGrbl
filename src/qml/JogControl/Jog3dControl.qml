/*
 * MIT License
 *
 * Copyright (c) 2020 Petr Mironychev <artifeks92@gmail.com>
 *
 * This file is part of QtGrbl project https://github.com/semlanik/qtgrbl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.12
import QtQuick3D 1.15
import QtQuick3D.Helpers 1.15
import QtQuick3D.Effects 1.15

View3D {
    id: root
    property bool type: false
    anchors {
        top: controlType.bottom
        bottom: parent.botton
        left: parent.left
        right: parent.right
    }

    environment: SceneEnvironment {
        antialiasingMode: SceneEnvironment.MSAA
    }

    OrthographicCamera {
        id: mainCamera

        position: Qt.vector3d(0, 0, 500)
        clipNear: 0
        clipFar: 2000
    }

    DirectionalLight {
        brightness: 100
    }

    Node {
        id: control3d
        scale: Qt.vector3d(0.4, 0.4, 0.4)
        eulerRotation: Qt.vector3d(20, -45, -20)

        GizmoControl {
            visible: !root.type
        }
        PlaneControl {
            visible: root.type
        }

// TODO: Loader3D crashes, need to investigate why
//        Loader3D {
//            sourceComponent: root.type ? jogControl : gizmoControl
//        }
//        Component {
//            id: planeControl
//            PlaneControl {
//            }
//        }
//        Component {
//            id: gizmoControl
//            GizmoControl {
//            }
//        }
    }
}
