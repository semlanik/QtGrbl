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
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import QtQml.Models 2.1
import QtQuick.Controls.Material 2.12

import QtGrbl 1.0


import QmlPolicyStateMachine 1.0
import "GrblStateMachine"
import "JogControl"

Window {
    width: 640
    height: 480
    title: qsTr("QtGrbl")

    Material.theme: Universal.Dark
    Material.accent: Universal.Violet

    SideToolbar {
        id: leftToolbar
        position: "left"
        content: ObjectModel {
            GCodeStateView {}
            GrblStatusView {}
        }
    }

    SideToolbar {
        id: rightToolbar
        position: "right"
        content: ObjectModel {
            ConsoleOutput {
                model: GrblConsole
                width: parent.width
                height: 500
            }
            Item {
                height: 500
                width: parent.width
                Switch { //TODO: replace with multistate button(like radio button)
                    id: controlType
                    text: "Control type"
                    checked: true
                }
                Jog3dControl {
                    id: jog
                    type: controlType.checked
                    anchors.top: controlType.top
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                }
            }
        }
    }

    FileDialog {
        id: fileSelection
        selectMultiple: false
        selectFolder: false
        onAccepted: {
            GrblEngine.filePath = fileSelection.fileUrl;//TODO: not a SerialEngine functionality
        }
    }

    TextField {
        id: consoleInput
        focus: true
        objectName: "consoleInput"
        StatePolicy.allowed: "idle|error|alarm"
        onActiveFocusChanged: {
            forceActiveFocus()
        }

        width: parent.width/4
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }

        Keys.onUpPressed: {
            consoleInput.text = GrblConsole.moveHistoryUp()
        }

        Keys.onDownPressed: {
            consoleInput.text = GrblConsole.moveHistoryDown()
        }

        onTextEdited: {
            GrblConsole.recentInput = text;
        }

        onAccepted: {
            GrblConsole.sendCommand(consoleInput.text)
            consoleInput.text = "";
        }
    }

    ToolBar {
        id: toolBar
        anchors.left: leftToolbar.right
        anchors.right: rightToolbar.left
        Row {
            ComboBox {
                id: portSelector
                model: GrblSerial.portList
                StatePolicy.allowed: "disconnected"
                onCurrentIndexChanged: {
                    GrblSerial.selectedPort = portSelector.currentIndex
                }
            }
            Button {
                text: "Update"
                StatePolicy.allowed: "disconnected"
                onClicked: {
                    GrblSerial.updatePortList()
                }
            }
            Button {
                text: stateMachine.currentState === "disconnected" ? "Connect" : "Disconnect"
                onClicked: {
                    stateMachine.toggleConnect()
                }
            }
            Button {
                text: "Clear"
                StatePolicy.allowed: "connected"
                onClicked: {
                    GrblConsole.clear()
                }
            }
            Button {
                text: "Clear error"
                StatePolicy.allowed: "error"
                onClicked: {
                    GrblSerial.clearError()
                }
            }
            Button {
                text: "Select file"
                StatePolicy.allowed: "idle"
                onClicked: {
                    fileSelection.open()
                }
            }
            Button {
                text: "Start"
                visible: enabled
                StatePolicy.allowed: "idle"
                onClicked: {
                    GrblEngine.start();
                }
            }
            Button {
                text: "Stop"
                visible: enabled
                StatePolicy.allowed: "running"
                onClicked: {
                    GrblEngine.stop();
                }
            }
            Button {
                objectName: "holdButton"
                text: "Hold"
                StatePolicy.forbidden: "hold|alarm"
                onClicked: {
                    GrblEngine.hold();
                }
            }
            Button {
                text: "Resume"
                StatePolicy.allowed: "hold"
                onClicked: {
                    GrblEngine.resume();
                }
            }
            Button {
                text: "Reset to 0"
                StatePolicy.allowed: "idle"
                onClicked: {
                    GrblEngine.resetToZero();
                }
            }
            Button {
                text: "Return to 0"
                StatePolicy.allowed: "idle"
                onClicked: {
                    GrblEngine.returnToZero();
                }
            }
        }
    }

    GrblStateMachine {
        id: stateMachine
    }

    Component.onCompleted: {
        showMaximized()
    }
}
