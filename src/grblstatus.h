/*
 * MIT License
 *
 * Copyright (c) 2021 Alexey Edelev <semlanik@gmail.com>
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

#pragma once

#include "grblabstractdatamodel.h"

#include <QObject>

namespace QtGrbl {

class GrblStatus : public GrblAbstractDataModel
{
    Q_OBJECT
    Q_PROPERTY(GrblState grblState READ grblState NOTIFY grblStateChanged)
    Q_PROPERTY(qreal x READ x NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y NOTIFY yChanged)
    Q_PROPERTY(qreal z READ z NOTIFY zChanged)

public:
    enum GrblState {
        Invalid = -1,
        Idle,
        Run,
        Hold,
        Jog,
        Alarm,
        Door,
        Check,
        Home,
        Sleep
    };
    Q_ENUM(GrblState)

    GrblStatus() = default;
    virtual ~GrblStatus() = default;

    GrblState grblState() const {
        return m_grblState;
    }

    qreal x() const {
        return m_x;
    }

    qreal y() const {
        return m_y;
    }

    qreal z() const {
        return m_z;
    }

public slots:
    void setX(qreal x) {
        if (m_x == x)
            return;

        m_x = x;
        emit xChanged();
    }

    void setY(qreal y) {
        if (m_y == y)
            return;

        m_y = y;
        emit yChanged();
    }

    void setZ(qreal z) {
        if (m_z == z)
            return;

        m_z = z;
        emit zChanged();
    }

signals:
    void grblStateChanged();
    void xChanged();
    void yChanged();
    void zChanged();

protected:
    bool parseData() override;

private:
    void setGrblState(GrblState state) {
        if (state != m_grblState) {
            m_grblState = state;
            emit grblStateChanged();
        }
    }

    void parseState(QByteArray state);
    void parseStatusField(const QByteArray &data);

    GrblState m_grblState;
    qreal m_x;
    qreal m_y;
    qreal m_z;
};

}
