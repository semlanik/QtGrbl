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

#include <QObject>
#include <QByteArray>

namespace QtGrbl {
class GrblGCodeState : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)

    Q_PROPERTY(int toolNumber READ toolNumber NOTIFY toolNumberChanged)
    Q_PROPERTY(qreal spindleSpeed READ spindleSpeed NOTIFY spindleSpeedChanged)
    Q_PROPERTY(qreal feedRate READ feedRate NOTIFY feedRateChanged)
    Q_PROPERTY(QtGrbl::GrblGCodeState::MotionMode motionMode READ motionMode WRITE setMotionMode NOTIFY motionModeChanged)
    Q_PROPERTY(QtGrbl::GrblGCodeState::PlaneSelect planeSelect READ planeSelect WRITE setPlaneSelect NOTIFY planeSelectChanged)
    Q_PROPERTY(QtGrbl::GrblGCodeState::DistanceMode distanceMode READ distanceMode WRITE setDistanceMode NOTIFY distanceModeChanged)
    Q_PROPERTY(QtGrbl::GrblGCodeState::FeedRateMode feedRateMode READ feedRateMode WRITE setFeedRate NOTIFY feedRateModeChanged)
    Q_PROPERTY(QtGrbl::GrblGCodeState::UnitsMode unitsMode READ unitsMode WRITE setUnitsMode NOTIFY unitsModeChanged)
    Q_PROPERTY(QtGrbl::GrblGCodeState::ToolLengthOffset toolLengthOffset READ toolLengthOffset WRITE setToolLengthOffset NOTIFY toolLengthOffsetChanged)
    Q_PROPERTY(QtGrbl::GrblGCodeState::ProgramMode programMode READ programMode WRITE setProgramMode NOTIFY programModeChanged)
    Q_PROPERTY(QtGrbl::GrblGCodeState::SpindleState spindleState READ spindleState WRITE setSpindleState NOTIFY spindleStateChanged)
    Q_PROPERTY(QtGrbl::GrblGCodeState::CoolantState coolantState READ coolantState WRITE setCoolantState NOTIFY coolantStateChanged)
    Q_PROPERTY(bool cutterRadiusCompensation READ cutterRadiusCompensation WRITE setCutterRadiusCompensation NOTIFY cutterRadiusCompensationChanged)
    Q_PROPERTY(bool arcIJKDistanceMode READ arcIJKDistanceMode WRITE setArcIJKDistanceMode NOTIFY arcIJKDistanceModeChanged)

public:
    enum MotionMode {
        RapidPositioning,
        LinearInterpolation,
        CircularInterpolationCW,
        CircularInterpolationCCW,
        CancelCannedCycle
    };
    Q_ENUM(MotionMode)

    enum PlaneSelect {
        XYPlane,
        ZXPlane,
        YZPlane
    };
    Q_ENUM(PlaneSelect)

    enum DistanceMode {
        AbsoluteProgramming,
        IncrementalProgramming
    };
    Q_ENUM(DistanceMode)

    enum FeedRateMode {
        InverseTime,
        PerMinute
    };
    Q_ENUM(FeedRateMode)

    enum UnitsMode {
        Inches,
        Millimeters
    };
    Q_ENUM(UnitsMode)

    enum ToolLengthOffset {
        CompensationNegative,
        CompensationCancel
    };
    Q_ENUM(ToolLengthOffset)

    enum ProgramMode {
        CompulsoryStop,
        OptionalStop,
        EndOfProgram,
        EndOfProgramWithReturn
    };
    Q_ENUM(ProgramMode)

    enum SpindleState {
        SpindleOnCW,
        SpindleOnCCW,
        SpindleStop
    };
    Q_ENUM(SpindleState)

    enum CoolantState {
        CoolantMist,
        CoolantFlood,
        CoolantOff
    };
    Q_ENUM(CoolantState)

    explicit GrblGCodeState(const QByteArray &stateData = {}, QObject *parent = nullptr);
    virtual ~GrblGCodeState() = default;

    GrblGCodeState &fromRawData(const QByteArray& stateData) {
        parse(stateData);
        return *this;
    }

    int toolNumber() const {
        return m_toolNumber;
    }

    qreal spindleSpeed() const {
        return m_spindleSpeed;
    }

    qreal feedRate() const {
        return m_feedRate;
    }

    MotionMode motionMode() const {
        return m_motionMode;
    }

    PlaneSelect planeSelect() const {
        return m_planeSelect;
    }

    DistanceMode distanceMode() const {
        return m_distanceMode;
    }

    FeedRateMode feedRateMode() const {
        return m_feedRateMode;
    }

    UnitsMode unitsMode() const {
        return m_unitsMode;
    }

    ToolLengthOffset toolLengthOffset() const {
        return m_toolLengthOffset;
    }

    ProgramMode programMode() const {
        return m_programMode;
    }

    SpindleState spindleState() const {
        return m_spindleState;
    }

    CoolantState coolantState() const {
        return m_coolantState;
    }

    bool cutterRadiusCompensation() const {
        return m_cutterRadiusCompensation;
    }

    bool arcIJKDistanceMode() const {
        return m_arcIJKDistanceMode;
    }

    bool isValid() const {
        return !m_raw.isEmpty();
    }

public slots:
    void setToolNumber(int toolNumber) {
        if (m_toolNumber == toolNumber)
            return;

        m_toolNumber = toolNumber;
        emit toolNumberChanged();
    }

    void setSpindleSpeed(qreal spindleSpeed) {
        qWarning("Floating point comparison needs context sanity check");
        if (qFuzzyCompare(m_spindleSpeed, spindleSpeed))
            return;

        m_spindleSpeed = spindleSpeed;
        emit spindleSpeedChanged();
    }

    void setFeedRate(qreal feedRate) {
        qWarning("Floating point comparison needs context sanity check");
        if (qFuzzyCompare(m_feedRate, feedRate))
            return;

        m_feedRate = feedRate;
        emit feedRateChanged();
    }

    void setMotionMode(MotionMode motionMode) {
        if (m_motionMode == motionMode)
            return;

        m_motionMode = motionMode;
        emit motionModeChanged();
    }

    void setPlaneSelect(PlaneSelect planeSelect) {
        if (m_planeSelect == planeSelect)
            return;

        m_planeSelect = planeSelect;
        emit planeSelectChanged();
    }

    void setDistanceMode(DistanceMode distanceMode) {
        if (m_distanceMode == distanceMode)
            return;

        m_distanceMode = distanceMode;
        emit distanceModeChanged();
    }

    void setFeedRateMode(FeedRateMode feedRateMode) {
        if (m_feedRateMode == feedRateMode)
            return;

        m_feedRateMode = feedRateMode;
        emit feedRateModeChanged();
    }

    void setUnitsMode(UnitsMode unitsMode) {
        if (m_unitsMode == unitsMode)
            return;

        m_unitsMode = unitsMode;
        emit unitsModeChanged();
    }

    void setToolLengthOffset(ToolLengthOffset toolLengthOffset) {
        if (m_toolLengthOffset == toolLengthOffset)
            return;

        m_toolLengthOffset = toolLengthOffset;
        emit toolLengthOffsetChanged();
    }

    void setProgramMode(ProgramMode programMode) {
        if (m_programMode == programMode)
            return;

        m_programMode = programMode;
        emit programModeChanged();
    }

    void setSpindleState(SpindleState spindleState) {
        if (m_spindleState == spindleState)
            return;

        m_spindleState = spindleState;
        emit spindleStateChanged();
    }

    void setCoolantState(CoolantState coolantState) {
        if (m_coolantState == coolantState)
            return;

        m_coolantState = coolantState;
        emit coolantStateChanged();
    }

    void setCutterRadiusCompensation(bool cutterRadiusCompensation) {
        if (m_cutterRadiusCompensation == cutterRadiusCompensation)
            return;

        m_cutterRadiusCompensation = cutterRadiusCompensation;
        emit cutterRadiusCompensationChanged();
    }

    void setArcIJKDistanceMode(bool arcIJKDistanceMode) {
        if (m_arcIJKDistanceMode == arcIJKDistanceMode)
            return;

        m_arcIJKDistanceMode = arcIJKDistanceMode;
        emit arcIJKDistanceModeChanged();
    }

signals:
    void isValidChanged();
    void toolNumberChanged();
    void spindleSpeedChanged();
    void feedRateChanged();
    void motionModeChanged();
    void planeSelectChanged();
    void distanceModeChanged();
    void feedRateModeChanged();
    void unitsModeChanged();
    void toolLengthOffsetChanged();
    void programModeChanged();
    void spindleStateChanged();
    void coolantStateChanged();
    void cutterRadiusCompensationChanged();
    void arcIJKDistanceModeChanged();

private:
    void parse(QByteArray stateData);

    int m_toolNumber;
    qreal m_spindleSpeed;
    qreal m_feedRate;
    MotionMode m_motionMode;
    PlaneSelect m_planeSelect;
    DistanceMode m_distanceMode;
    FeedRateMode m_feedRateMode;
    UnitsMode m_unitsMode;
    ProgramMode m_programMode;
    SpindleState m_spindleState;
    CoolantState m_coolantState;
    bool m_cutterRadiusCompensation;
    bool m_arcIJKDistanceMode;

    QByteArray m_raw;
    ToolLengthOffset m_toolLengthOffset;
};
}

//Q_DECLARE_METATYPE(QtGrbl::GrblGCodeState)
