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

#include "grblgcodestate.h"
#include "qtgrblcommon.h"

#include <string>
#include <unordered_map>

using namespace QtGrbl;

namespace {
template<typename T>
std::pair<QByteArray, QVariant> make_pair(const char *code, T value) {
    return {code, QVariant::fromValue<T>(value)};
}

static std::unordered_map<QByteArray, QVariant> valueMap = {
    make_pair("G0", GrblGCodeState::RapidPositioning)
    ,make_pair("G1", GrblGCodeState::LinearInterpolation)
    ,make_pair("G2", GrblGCodeState::CircularInterpolationCW)
    ,make_pair("G3", GrblGCodeState::CircularInterpolationCCW)
    ,make_pair("G38.2", -1) // Not supported yet
    ,make_pair("G38.3", -1) // Not supported yet
    ,make_pair("G38.4", -1) // Not supported yet
    ,make_pair("G38.5", -1) // Not supported yet
    ,make_pair("G80", GrblGCodeState::CancelCannedCycle)
    ,make_pair("G54", -1) // Not supported yet
    ,make_pair("G55", -1) // Not supported yet
    ,make_pair("G56", -1) // Not supported yet
    ,make_pair("G57", -1) // Not supported yet
    ,make_pair("G58", -1) // Not supported yet
    ,make_pair("G59", -1) // Not supported yet
    ,make_pair("G17", GrblGCodeState::XYPlane)
    ,make_pair("G18", GrblGCodeState::ZXPlane)
    ,make_pair("G19", GrblGCodeState::YZPlane)
    ,make_pair("G90", GrblGCodeState::AbsoluteProgramming)
    ,make_pair("G91", GrblGCodeState::IncrementalProgramming)
    ,make_pair("G91.1", true)
    ,make_pair("G93", GrblGCodeState::InverseTime)
    ,make_pair("G94", GrblGCodeState::PerMinute)
    ,make_pair("G20", GrblGCodeState::Inches)
    ,make_pair("G21", GrblGCodeState::Millimeters)
    ,make_pair("G40", true)
    ,make_pair("M0", GrblGCodeState::CompulsoryStop)
    ,make_pair("M1", GrblGCodeState::OptionalStop)
    ,make_pair("M2", GrblGCodeState::EndOfProgram)
    ,make_pair("M30", GrblGCodeState::EndOfProgramWithReturn)
    ,make_pair("M3", GrblGCodeState::SpindleOnCW)
    ,make_pair("M4", GrblGCodeState::SpindleOnCCW)
    ,make_pair("M5", GrblGCodeState::SpindleStop)
    ,make_pair("M7", GrblGCodeState::CoolantMist)
    ,make_pair("M8", GrblGCodeState::CoolantFlood)
    ,make_pair("M9", GrblGCodeState::CoolantOff)
};
static std::unordered_map<QByteArray, QByteArray> propertyMap = {{"G0", "motionMode"}
                                                                 ,{"G1", "motionMode"}
                                                                 ,{"G2", "motionMode"}
                                                                 ,{"G3", "motionMode"}
                                                                 ,{"G38.2", ""} // Not supported yet
                                                                 ,{"G38.3", ""} // Not supported yet
                                                                 ,{"G38.4", ""} // Not supported yet
                                                                 ,{"G38.5", ""} // Not supported yet
                                                                 ,{"G80", "motionMode"}
                                                                 ,{"G54", ""} // Not supported yet
                                                                 ,{"G55", ""} // Not supported yet
                                                                 ,{"G56", ""} // Not supported yet
                                                                 ,{"G57", ""} // Not supported yet
                                                                 ,{"G58", ""} // Not supported yet
                                                                 ,{"G59", ""} // Not supported yet
                                                                 ,{"G17", "planeSelect"}
                                                                 ,{"G18", "planeSelect"}
                                                                 ,{"G19", "planeSelect"}
                                                                 ,{"G90", "distanceMode"}
                                                                 ,{"G91", "distanceMode"}
                                                                 ,{"G91.1", "arcIJKDistanceMode"}
                                                                 ,{"G93", "feedRateMode"}
                                                                 ,{"G94", "feedRateMode"}
                                                                 ,{"G20", "unitsMode"}
                                                                 ,{"G21", "unitsMode"}
                                                                 ,{"G40", "cutterRadiusCompensation"}
                                                                 ,{"M0", "programMode"}
                                                                 ,{"M1", "programMode"}
                                                                 ,{"M2", "programMode"}
                                                                 ,{"M30", "programMode"}
                                                                 ,{"M3", "spindleState"}
                                                                 ,{"M4", "spindleState"}
                                                                 ,{"M5", "spindleState"}
                                                                 ,{"M7", "coolantState"}
                                                                 ,{"M8", "coolantState"}
                                                                 ,{"M9", "coolantState"}};
}

GrblGCodeState::GrblGCodeState(const QByteArray &stateData, QObject *parent) : GrblAbstractDataModel(parent)
  , m_motionMode(CancelCannedCycle)
  , m_planeSelect(XYPlane)
  , m_distanceMode(AbsoluteProgramming)
  , m_arcIJKDistanceMode(true)
  , m_feedRate(PerMinute)
  , m_unitsMode(Millimeters)
  , m_cutterRadiusCompensation(true)
  , m_toolLengthOffset(CompensationCancel)
  , m_programMode(CompulsoryStop)
  , m_spindleSpeed(SpindleStop)
  , m_coolantState(CoolantOff)
{
    qDebug() << "Parse GrblGCodeState: " << stateData;
    parseRawData(stateData);
}

bool GrblGCodeState::parseData()
{
    auto stateData = m_raw;
    qDebug() << "Parse GrblGCodeState: " << stateData;

    if (!stateData.startsWith(GCodeStatePrefix)) {
        return false;
    }

    m_raw = stateData;
    stateData = stateData.trimmed();
    int dataLen = stateData.size() - GCodeStatePrefixLen - GCodeStatePostfixLen; //
    if (dataLen <= 0) {
        return false;
    }

    stateData = stateData.mid(GCodeStatePrefixLen, dataLen);
    auto params = stateData.split(' ');
    for (auto param : params) {
        if (param.isEmpty()) {
            continue;
        }

        bool ok = false;
        switch (param[0]) {
        case 'S': {
            qreal val = param.mid(1, param.size() - 1).toDouble(&ok);
            if (ok) {
                setSpindleSpeed(val);
            } else {
                qWarning() << "Invalid spindle speed value received from machine: " << param;
            }
        }
            continue;
        case 'F': {
            qreal val = param.mid(1, param.size() - 1).toDouble(&ok);
            if (ok) {
                setFeedRate(val);
            } else {
                qWarning() << "Invalid feed rate value received from machine: " << param;
            }
        }
            continue;
        case 'T': {
            qreal val = param.mid(1, param.size() - 1).toInt(&ok);
            if (ok) {
                setToolNumber(val);
            } else {
                qWarning() << "Invalid tool number received from machine: " << param;
            }
        }
            continue;
        default:
            break;
        }

        auto found = valueMap.find(param);
        if (found == std::end(valueMap)) {
            qWarning() << "Unknown paramerter received in G-code state string" << param;
            continue;
        }

        auto propertyName = propertyMap.find(param);
        if (propertyName == std::end(propertyMap)) {
            qWarning() << "Paramerter is not found in property map" << param;
            continue;
        }

        if (propertyName->second.isEmpty()) {
            qWarning() << "Paramerter is not supported" << param;
            continue;
        }

        qDebug() << "Parameter: " << param << " is mapped to " << propertyName->second << found->second;
        setProperty(propertyName->second.data(), found->second);
    }

    return true;
}
