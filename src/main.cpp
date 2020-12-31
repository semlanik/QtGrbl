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
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "grblengine.h"
#include "grblserial.h"
#include "grblconsole.h"
#include "grblgcodestate.h"

#include <memory>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    std::shared_ptr<QtGrbl::GrblSerial> serialEngine = std::make_shared<QtGrbl::GrblSerial>();
    std::shared_ptr<QtGrbl::GrblEngine> glrblEngine = std::make_shared<QtGrbl::GrblEngine>();
    std::shared_ptr<QtGrbl::GrblConsole> console = std::make_shared<QtGrbl::GrblConsole>();

    //Make connections between components
    glrblEngine->attach(serialEngine);
    QObject::connect(console.get(), qOverload<const QString &, QtGrbl::CommandPriority>(&QtGrbl::GrblConsole::sendCommand),
                     serialEngine.get(), qOverload<const QString &, QtGrbl::CommandPriority>(&QtGrbl::GrblSerial::sendCommand));
    QObject::connect(serialEngine.get(), &QtGrbl::GrblSerial::commandSent,
                     console.get(), &QtGrbl::GrblConsole::writeCommand);
    QObject::connect(serialEngine.get(), &QtGrbl::GrblSerial::responseReceived,
                     console.get(), &QtGrbl::GrblConsole::writeResponse);

    QtGrbl::qmlRegisterGrblSingleton("GrblSerial", serialEngine);
    QtGrbl::qmlRegisterGrblSingleton("GrblEngine", glrblEngine);
    QtGrbl::qmlRegisterGrblSingleton("GrblConsole", console);

    qmlRegisterUncreatableType<QtGrbl::GrblConsoleRecord>("QtGrbl", 1, 0, "GrblConsoleRecord", "Uncreatable");
    qmlRegisterUncreatableType<QtGrbl::GrblGCodeState>("QtGrbl", 1, 0, "GrblGCodeState", "Uncreatable");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
