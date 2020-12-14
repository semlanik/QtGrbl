#pragma once

#include <QObject>
#include <QStringList>
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
#include <memory>

class QSerialPort;

class SerialPortEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList portList READ portList NOTIFY portListChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString consoleOutput READ consoleOutput NOTIFY consoleOutputChanged)
    Q_ENUMS(Status)
public:
    enum Status {
        Disconnected,
        Connected,
        Progress,
        Paused
    };

    static SerialPortEngine *instance() {
        static SerialPortEngine _instance;
        return &_instance;
    }
    Q_INVOKABLE void updatePortList();
    Q_INVOKABLE void connectPort(int portIndex);
    Q_INVOKABLE void sendCommand(const QString &command);
    Q_INVOKABLE void clearOutput();

    QStringList portList() const {
        return m_portList;
    }

    Status status() const {
        return m_status;
    }

    QString consoleOutput() const
    {
        return m_consoleOutput;
    }
signals:
    void portListChanged();
    void statusChanged();
    void consoleOutputChanged();

private:
    explicit SerialPortEngine(QObject *parent = nullptr);
    virtual ~SerialPortEngine();

    QStringList m_portList;
    Status m_status;
    std::unique_ptr<QSerialPort> m_port;
    QString m_consoleOutput;
};
