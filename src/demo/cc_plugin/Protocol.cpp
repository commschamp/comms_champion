//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

#include "Protocol.h"

#include "comms/comms.h"

namespace cc = comms_champion;

namespace comms_champion
{

namespace demo
{

namespace cc_plugin
{

Protocol::~Protocol() = default;

const std::string& Protocol::nameImpl() const
{
    static const std::string Str("Demo");
    return Str;
}

Protocol::UpdateStatus Protocol::updateMessageInfoImpl(cc::MessageInfo& msgInfo)
{
    auto msgPtr = msgInfo.getAppMessage();
    if (!msgPtr) {
        return UpdateStatus::NoChangeToAppMsg;
    }

    auto* castedMsgPtr = dynamic_cast<Message*>(msgPtr.get());
    if (castedMsgPtr == nullptr) {
        return UpdateStatus::NoChangeToAppMsg;
    }

    bool updated = castedMsgPtr->refresh();
    auto parentStatus = Base::updateMessageInfoImpl(msgInfo);
    if (updated) {
        return UpdateStatus::AppMsgWasChanged;
    }
    return parentStatus;
}

}  // namespace cc_plugin

}  // namespace demo

}  // namespace comms_champion
