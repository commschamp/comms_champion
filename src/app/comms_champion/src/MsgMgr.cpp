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


#include "MsgMgr.h"

#include <cassert>

#include <QtCore/QTimer>
#include <QtCore/QVariant>

#include "GlobalConstants.h"

namespace comms_champion
{

MsgMgr* MsgMgr::instance()
{
    return &(instanceRef());
}

MsgMgr& MsgMgr::instanceRef()
{
    static MsgMgr mgr;
    return mgr;
}

void MsgMgr::timeout()
{
    if ((m_protStack.empty()) ||
        (!m_recvEnabled)) {
        return;
    }

    static const std::uint8_t Buf[] = {
        0x0, 0x3, 0x0, 0x01, 0x02
        ,0x0, 0x4, 0x1, 0x01, 0x00, 0x13
    };
    static const auto BufSize = std::extent<decltype(Buf)>::value;

    auto& protocol = *m_protStack.back();
    auto allMsgs = protocol.read(&Buf[0], BufSize);
    for (auto& msgInfo : allMsgs) {
        assert(msgInfo->getAppMessage());
        msgInfo->setExtraProperty(
            GlobalConstants::msgNumberPropertyName(),
            QVariant::fromValue(m_nextMsgNum));
        ++m_nextMsgNum;
        m_recvMsgs.push_back(std::move(msgInfo));
        emit sigMsgReceived(m_recvMsgs.back());
    }
}

void MsgMgr::addProtocol(ProtocolPtr&& protocol)
{
    m_protStack.push_back(std::move(protocol));
}

void MsgMgr::setRecvEnabled(bool enabled)
{
    m_recvEnabled = enabled;
}

MsgMgr::MsgMgr(QObject* parent)
  : Base(parent)
{
    auto* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->start(2000);
}

}  // namespace comms_champion

