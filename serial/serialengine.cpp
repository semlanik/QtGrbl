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
#include "serialengine.h"

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QList>
#include <QDebug>

SerialPortEngine::SerialPortEngine(QObject *parent) : QObject(parent)
  ,m_status(Disconnected)
{
    updatePortList();
}


SerialPortEngine::~SerialPortEngine()
{
    if (m_port->isOpen()) {
        m_port->close();
    }
}

void SerialPortEngine::updatePortList()
{
    m_portList.clear();
    emit portListChanged();

    auto ports = QSerialPortInfo::availablePorts();
    for (auto port : ports) {
        m_portList.append(port.portName());
    }
    emit portListChanged();
}

void SerialPortEngine::connectPort(int portIndex)
{
    if (m_port) {
        qCritical() << "Port is already opened" << m_port->portName();
        return;
    }

    auto ports = QSerialPortInfo::availablePorts();
    if (portIndex >= ports.size() || portIndex < 0) {
        qCritical() << "invalid port index" << portIndex;
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

    if (m_port->open(QSerialPort::ReadWrite)) {
        connect(m_port.get(), &QSerialPort::readyRead, this, [this](){
            QByteArray grblData = m_port->readAll();
            while (grblData.size() > 0) {
                qDebug() << "Raw data: "  << grblData.toHex();
                m_consoleOutput.append(QString::fromLatin1(grblData));
                emit consoleOutputChanged();

                qDebug() << grblData;
                grblData.clear();
            }
        });
    } else {
        qCritical() << "Unable to open" << m_port->portName() << m_port->errorString();
    }
}

void SerialPortEngine::sendCommand(const QString &command)
{
    qDebug() << "Send command: " << command;
    if (!m_port || !m_port->isOpen()) {
        qCritical() << "Unable to send data. Port is not opened";
        return;
    }
    m_port->write(command.toLatin1() + "\n");
}

void SerialPortEngine::clearOutput()
{
    m_consoleOutput.clear();
    emit consoleOutputChanged();
}
