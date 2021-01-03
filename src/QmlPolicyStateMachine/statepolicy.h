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

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QList>

class StatePolicyAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString allowed READ allowed WRITE setAllowed NOTIFY allowedChanged)
    Q_PROPERTY(QString forbidden READ forbidden WRITE setForbidden NOTIFY forbiddenChanged)
public:
    StatePolicyAttached(QObject *parent) : QObject(parent) {
    }

    QString allowed() const {
        return m_allowed;
    }

    QString forbidden() const {
        return m_forbidden;
    }

public slots:
    void setAllowed(QString allowed) {
        if (m_allowed == allowed)
            return;

        m_allowed = allowed;
        emit allowedChanged(m_allowed);
    }

    void setForbidden(QString forbidden) {
        if (m_forbidden == forbidden)
            return;

        m_forbidden = forbidden;
        emit forbiddenChanged(m_forbidden);
    }

signals:
    void allowedChanged(QString allowed);
    void forbiddenChanged(QString forbidden);

private:
    QString m_allowed;
    QString m_forbidden;
};

class StatePolicyCollection : public QObject
{
    Q_OBJECT
public:
    StatePolicyCollection() = default;
    static StatePolicyCollection *instance() {
        static StatePolicyCollection _instance;
        return &_instance;
    }
    Q_INVOKABLE void enterState(const QString &state);
    Q_INVOKABLE void exitState(const QString &state);

    void add(StatePolicyAttached *policy) {
        m_policies.append(policy);
        execPolicy(policy);
    }

private:
    Q_DISABLE_COPY_MOVE(StatePolicyCollection)

    void execPolicy(StatePolicyAttached *policy);

    QList<QPointer<StatePolicyAttached>> m_policies;
    QList<QString> m_stateList;
};

class StatePolicy : public QObject
{
    Q_OBJECT
    QML_ATTACHED(StatePolicyAttached)
public:
    static StatePolicyAttached *qmlAttachedProperties(QObject *object) {
        auto policy = new StatePolicyAttached(object);
        StatePolicyCollection::instance()->add(policy);
        return policy;
    }
};

