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

void MsgMgr::start()
{
    for (auto& s : m_sockets) {
        s->start();
    }
}

void MsgMgr::stop()
{
    for (auto& s : m_sockets) {
        s->stop();
    }
}

ProtocolPtr MsgMgr::getProtocol() const
{
    return m_protocol;
}

void MsgMgr::setRecvEnabled(bool enabled)
{
    m_recvEnabled = enabled;
}

void MsgMgr::deleteMsg(MessageInfoPtr msgInfo)
{
    assert(!m_allMsgs.empty());
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
        m_allMsgs.begin(),
        m_allMsgs.end(),
        msgNum,
        [&msgNumFromMsgInfoFunc](const MessageInfoPtr& msgInfoTmp, MsgNumberType val) -> bool
        {
            return msgNumFromMsgInfoFunc(*msgInfoTmp) < val;
        });

    if (iter == m_allMsgs.end()) {
        assert(!"Deleting non existing message.");
        return;
    }

    m_allMsgs.erase(iter);
}

void MsgMgr::deleteAllMsgs()
{
    m_allMsgs.clear();
}

void MsgMgr::sendMsgs(const MsgInfosList& msgs)
{
    if (msgs.empty()) {
        return;
    }

    assert(!m_sockets.empty());
    auto& lastSocket = m_sockets.back();

    auto dataInfos = m_protocol->write(msgs);
    for (auto& dInfo: dataInfos) {
        lastSocket->sendData(std::move(dInfo));
    }

    for (auto& msgInfo : msgs) {
        auto msgInfoToSend = m_protocol->cloneMessage(*msgInfo);
        updateInternalId(*msgInfoToSend);
        updateMsgType(*msgInfoToSend, MsgType::Sent);
        m_allMsgs.push_back(msgInfoToSend);
        emit sigMsgAdded(msgInfoToSend);
    }
}

const MsgMgr::MsgsList& MsgMgr::getAllMsgs() const
{
    return m_allMsgs;
}

void MsgMgr::socketDataReceived(DataInfoPtr dataInfoPtr)
{
    if ((!m_recvEnabled) || !(m_protocol)) {
        return;
    }

    assert(dataInfoPtr);
    auto msgsList = m_protocol->read(*dataInfoPtr);
    if (msgsList.empty()) {
        return;
    }

    for (auto& msgInfo : msgsList) {
        assert(msgInfo->getAppMessage());
        updateInternalId(*msgInfo);
        updateMsgType(*msgInfo, MsgType::Received);
        emit sigMsgAdded(msgInfo);
    }

    m_allMsgs.reserve(m_allMsgs.size() + msgsList.size());
    std::move(msgsList.begin(), msgsList.end(), std::back_inserter(m_allMsgs));
}

MsgMgr::MsgMgr(QObject* parent)
  : Base(parent)
{
    m_allMsgs.reserve(1024);
}

void MsgMgr::updateInternalId(MessageInfo& msgInfo)
{
    msgInfo.setExtraProperty(
        GlobalConstants::msgNumberPropertyName(),
        QVariant::fromValue(m_nextMsgNum));
    ++m_nextMsgNum;
    assert(0 < m_nextMsgNum); // wrap around is not supported
}

void MsgMgr::updateMsgType(MessageInfo& msgInfo, MsgType type)
{
    assert((type == MsgType::Received) || (type == MsgType::Sent));
    msgInfo.setExtraProperty(
        GlobalConstants::msgTypePropertyName(),
        QVariant::fromValue(static_cast<int>(type)));

}

}  // namespace comms_champion

