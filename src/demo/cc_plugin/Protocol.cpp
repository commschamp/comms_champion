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

const QString& Protocol::nameImpl() const
{
    static const QString& Str("Demo");
    return Str;
}

Protocol::UpdateStatus Protocol::updateMessageImpl(cc::Message& msg)
{
    auto* castedMsgPtr = dynamic_cast<demo::cc_plugin::Message*>(&msg);
    if (castedMsgPtr == nullptr) {
        assert(!"Should not happen");
        return UpdateStatus::NoChange;
    }

    bool updated = castedMsgPtr->refresh();
    auto parentStatus = Base::updateMessageImpl(msg);
    if (updated) {
        return UpdateStatus::Changed;
    }
    return parentStatus;
}

}  // namespace cc_plugin

}  // namespace demo

}  // namespace comms_champion
