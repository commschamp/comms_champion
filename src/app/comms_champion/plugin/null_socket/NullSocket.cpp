//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "NullSocket.h"

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QTimer>
CC_ENABLE_WARNINGS()

#include <cassert>

namespace comms_champion
{

NullSocket::NullSocket() = default;
NullSocket::~NullSocket() = default;

bool NullSocket::startImpl()
{
    if (m_running) {
        return false;
    }

    m_running = true;
    return true;
}

void NullSocket::stopImpl()
{
    m_running = false;
}

void NullSocket::sendDataImpl(DataInfoPtr dataPtr)
{
    static_cast<void>(dataPtr);
}

}  // namespace comms_champion


