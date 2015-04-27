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

#include <memory>


namespace comms_champion
{

//GlobalConstants* GlobalConstants::instance()
//{
//    static std::unique_ptr<GlobalConstants> obj(new GlobalConstants());
//    return obj.get();
//}

const char* GlobalConstants::indexPropertyName()
{
    static const char* str = "index";
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

const char* GlobalConstants::msgDelayPropertyName()
{
    static const char* str = "msg_delay";
    return str;
}

const char* GlobalConstants::msgDelayUnitsPropertyName()
{
    static const char* str = "msg_delay_units";
    return str;
}

const char* GlobalConstants::msgRepeatDurationPropertyName()
{
    static const char* str = "msg_repeat_duration";
    return str;
}

const char* GlobalConstants::msgRepeatUnitsPropertyName()
{
    static const char* str = "msg_repeat_units";
    return str;
}

const char* GlobalConstants::msgRepeatCountPropertyName()
{
    static const char* str = "msg_repeat_count";
    return str;
}

const char* GlobalConstants::timestampPropertyName()
{
    static const char* str = "timestamp";
    return str;
}

const char* GlobalConstants::msgTypePropertyName()
{
    static const char* str = "msg_type";
    return str;
}

}  // namespace comms_champion


