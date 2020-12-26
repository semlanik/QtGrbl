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

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import QtGrbl 1.0
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3

Window {
    width: 640
    height: 480
    title: qsTr("Hello World")
    ToolBar {
        id: toolBar
        Row {
            ComboBox {
                id: portSelector
                model: SerialEngine.portList
            }
            Button {
                text: "Connect" //TODO: Check state machine state here ? "Connect" : "Disconnect"
                onClicked: {
                    SerialEngine.connectPort(portSelector.currentIndex)
                }
            }
            Button {
                text: "Clear"
                onClicked: {
                    SerialEngine.clearOutput()
                }
            }
            Button {
                text: "Clear error"
                onClicked: {
                    SerialEngine.clearError()
                }
            }
            Button {
                text: "Select file"
                onClicked: {
                    fileSelection.open()
                }
            }
            Button {
                text: "Start"
                onClicked: {
                    SerialEngine.start();//TODO: not a SerialEngine functionality
                }
            }
            Button {
                text: "Reset to 0"
                onClicked: {
                    SerialEngine.resetToZero();//TODO: not a SerialEngine functionality
                }
            }
        }
    }

    ConsoleOutput {
        text: SerialEngine.consoleOutput
        anchors {
            left: parent.left
            right: parent.right
            top: toolBar.bottom
            bottom: consoleInput.top
        }
    }

    FileDialog {
        id: fileSelection
        selectMultiple: false
        selectFolder: false
        onAccepted: {
            SerialEngine.filePath = fileSelection.fileUrl;//TODO: not a SerialEngine functionality
        }
    }

    TextField {
        id: consoleInput
        focus: true
        onActiveFocusChanged: {
            forceActiveFocus()
        }

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        onAccepted: {
            SerialEngine.sendCommand(consoleInput.text)
            consoleInput.text = "";
        }
    }
    Component.onCompleted: {
        showMaximized()
    }
}
