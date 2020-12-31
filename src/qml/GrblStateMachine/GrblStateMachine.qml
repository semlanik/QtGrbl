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
import QtQuick 2.0
import QtQml.StateMachine 1.0 as DSM

import QtGrbl 1.0

DSM.StateMachine {
    id: root
    initialState: disconnected
    running: true
    property string currentState: ""
    signal toggleConnect()
    TraceableState {
        id: disconnected
        stateMachine: root
        name: "disconnected"
        onEntered: {
            console.log("Entered level 2" + name)
        }

        DSM.SignalTransition {
            targetState: connecting
            signal: root.toggleConnect
        }
    }

    //Grbl initialization
    TraceableState {
        id: connecting
        name: "connecting"
        stateMachine: root
        initialState: openPort
        TraceableState {
            id: openPort
            name: "openPort"
            stateMachine: root
            onEntered: {
                GrblSerial.connectPort()
            }
            DSM.SignalTransition {
                targetState: portReady
                signal: GrblSerial.isConnectedChanged
                guard: GrblSerial.isConnected
            }
        }
        TraceableState { //Give grbl time to initialize
            id: portReady
            name: "portReady"
            stateMachine: root
            DSM.TimeoutTransition {
                targetState: readSettings
                timeout: 3000
            }
        }
        TraceableState {
            id: readSettings
            name: "readSettings"
            stateMachine: root
            onEntered: {
                //TODO: GrblSettings.update()
            }
            //TODO: Transit to next state once settings ready
            //DSM.SignalTransition {
            //    targetState: readStatus
            //    signal: GrblSettings.isValidChanged
            //    guard: GrblSettings.isValid
            //}
            DSM.TimeoutTransition {
                targetState: readGCodeState
                timeout: 100
            }
        }
        TraceableState {
            id: readGCodeState
            name: "readGCodeState"
            stateMachine: root
            onEntered: {
                GrblEngine.updateGCodeState();
            }
            DSM.SignalTransition {
                targetState: connected
                signal: GrblEngine.gcodeState.isValidChanged
                guard: GrblEngine.gcodeState.isValid
            }
        }
    }

    //Grbl working mode
    TraceableState {
        id: connected
        name: "connected"
        stateMachine: root
        initialState: idle
        TraceableState {
            id: idle
            name: "idle"
            stateMachine: root
        }
        TraceableState {
            id: progress
            name: "progress"
            stateMachine: root
            initialState: running
            TraceableState {
                id: running
                name: "running"
                stateMachine: root
            }
            TraceableState {
                id: hold
                name: "hold"
                stateMachine: root
            }
            TraceableState {
                id: alert
                name: "alert"
                stateMachine: root
            }
        }
        TraceableState {
            id: error
            name: "error"
            stateMachine: root
        }

        DSM.SignalTransition {
            targetState: disconnecting
            signal: root.toggleConnect
            guard: GrblSerial.isConnected
        }
    }

    TraceableState {
        id: disconnecting
        name: "disconnecting"
        stateMachine: root
        onEntered: {
            GrblSerial.disconnectPort();
        }
        DSM.SignalTransition {
            targetState: disconnected
            signal: GrblSerial.isConnectedChanged
            guard: !GrblSerial.isConnected
        }
    }

    Connections {
        target: GrblSerial
        onIsConnectedChanged: {
            console.log("GrblSerial: " + GrblSerial.isConnected)
        }
    }

    DSM.FinalState {
        id: exit
    }
//    onFinished: Qt.quit()
}
