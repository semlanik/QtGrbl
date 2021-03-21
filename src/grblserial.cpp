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

#include "grblserial.h"

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDebug>

using namespace QtGrbl;

GrblSerial::GrblSerial(QObject *parent) : QObject(parent)
  , m_status(GrblSerial::Idle)
  , m_selectedPort(-1)
{
    updatePortList();
}


GrblSerial::~GrblSerial()
{
    disconnectPort();
}

void GrblSerial::updatePortList()
{
    m_portList.clear();
    emit portListChanged();

    auto ports = QSerialPortInfo::availablePorts();
    for (auto port : ports) {
        m_portList.append(port.portName());
    }
    emit portListChanged();
}

void GrblSerial::connectPort()
{
    GrblSerial::connectPort(m_selectedPort);
}

void GrblSerial::connectPort(int portIndex)
{
    if (m_port) {
        qCritical() << "Port is already opened" << m_port->portName();
        return;
    }

    auto ports = QSerialPortInfo::availablePorts();
    if (portIndex >= ports.size() || portIndex < 0) {
        qCritical() << "invalid port index" << portIndex;
        emit isConnectedChanged();
        return;
    }
    auto portInfo = ports[portIndex];
    qDebug() << "Connecting to " << portInfo.portName();
    m_port = std::make_unique<QSerialPort>(portInfo);
    m_port->setBaudRate(QSerialPort::Baud115200);
    m_port->setFlowControl(QSerialPort::NoFlowControl);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);

    if (!m_port->open(QSerialPort::ReadWrite)) {
        qCritical() << "Unable to open" << m_port->portName() << m_port->errorString();
        m_port.reset();
        emit isConnectedChanged();
        return;
    }

    emit isConnectedChanged();

    QObject::connect(m_port.get(), &QSerialPort::readyRead, this, [this]() {
        while (m_port->canReadLine()) {
            QByteArray grblData = m_port->readLine();
            qDebug() << "Raw data: "  << grblData.toHex();
            qDebug() << "String data: " << grblData;

            auto lastMessage = m_sent.take();

            emit responseReceived(grblData);
            if (grblData == "ok\r\n") {
                m_status = GrblSerial::Idle;
                processQueue();
            } else if (grblData.startsWith("error:")) {
                m_status = GrblSerial::Error;
                qCritical() << "Error occured: " << QString::fromLatin1(grblData);
                qWarning() << "Last command: " << lastMessage;
            }
        }
    });
    QObject::connect(m_port.get(), &QSerialPort::errorOccurred, this, &GrblSerial::onError);
}

void GrblSerial::disconnectPort()
{
    if (m_port) {
        if (m_port->isOpen()) {
            m_port->close();
        }
        m_port.reset();
        emit isConnectedChanged();
    }
    m_queue.clear();
    m_sent.clear();
}

bool GrblSerial::isConnected() const
{
    return m_port && m_port->isOpen();
}

void GrblSerial::onError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) {
        qCritical() << "\"NoError\" error occured, it should n't happen";
        return;
    }

    setStatus(GrblSerial::Error);
    qCritical() << "Error " << error << " occured on serial port. Closing it.";
    disconnectPort();
}

void GrblSerial::sendCommand(const QString &command, QtGrbl::CommandPriority prio)
{
    QByteArray buffer = command.trimmed().toLatin1();
    sendCommand(buffer, prio);
}

void GrblSerial::sendCommand(QByteArrayList commands, QtGrbl::CommandPriority prio)
{
    if (!m_port || !m_port->isOpen()) {
        qCritical() << "Unable to send data. Port is not opened";
        return;
    }

    if (prio == QtGrbl::CommandPriority::Front) {
        std::reverse(std::begin(commands), std::end(commands));
    }

    for (auto command : commands) {
        if (command.startsWith(';') || command.startsWith("(")) {
            qDebug() << "Skip command: " << command;
            continue;
        }

        command = command.trimmed();
        command.append('\n'); /* TODO: add setting to switch carriage
                               * return symbol
                               */

        qDebug() << "Send command: " << command << "Prio: " << prio;
        switch (prio) {
        case QtGrbl::CommandPriority::Immediate:
            if (!m_sent.test(command)) {
                // If last command size > 128 bytes, we cannot send Immediate command, because it would
                // cause buffer overflow in grbl
                m_queue.push_front(command);
            } else {
                write(command);
                // No further queue processing.
                return;
            }
        case QtGrbl::CommandPriority::Front:
            m_queue.push_front(command);
            break;
        default:
            m_queue.push_back(command);
            break;
        }
    }
    processQueue();
}

void GrblSerial::sendCommand(const QByteArray &command, QtGrbl::CommandPriority prio)
{
    sendCommand(QByteArrayList() << command);
}

void GrblSerial::processQueue()
{
    switch (m_status) {
    case GrblSerial::Idle:
        if (m_queue.size() > 0) {
            auto buffer = m_queue.takeFirst();
            if (buffer.size() > GrblMaxCommandLineSize) {
                qCritical() << "Invalid command size: " << buffer.size() << "Maximum command size"
                            << "is" << GrblMaxCommandLineSize << "bytes";
                return;
            }
            write(buffer);
            setStatus(GrblSerial::Busy);
        }
        break;
    case GrblSerial::Error:
        qCritical() << "Machine is in error state, user action required";
    default:
        break;
    }
}

void GrblSerial::write(const QByteArray &buffer)
{
    if (!m_port || !m_port->isOpen()) {
        qCritical() << "Unable to write data. Port is not opened";
        return;
    }

    if (m_port->write(buffer) != buffer.size()) {
        qCritical() << "Unable to write command buffer";
        return;
    }

    emit commandSent(buffer);
    m_sent.push(buffer);
}

void GrblSerial::clearError()
{
    qWarning() << "Manual error unlock triggered";
    setStatus(GrblSerial::Idle);
    processQueue();
}

void GrblSerial::clearCommandQueue()
{
    m_queue.clear();
    m_sent.clear();
    sendCommand(QByteArray("\x85"), CommandPriority::Immediate);
}
