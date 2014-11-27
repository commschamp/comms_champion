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

#include "GlobalConstants.h"

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QtQml>


namespace comms_champion
{

namespace
{

QObject *getGlobalConstants(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return GlobalConstants::instance();
}

}  // namespace

GlobalConstants* GlobalConstants::instance()
{
    static GlobalConstants* obj = new GlobalConstants();
    return obj;
}

void GlobalConstants::qmlRegister()
{
    qmlRegisterSingletonType<GlobalConstants>("cc.GlobalConstants", 1, 0, "GlobalConstants", &getGlobalConstants);
}

const QString& GlobalConstants::background()
{
    static const QString str("lightgray");
    return str;
}

const QString& GlobalConstants::mapState(State value)
{
    static const QString UnknownStr("UNKNOWN");
    static const QString WaitingStr("WAITING");
    static const QString RunningStr("RUNNING");
    static const QString SendingStr("SENDING");
    static const QString SendingAllStr("SENDING_ALL");
    static const QString* Map[] = {
        &WaitingStr,
        &RunningStr,
        &SendingStr,
        &SendingAllStr
    };

    static_assert(std::extent<decltype(Map)>::value == static_cast<int>(State::NumOfStates),
        "State enum has changed");

    if (State::NumOfStates <= value) {
        return UnknownStr;
    }
    return *Map[static_cast<int>(value)];
}

const QString& GlobalConstants::waitingState()
{
    return mapState(State::Waiting);
}

const QString& GlobalConstants::runningState()
{
    return mapState(State::Running);
}

const QString& GlobalConstants::sendingState()
{
    return mapState(State::Sending);
}

const QString& GlobalConstants::sendingAllState()
{
    return mapState(State::SendingAll);
}

const QString& GlobalConstants::startIconPathFromQml()
{
    static const QString str("../image/start.png");
    return str;
}

const QString& GlobalConstants::startAllIconPathFromQml()
{
    static const QString str("../image/start_all.png");
    return str;
}

const QString& GlobalConstants::stopIconPathFromQml()
{
    static const QString str("../image/stop.png");
    return str;
}

const QString& GlobalConstants::saveIconPathFromQml()
{
    static const QString str("../image/save.png");
    return str;
}

const QString& GlobalConstants::configIconPathFromQml()
{
    static const QString str("../image/config.png");
    return str;
}

const char* GlobalConstants::indexPropertyName()
{
    static const char* str = "index";
    return str;
}

const char* GlobalConstants::namePropertyName()
{
    static const char* str = "name";
    return str;
}

const char* GlobalConstants::msgNumberPropertyName()
{
    static const char* str = "msg_num";
    return str;
}

const char* GlobalConstants::msgObjPropertyName()
{
    static const char* str = "msg_obj";
    return str;
}

GlobalConstants::GlobalConstants(QObject* parent)
  : Base(parent)
{
}

}  // namespace comms_champion


