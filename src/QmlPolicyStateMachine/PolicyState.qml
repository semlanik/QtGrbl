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

import QmlPolicyStateMachine 1.0

DSM.State {
    id: root
    property string name: ""
    property DSM.StateMachine stateMachine: null
    property string previousState: "" //TODO: make private in Qt6
    onEntered: {
        if (root.name == "") {
            console.error("GrblStateMachine: State doesn't have name assigned.");
            //TODO: Return to previous state here
            return;
        }

        StatePolicyCollection.enterState(root.name);
        root.previousState = stateMachine.currentState;
        stateMachine.currentState = root.name
        console.log("GrblStateMachine: Entered: " + root.name)
    }
    onExited: {
        StatePolicyCollection.exitState(root.name);
        stateMachine.currentState = root.previousState;
        console.log("GrblStateMachine: Exited: " + root.name)
    }
}
