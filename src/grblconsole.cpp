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

#include "grblconsole.h"

#include <QDateTime>

using namespace QtGrbl;

GrblConsole::GrblConsole(QObject *parent) : UniversalListModel({}, parent)
  , m_inputHistoryPointer(m_inputHistory.end())
{
}

void GrblConsole::writeCommand(const QByteArray &buffer)
{
    append(new GrblConsoleRecord(GrblConsoleRecord::Command, QDateTime::currentDateTime(),
                                 QString::fromLatin1(buffer)));
}

void GrblConsole::writeResponse(const QByteArray &buffer)
{
    auto type = GrblConsoleRecord::Response;
    if (buffer.startsWith(QtGrbl::GrblStatusPrefix)) {
        if (m_noStatus) {
            return;
        }
        type = GrblConsoleRecord::Status;
    }
    append(new GrblConsoleRecord(type, QDateTime::currentDateTime(),
                                 QString::fromLatin1(buffer)));
}

void GrblConsole::clear()
{
    reset({});
}

void GrblConsole::save(const QString &filePath)
{
    //TODO:
}
