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

#include "ProtocolsStack.h"

namespace comms_champion
{

void ProtocolsStack::addProtocol(ProtocolPtr&& protocol)
{
    m_protocols.push_back(std::move(protocol));
}

ProtocolsStack::ProtocolsInfoPtrList ProtocolsStack::processSocketData(
    DataInfoPtr dataInfoPtr)
{
    ProtocolsInfoPtrList allProtocolsInfos;

    if (m_protocols.empty()) {
        return allProtocolsInfos;
    }

    // TODO: process all protocols
    auto& protocol = *m_protocols.back();
    auto allMsgs = protocol.read(std::move(dataInfoPtr));
    for (auto& msgInfo : allMsgs) {

        msgInfo->setProtocolName(protocol.name());

        auto protInfo = makeProtocolsInfo();
        protInfo->push_back(std::move(msgInfo));
        allProtocolsInfos.push_back(std::move(protInfo));
    }

    return allProtocolsInfos;
}

}  // namespace comms_champion


