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

#include "grblstatus.h"

#include <QDebug>
#include <QByteArrayList>
#include <QMetaEnum>

using namespace QtGrbl;

bool GrblStatus::parseData()
{
    QByteArray statusData = m_raw.mid(1, statusData.size() - 2);
    QByteArrayList statusDataList = statusData.split('|');
    parseState(statusDataList.takeFirst());
    for (auto data : statusDataList) {
        parseStatusField(data);
    }
    return true;
}

void GrblStatus::parseState(QByteArray stateData) {
    stateData = stateData.split(':').first();
    qDebug() << "stateData: " << stateData;
    int index = metaObject()->indexOfEnumerator("GrblState");
    Q_ASSERT_X(index >= 0 && index < metaObject()->enumeratorCount(),
               "GrblStatus", "GrblState is not found in meta object");

    bool ok = false;
    QMetaEnum enumerator = metaObject()->enumerator(index);
    int value = enumerator.keyToValue(stateData.data(), &ok);

    if (!ok) {
        qWarning() << "Invalid grbl state: " << stateData;
        setGrblState(Invalid);
        return;
    }
    setGrblState(static_cast<GrblStatus::GrblState>(value));
}

void GrblStatus::parseStatusField(const QByteArray &data) {
    auto dataList = data.split(':');
    if (dataList.size() != 2) {
        qCritical() << "Invalid grbl status data: " << dataList;
        return;
    }

    auto key = dataList.at(0);
    auto value = dataList.at(1);

    if (key == "MPos") {
        auto coords = value.split(',');
        if (coords.size() < 0 || coords.at(0).isEmpty()) {
            qWarning() << "MPos contains no data";
        }
        setX(std::atof(coords.at(0).data()));
        if (coords.size() > 0 && !coords.at(1).isEmpty()) {
            setY(std::atof(coords.at(1).data()));
        }
        if (coords.size() > 1 && !coords.at(2).isEmpty()) {
            setZ(std::atof(coords.at(2).data()));
        }
    } else {
        qWarning() << "Unhandled status field: " << key;
    }
}
