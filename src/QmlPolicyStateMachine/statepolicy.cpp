/*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>
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

#include "statepolicy.h"

#include <QQuickItem>
#include <QPointer>

void StatePolicyCollection::enterState(const QString &state) {
    m_stateList.append(state);
    for (auto policy : m_policies) {
        execPolicy(policy);
    }
}

void StatePolicyCollection::exitState(const QString &state) {
    m_stateList.removeAll(state);
    for (auto policy : m_policies) {
        execPolicy(policy);
    }
}


void StatePolicyCollection::execPolicy(StatePolicyAttached *policy)
{
    if (policy == nullptr) {
        return;
    }

    auto item = qobject_cast<QQuickItem *>(policy->parent());
    if (item == nullptr) {
        return;
    }

    QStringList allowed = policy->allowed().split("|", Qt::SkipEmptyParts);
    QStringList forbidden = policy->forbidden().split("|", Qt::SkipEmptyParts);

    bool result = allowed.isEmpty() || allowed.first().isEmpty();
    for (auto policy : allowed) {
        qDebug() << "check" <<  policy << "in " << m_stateList;
        if (m_stateList.contains(policy)) {
            result = true;
            break;
        }
    }

    for (auto policy : forbidden) {
        qDebug() << "check" <<  policy << "in " << m_stateList;
        if (m_stateList.contains(policy)) {
            result = false;
            break;
        }
    }

    qDebug() << "Execute policy for states: " << item->objectName() << "stateList:" << m_stateList
             << " allowed: " << allowed << " forbidden: " << forbidden
             << "result: " << result;
    item->setEnabled(result);
}
