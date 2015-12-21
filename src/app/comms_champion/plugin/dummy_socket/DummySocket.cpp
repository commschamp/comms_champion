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

#include "DummySocket.h"

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QTimer>
CC_ENABLE_WARNINGS()

#include <cassert>

namespace comms_champion
{

DummySocket::DummySocket()
{
    auto* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->start(2000);
}

bool DummySocket::startImpl()
{
    m_running = true;
    return true;
}

void DummySocket::stopImpl()
{
    m_running = false;
}

void DummySocket::sendDataImpl(DataInfoPtr dataPtr)
{
    static_cast<void>(dataPtr);
}

void DummySocket::timeout()
{
    if (!m_running) {
        return;
    }

    static const std::uint8_t Buf[] = {
        0x68, 0x9f, 0x0, 0x3, 0x0, 0x01, 0x02,
        0x68, 0x9f, 0x0, 0x4, 0x1, 0x01, 0x00, 0x13
    };
    static const auto BufSize = std::extent<decltype(Buf)>::value;

    DataInfoPtr dataInfoPtr(new DataInfo());
    dataInfoPtr->m_timestamp = DataInfo::TimestampClock::now();
    assert(dataInfoPtr->m_timestamp != DataInfo::Timestamp());
    dataInfoPtr->m_data.assign(&Buf[0], &Buf[0] + BufSize);
    reportDataReceived(std::move(dataInfoPtr));
}

}  // namespace comms_champion


