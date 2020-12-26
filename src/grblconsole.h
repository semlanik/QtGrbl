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

#include <QFile>

#include "universallistmodel.h"
#include "grblconsolerecord.h"
#include "qtgrblcommon.h"

namespace QtGrbl {

class GrblConsole : public UniversalListModel<GrblConsoleRecord>
{
    Q_OBJECT
    Q_PROPERTY(bool saveToFile READ saveToFile WRITE setSaveToFile NOTIFY saveToFileChanged)

public:
    GrblConsole(QObject *parent = nullptr);
    virtual ~GrblConsole() = default;

    Q_INVOKABLE void save(const QString &filePath);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void sendCommand(const QString &command) {
        //For user command default priority is Front
        sendCommand(command, QtGrbl::CommandPriority::Front);
    }

    void writeCommand(const QByteArray &buffer);
    void writeResponse(const QByteArray &buffer);

    bool saveToFile() const {
        return m_logFile.isOpen();
    }

    void setSaveToFile(bool saveToFile) {
        if (m_logFile.isOpen() && !saveToFile) {
            m_logFile.close();
        } else if(!m_logFile.isOpen() && saveToFile) {
            m_logFile.open(QFile::WriteOnly);
        }

        emit saveToFileChanged();
    }

signals:
    //! Do not use this method directly
    void sendCommand(const QString &command, QtGrbl::CommandPriority prio);
    void saveToFileChanged();

private:
    QFile m_logFile;
    bool m_saveToFile;
};

}

