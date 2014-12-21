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

void MsgMgr::addSocket(SocketPtr&& socket)
{
    m_socket = std::move(socket);
    connect(
        m_socket.get(), SIGNAL(sigDataReceived(DataInfoPtr)),
        this, SLOT(socketDataReceived(DataInfoPtr)));
}

void MsgMgr::addProtocol(ProtocolPtr&& protocol)
{
    m_protStack.push_back(std::move(protocol));
}

void MsgMgr::setRecvEnabled(bool enabled)
{
    m_recvEnabled = enabled;
    if (!m_socket) {
        return;
    }

    if (enabled) {
        m_socket->start();
    }
    else {
        m_socket->stop();
    }
}

void MsgMgr::socketDataReceived(DataInfoPtr dataInfoPtr)
{
    if ((m_protStack.empty()) ||
        (!m_recvEnabled)) {
        return;
    }

    auto* buf = &dataInfoPtr->m_data[0];
    const auto bufSize = dataInfoPtr->m_data.size();

    // TODO: process all protocols
    auto& protocol = *m_protStack.back();
    auto allMsgs = protocol.read(&buf[0], bufSize);
    for (auto& msgInfo : allMsgs) {
        assert(msgInfo->getAppMessage());
        msgInfo->setExtraProperty(
            GlobalConstants::msgNumberPropertyName(),
            QVariant::fromValue(m_nextMsgNum));
        ++m_nextMsgNum;

        msgInfo->setProtocolName(protocol.name());

        auto protInfo = makeProtocolsInfo();
        protInfo->push_back(std::move(msgInfo));
        m_recvMsgs.push_back(std::move(protInfo));
        emit sigMsgReceived(m_recvMsgs.back());
    }
}

MsgMgr::MsgMgr(QObject* parent)
  : Base(parent)
{
}

}  // namespace comms_champion

