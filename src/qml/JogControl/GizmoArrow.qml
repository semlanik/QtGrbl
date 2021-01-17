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
        scale: Qt.vector3d(0.5, 1, 0.5)
        materials: DefaultMaterial {
            diffuseColor: isHovered ? hoverColor : arrowColor
        }
    }
}
