/*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>
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

import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {
    id: root
    property alias headerText: _headerText.text
    property alias content: _toolList.model
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
        id: _header
        Text {
            id: _headerText //Should display active settings name
        }

        Button {
            text: "X"
        }
    }

    Column {
        anchors.top: _header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        Repeater {
            id: _toolList
            anchors.fill: parent
        }
    }
}
