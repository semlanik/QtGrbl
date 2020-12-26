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

#pragma once

#include <QObject>
#include <QStringList>
#include <memory>
#include <QQueue>

#include "qtgrblcommon.h"

class QSerialPort;

namespace QtGrbl {

class GrblSerial : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList portList READ portList NOTIFY portListChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
public:
    enum Status {
        Idle, //! No command is sent, no response awaiting
        Busy, //! Command is sent, awating for response
        Error //! Error occured, clear error is required
    };
    Q_ENUM(Status)

    explicit GrblSerial(QObject *parent = nullptr);
    virtual ~GrblSerial();

    Q_INVOKABLE void updatePortList();
    Q_INVOKABLE void connectPort(int portIndex);
    Q_INVOKABLE void clearError();

    void sendCommand(const QString &command, QtGrbl::CommandPriority prio = QtGrbl::CommandPriority::Back);
    void sendCommand(QByteArray command, QtGrbl::CommandPriority prio = QtGrbl::CommandPriority::Back);

    QStringList portList() const {
        return m_portList;
    }

    Status status() const {
        return m_status;
    }

signals:
    void responseReceived(const QByteArray &response);
    void commandSent(const QByteArray &command);

    void portListChanged();
    void statusChanged();

private:
    void processQueue();
    void write(const QByteArray &buffer);

    QStringList m_portList;
    Status m_status;

    std::unique_ptr<QSerialPort> m_port;
    QQueue<QByteArray> m_queue;
    QByteArray m_lastCommand;
};

}