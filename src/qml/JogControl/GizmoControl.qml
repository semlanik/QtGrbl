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

import QtQuick 2.0
import QtQuick3D 1.15

Node {
    id: root

    Model {
        position: Qt.vector3d(0, 0, 0)
        source: "#Sphere"
        materials: PlainMaterial {
            baseColor: "grey"
        }
    }

    GizmoArrow {
        objectName: "z+"
        eulerRotation: Qt.vector3d(0, 0, 0)
        arrowColor: "blue"
    }

    GizmoArrow {
        objectName: "x+"
        eulerRotation: Qt.vector3d(-90, 0, -90)
        arrowColor: "red"
    }

    GizmoArrow {
        objectName: "y+"
        eulerRotation: Qt.vector3d(0, 90, 90)
        arrowColor: "green"
    }

    GizmoJogPlane {
        color: "blue"
        eulerRotation: Qt.vector3d(0, 0, 0)
    }

    GizmoJogPlane {
        eulerRotation: Qt.vector3d(-90, 0, -90)
        color: "red"
    }

    GizmoJogPlane {
        eulerRotation: Qt.vector3d(0, 90, 90)
        color: "green"
    }
}
