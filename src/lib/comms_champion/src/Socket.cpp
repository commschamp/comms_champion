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

#include "comms_champion/Socket.h"

namespace comms_champion
{

Socket::Socket() = default;
Socket::~Socket() = default;

bool Socket::start()
{
    return startImpl();
}

void Socket::stop()
{
    stopImpl();
}

void Socket::sendData(DataInfoPtr dataPtr)
{
    sendDataImpl(std::move(dataPtr));
}

bool Socket::startImpl()
{
    return true;
}

void Socket::stopImpl()
{
}

void Socket::reportDataReceived(DataInfoPtr dataPtr)
{
    if (m_dataReceivedCallback) {
        m_dataReceivedCallback(std::move(dataPtr));
    }
}

void Socket::reportError(const QString& msg)
{
    if (m_errorReportCallback) {
        m_errorReportCallback(msg);
    }
}

}  // namespace comms_champion
