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

//TODO: Move to other place
#include <QFile>
#include <QUrl>
//TODO:end Move to other place

#include "serialstatemachine.h"

SerialPortEngine::SerialPortEngine(QObject *parent) : QObject(parent)
  ,m_status(SerialPortEngine::Idle)
  ,m_stateMachine(std::make_unique<SerialStateMachine>(this))
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
            while(m_port->canReadLine()) {
                QByteArray grblData = m_port->readLine();
                qDebug() << "Raw data: "  << grblData.toHex();
                m_consoleOutput.append(QString::fromLatin1(grblData));
                emit consoleOutputChanged();
                qDebug() << grblData;

                if (grblData == "ok\r\n") {
                    m_status = SerialPortEngine::Idle;
                    processQueue();
                } else if (grblData.startsWith("error:")) {
                    m_status = SerialPortEngine::Error;
                    qCritical() << "Error occured: " << QString::fromLatin1(grblData);
                    qWarning() << "Last command: " << m_lastCommand;
                }
            }
        });
    } else {
        qCritical() << "Unable to open" << m_port->portName() << m_port->errorString();
    }
}

void SerialPortEngine::sendCommand(const QString &command)
{
    QByteArray buffer = command.trimmed().toLatin1() + "\n";//TODO: add setting to switch carriage
                                                            //return symbol
    sendCommand(buffer);
}

void SerialPortEngine::sendCommand(QByteArray command)
{
    if (!m_port || !m_port->isOpen()) {
        qCritical() << "Unable to send data. Port is not opened";
        return;
    }

    if (command.startsWith(';') || command.startsWith("(")) {
        qDebug() << "Skip command: " << command;
        return;
    }

    if (command.endsWith("\r\n") || command.endsWith("\n\r")) {
        command.resize(command.size() - 1);
        command[command.size() - 1] = '\n';//TODO: add setting to switch carriage
                                           //return symbol
    }

    qDebug() << "Send command: " << command;
    m_queue.push_back(command);
    processQueue();
}

void SerialPortEngine::processQueue()
{
    switch (m_status) {
    case SerialPortEngine::Idle: {
        if (m_queue.size() > 0) {
            auto buffer = m_queue.takeFirst();
            if (buffer.size() > 128) {
                qCritical() << "Invalid command size: " << buffer.size() << "Maximum command size"
                            << "is 128 bytes";
                return;
            }
            qDebug() << "Enqueue next command: " << buffer;
            m_lastCommand = buffer;
            m_port->write(buffer);
            m_status = SerialPortEngine::Busy;
        }
    }
        break;
    case SerialPortEngine::Error:
        qCritical() << "Machine is in error state, user action required";
    default:
        break;
    }
}

void SerialPortEngine::clearError()
{
    qWarning() << "Manual error unlock triggered";
    m_status = SerialPortEngine::Idle;
    processQueue();
}

void SerialPortEngine::clearOutput()
{
    m_consoleOutput.clear();
    emit consoleOutputChanged();
}

void SerialPortEngine::setFilePath(const QString &fileUrl)
{
    QString filePath = QUrl(fileUrl).toLocalFile();
    m_file.close();
    m_file.setFileName("");
    if (!QFile::exists(filePath)) {
        qCritical() << "File doesn't exists" << filePath;
        emit filePathChanged();
        return;
    }

    m_file.setFileName(filePath);
    if (!m_file.open(QFile::ReadOnly)) {
        qCritical() << "Unable to open file" << filePath;
        m_file.setFileName("");
        emit filePathChanged();
        return;
    }
}

void SerialPortEngine::start()
{
    if (!m_file.isOpen()) {
        qCritical() << "File is not opened";
        return;
    }
    while (!m_file.atEnd()) {
        sendCommand(m_file.readLine());
    }
}

void SerialPortEngine::resetToZero()
{
    sendCommand(QByteArray("G92 X0 Y0 Z0\n"));
}
