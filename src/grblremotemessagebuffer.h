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

#include <QByteArray>

#include "qtgrblcommon.h"

namespace QtGrbl {

/*!
 * \brief The GrblRemoteMessageBuffer class is simple FIFO queue that controls remote buffer
 * overflow
 */
class GrblRemoteMessageBuffer
{
public:
    GrblRemoteMessageBuffer() = default;
    virtual ~GrblRemoteMessageBuffer() = default;

    bool test(const QByteArray &command) {
        return (m_buffer.size() + command.size()) < GrblMaxCommandLineSize;
    }

    void push(const QByteArray &buffer) {
        m_buffer.append(buffer);
    }

    QByteArray take() {
        int lastMessageLen = m_buffer.indexOf('\n') + 1;
        if (lastMessageLen > 0) {
            auto lastMessage = m_buffer.mid(0, lastMessageLen);
            if (lastMessageLen < m_buffer.size()) {
                m_buffer = m_buffer.mid(lastMessageLen);
            } else {
                clear();
            }
            return lastMessage;
        }
        clear();
        return QByteArray();
    }

    int size() const {
        return m_buffer.size();
    }

    void clear() {
        m_buffer.clear();
    }

private:
    QByteArray m_buffer;
};

}
