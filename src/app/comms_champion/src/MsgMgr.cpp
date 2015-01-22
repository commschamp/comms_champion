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

void MsgMgr::addSocket(SocketPtr socket)
{
    if (!m_sockets.empty()) {
        auto& lastSocket = m_sockets.back();

        disconnect(
            lastSocket.get(), SIGNAL(sigDataReceived(DataInfoPtr)),
            this, SLOT(socketDataReceived(DataInfoPtr)));

        connect(
            lastSocket.get(), SIGNAL(sigDataReceived(DataInfoPtr)),
            socket.get(), SLOT(feedInData(DataInfoPtr)));

        connect(
            socket.get(), SIGNAL(sigDataToSend(DataInfoPtr)),
            lastSocket.get(), SLOT(sendData(DataInfoPtr)));
    }

    connect(
        socket.get(), SIGNAL(sigDataReceived(DataInfoPtr)),
        this, SLOT(socketDataReceived(DataInfoPtr)));

    m_sockets.push_back(std::move(socket));
}

void MsgMgr::setProtocol(ProtocolPtr protocol)
{
    m_protocol = std::move(protocol);
}

ProtocolPtr MsgMgr::getProtocol() const
{
    return m_protocol;
}

void MsgMgr::setRecvEnabled(bool enabled)
{
    m_recvEnabled = enabled;
    if (m_sockets.empty()) {
        return;
    }

    if (enabled) {
        for (auto& s : m_sockets) {
            s->start();
        }
    }
    else {
        for (auto& s : m_sockets) {
            s->stop();
        }
    }
}

void MsgMgr::deleteRecvMsg(MessageInfoPtr msgInfo)
{
    assert(!m_recvMsgs.empty());
    assert(msgInfo);

    auto msgNumFromMsgInfoFunc =
        [](const MessageInfo& info) -> MsgNumberType
        {
            auto msgNumVar = info.getExtraProperty(
                GlobalConstants::msgNumberPropertyName());
            assert(msgNumVar.isValid());
            assert(msgNumVar.canConvert<MsgNumberType>());
            return msgNumVar.value<MsgNumberType>();
        };

    auto msgNum = msgNumFromMsgInfoFunc(*msgInfo);

    auto iter = std::lower_bound(
        m_recvMsgs.begin(),
        m_recvMsgs.end(),
        msgNum,
        [&msgNumFromMsgInfoFunc](const MessageInfoPtr& msgInfoTmp, MsgNumberType val) -> bool
        {
            return msgNumFromMsgInfoFunc(*msgInfoTmp) < val;
        });

    if (iter == m_recvMsgs.end()) {
        assert(!"Deleting non existing message.");
        return;
    }

    m_recvMsgs.erase(iter);
}

void MsgMgr::socketDataReceived(DataInfoPtr dataInfoPtr)
{
    if ((!m_recvEnabled) || !(m_protocol)) {
        return;
    }

    auto msgsList = m_protocol->read(dataInfoPtr);
    if (msgsList.empty()) {
        return;
    }

    for (auto& msgInfo : msgsList) {
        assert(msgInfo->getAppMessage());

        msgInfo->setExtraProperty(
            GlobalConstants::msgNumberPropertyName(),
            QVariant::fromValue(m_nextMsgNum));
        ++m_nextMsgNum;
        assert(0 < m_nextMsgNum); // wrap around is not supported
        emit sigMsgReceived(msgInfo);
    }

    m_recvMsgs.reserve(m_recvMsgs.size() + msgsList.size());
    std::move(msgsList.begin(), msgsList.end(), std::back_inserter(m_recvMsgs));
}

MsgMgr::MsgMgr(QObject* parent)
  : Base(parent)
{
    m_recvMsgs.reserve(1024);
}

}  // namespace comms_champion

