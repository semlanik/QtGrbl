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
import QmlPolicyStateMachine 1.0

PolicyStateMachine {
    id: root
    initialState: disconnected
    running: true
    signal toggleConnect()
    PolicyState {
        id: disconnected
        stateMachine: root
        name: "disconnected"
        onEntered: {
            GrblSerial.updatePortList();
        }

        DSM.SignalTransition {
            targetState: connecting
            signal: root.toggleConnect
        }
    }

    //Grbl initialization
    PolicyState {
        id: connecting
        name: "connecting"
        stateMachine: root
        initialState: openPort
        PolicyState {
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
            DSM.SignalTransition {
                targetState: disconnected
                signal: GrblSerial.isConnectedChanged
                guard: !GrblSerial.isConnected
            }
        }
        PolicyState { //Give grbl time to initialize
            id: portReady
            name: "portReady"
            stateMachine: root
            DSM.TimeoutTransition {
                targetState: readSettings
                timeout: 3000
            }
        }
        PolicyState {
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
        PolicyState {
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
        DSM.SignalTransition {
            targetState: disconnecting
            signal: root.toggleConnect
            guard: GrblSerial.isConnected
        }
    }

    //Grbl working mode
    PolicyState {
        id: connected
        name: "connected"
        stateMachine: root
        initialState: idle
        onEntered: {
            GrblEngine.subscribeStatusUpdate();
        }
        onExited: {
            GrblEngine.unsubscribeStatusUpdate();
        }
        DSM.SignalTransition {
            targetState: idle
            signal: GrblEngine.grblStatus.grblStateChanged
            guard: GrblEngine.grblStatus.grblState === GrblStatus.Idle
        }
        DSM.SignalTransition {
            targetState: running
            signal: GrblEngine.grblStatus.grblStateChanged
            guard: GrblEngine.grblStatus.grblState === GrblStatus.Run
        }
        DSM.SignalTransition {
            targetState: alarm
            signal: GrblEngine.grblStatus.grblStateChanged
            guard: GrblEngine.grblStatus.grblState === GrblStatus.Alarm
        }
        DSM.SignalTransition {
            targetState: hold
            signal: GrblEngine.grblStatus.grblStateChanged
            guard: GrblEngine.grblStatus.grblState === GrblStatus.Hold
        }
        PolicyState {
            id: idle
            name: "idle"
            stateMachine: root
        }
        PolicyState {
            id: hold
            name: "hold"
            stateMachine: root
            onEntered: {
                GrblEngine.unsubscribeStatusUpdate();
            }
        }
        PolicyState {
            id: running
            name: "running"
            stateMachine: root
        }
        PolicyState {
            id: alarm
            name: "alarm"
            stateMachine: root
        }
        DSM.SignalTransition {
            targetState: disconnecting
            signal: root.toggleConnect
            guard: GrblSerial.isConnected
        }
    }

    PolicyState {
        id: error
        name: "error"
        stateMachine: root
    }

    PolicyState {
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

    DSM.FinalState {
        id: exit
    }
//    onFinished: Qt.quit()
}
