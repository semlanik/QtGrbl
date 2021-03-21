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

#include "grblengine.h"

#include "grblgcodestate.h"
#include "grblserial.h"
#include "grblstatus.h"

#include <QFile>
#include <QTimer>
#include <QUrl>

#include <QDebug>

namespace {
    // 5Hz according to recomendation:
    // https://github.com/gnea/grbl/wiki/Grbl-v1.1-Interface#status-reporting
    constexpr int StatusUpdateInterval = 200;
}

using namespace QtGrbl;

GrblEngine::GrblEngine(QObject *parent) : QObject(parent)
  , m_gcodeState(std::make_unique<GrblGCodeState>())
  , m_grblStatus(std::make_unique<GrblStatus>())
{
    initStatusUpdates();
}

GrblEngine::~GrblEngine()
{
}

void GrblEngine::initStatusUpdates()
{
    m_statusTimer.setInterval(StatusUpdateInterval);
    m_statusTimer.setSingleShot(false);
    connect(&m_statusTimer, &QTimer::timeout, this, [this] {
        emit sendCommand("?", CommandPriority::Immediate);
    });
}

void GrblEngine::setFilePath(const QString &fileUrl)
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

void GrblEngine::start()
{
    auto engine = m_serialEngine.lock();
    if (!engine) {
        qCritical() << "Unable to reset to zero, serial engine is null";
        return;
    }

    if (!m_file.isOpen()) {
        qCritical() << "File is not opened";
        return;
    }

    while (!m_file.atEnd()) {
        emit sendCommand(m_file.readLine(), CommandPriority::Back);
    }
}

void GrblEngine::resetToZero()
{
    auto engine = m_serialEngine.lock();
    if (!engine) {
        qCritical() << "Unable to reset to zero, serial engine is null";
        return;
    }
    emit sendCommand(QByteArray("G92 X0 Y0 Z0"), CommandPriority::Back);
}

void GrblEngine::attach(const std::weak_ptr<GrblSerial> &serialEngine)
{
    if (m_serialEngine.lock()) {
        qCritical() << "Already attached to serial engine";
        return;
    }

    m_serialEngine = serialEngine;
    auto engine = m_serialEngine.lock();

    connect(this, qOverload<const QByteArray &, QtGrbl::CommandPriority>(&GrblEngine::sendCommand),
            engine.get(), qOverload<const QByteArray &, QtGrbl::CommandPriority>(&GrblSerial::sendCommand));
    connect(this, qOverload<const QByteArrayList &, QtGrbl::CommandPriority>(&GrblEngine::sendCommand),
            engine.get(), qOverload<QByteArrayList, QtGrbl::CommandPriority>(&GrblSerial::sendCommand));
    connect(engine.get(), &GrblSerial::responseReceived, this, [this](const QByteArray &response) {
        if (response.startsWith(GCodeStatePrefix)) {
            m_gcodeState->parseRawData(response);
        } else if (response.startsWith(GrblStatusPrefix)) {
            m_grblStatus->parseRawData(response);
        }
    });
}

void GrblEngine::hold()
{
    auto engine = m_serialEngine.lock();
    if (!engine) {
        qCritical() << "Unable to hold, serial engine is null";
        return;
    }
    //Stop Spindle first
    emit sendCommand(QByteArrayList() << "M5" << "!", CommandPriority::Immediate);
}

void GrblEngine::resume()
{
    auto engine = m_serialEngine.lock();
    if (!engine) {
        qCritical() << "Unable to resume, serial engine is null";
        return;
    }
    emit sendCommand(QByteArrayList() << "~" << "?", CommandPriority::Immediate);
    subscribeStatusUpdate();
}

void GrblEngine::stop()
{
    auto engine = m_serialEngine.lock();
    if (!engine) {
        qCritical() << "Unable to resume, serial engine is null";
        return;
    }
    emit sendCommand(QByteArrayList() << "M5" << "M0", CommandPriority::Immediate);
    engine->clearCommandQueue();
}

void GrblEngine::returnToZero()
{
    auto engine = m_serialEngine.lock();
    if (!engine) {
        qCritical() << "Unable to resume, serial engine is null";
        return;
    }
    emit sendCommand(QByteArrayList() << "G90" << "G0 Z5" << "G0 Y0 X0" << "G0 Z0",
                     CommandPriority::Front);
}

void GrblEngine::reset()
{
    emit sendCommand(QByteArray("\x18"), CommandPriority::Immediate);
}

void GrblEngine::updateGCodeState()
{
    m_gcodeState->reset();
    emit sendCommand(QByteArray("$G"), CommandPriority::Immediate);
}

void GrblEngine::subscribeStatusUpdate()
{
    m_statusTimer.start();
}

void GrblEngine::unsubscribeStatusUpdate()
{
    m_statusTimer.stop();
}
