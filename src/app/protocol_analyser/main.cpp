//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <QtWidgets/QApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickWindow>

#include "comms_champion/protocol.h"

namespace
{

namespace ccp = comms_champion::protocol;

void qmlRegisterAll()
{
    ccp::MsgMgr::qmlRegister();
    ccp::Message::qmlRegister();
}

}  // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterAll();

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/MainWindow.qml"));
    QObject *topLevel = engine.rootObjects().value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
    window->showMaximized();
    QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit())); // to make Qt.quit() to work.

    return app.exec();
}

