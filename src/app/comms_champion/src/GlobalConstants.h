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


#pragma once

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

namespace comms_champion
{

class GlobalConstants
{
public:

//    enum class State
//    {
//        Waiting,
//        Running,
//        Sending,
//        SendingAll,
//        NumOfStates // Must be last
//    };

//    static GlobalConstants* instance();

    static const char* indexPropertyName();
    static const char* msgNumberPropertyName();
    static const char* msgObjPropertyName();
    static const char* msgDelayPropertyName();
    static const char* msgDelayUnitsPropertyName();
    static const char* msgRepeatDurationPropertyName();
    static const char* msgRepeatUnitsPropertyName();
    static const char* msgRepeatCountPropertyName();
    static const char* timestampPropertyName();
    static const char* msgTypePropertyName();

private:
    GlobalConstants() = default;
};

} // namespace comms_champion


