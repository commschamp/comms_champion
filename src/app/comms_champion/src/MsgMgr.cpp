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
#include <algorithm>
#include <iterator>
#include <iostream>

#include <QtCore/QVariant>

#include "GlobalConstants.h"

namespace comms_champion
{

namespace
{

void updateMsgType(MessageInfo& msgInfo, MsgMgr::MsgType type)
{
    assert((type == MsgMgr::MsgType::Received) || (type == MsgMgr::MsgType::Sent));
    msgInfo.setExtraProperty(
        GlobalConstants::msgTypePropertyName(),
        QVariant::fromValue(static_cast<int>(type)));

}

void updateMsgTimestamp(MessageInfo& msgInfo, DataInfo::Timestamp timestamp)
{
    msgInfo.setExtraProperty(
        GlobalConstants::timestampPropertyName(),
        QVariant::fromValue(timestamp));
}

}  // namespace

MsgMgr* MsgMgr::instance()
{
    return &(instanceRef());
}

MsgMgr& MsgMgr::instanceRef()
{
    static MsgMgr mgr;
    return mgr;
}

MsgMgr::~MsgMgr() = default;

void MsgMgr::start()
{
    if (m_running) {
        assert(!"Already running");
        return;
    }

    if (m_socket) {
        m_socket->start();
    }

    m_running = true;
}

void MsgMgr::stop()
{
    if (!m_running) {
        assert(!"Already stopped.");
        return;
    }

    if (m_socket) {
        m_socket->stop();
    }

    m_running = false;
}

void MsgMgr::clear()
{
    if (m_running) {
        assert(!"Still running");
        stop();
    }

    m_socket.reset();
    m_protocol.reset();
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
    if (msgs.empty() || (!m_socket) || (!m_protocol)) {
        return;
    }

    auto dataInfos = m_protocol->write(msgs);
    auto now = DataInfo::TimestampClock::now();

    // TODO: write to the last filter instead of socket
    for (auto& dInfo: dataInfos) {
        dInfo->m_timestamp = now;
        m_socket->sendData(std::move(dInfo));
    }

    for (auto& msgInfo : msgs) {
        auto msgInfoToSend = m_protocol->cloneMessage(*msgInfo);
        updateInternalId(*msgInfoToSend);
        updateMsgType(*msgInfoToSend, MsgType::Sent);
        updateMsgTimestamp(*msgInfoToSend, now);
        m_allMsgs.push_back(msgInfoToSend);
        emit sigMsgAdded(msgInfoToSend);
    }
}

const MsgMgr::MsgsList& MsgMgr::getAllMsgs() const
{
    return m_allMsgs;
}

void MsgMgr::setSocket(SocketPtr socket)
{
    if (!socket) {
        if (m_socket) {
            m_socket->disconnect();
        }

        m_socket.reset();
        return;
    }


    connect(
        socket.get(), SIGNAL(sigDataReceived(DataInfoPtr)),
        this, SLOT(socketDataReceived(DataInfoPtr)));

    connect(
        socket.get(), SIGNAL(sigErrorReport(const QString&)),
        this, SIGNAL(sigErrorReported(const QString&)));

    m_socket = std::move(socket);
}

//void MsgMgr::removeSocket(SocketPtr socket)
//{
//    auto iter = std::find(m_sockets.begin(), m_sockets.end(), socket);
//    if (iter == m_sockets.end()) {
//        assert(!"Removing sockets that wasn't added before");
//        return;
//    }
//
//    do {
//        socket->disconnect();
//        bool prevExists = (iter != m_sockets.begin());
//        auto prevIter = iter;
//        std::advance(prevIter, -1);
//
//        auto nextIter = iter;
//        std::advance(nextIter, 1);
//        bool nextExists = (nextIter != m_sockets.end());
//
//        if (prevExists) {
//            disconnect(
//                (*prevIter).get(), SIGNAL(sigDataReceived(DataInfoPtr)),
//                socket.get(), SLOT(feedInData(DataInfoPtr)));
//        }
//
//        if (nextExists) {
//            disconnect(
//                (*nextIter).get(), SIGNAL(sigDataToSend(DataInfoPtr)),
//                socket.get(), SLOT(sendData(DataInfoPtr)));
//        }
//
//        if (prevExists && nextExists) {
//            connect(
//                (*prevIter).get(), SIGNAL(sigDataReceived(DataInfoPtr)),
//                (*nextIter).get(), SLOT(feedInData(DataInfoPtr)));
//
//            connect(
//                (*nextIter).get(), SIGNAL(sigDataToSend(DataInfoPtr)),
//                (*prevIter).get(), SLOT(sendData(DataInfoPtr)));
//
//            break;
//        }
//
//        if (prevExists) {
//            // Next doesn't exist
//            connect(
//                (*prevIter).get(), SIGNAL(sigDataReceived(DataInfoPtr)),
//                this, SLOT(socketDataReceived(DataInfoPtr)));
//            break;
//        }
//
//    } while (false);
//
//    m_sockets.erase(iter);
//}

void MsgMgr::setProtocol(ProtocolPtr protocol)
{
    m_protocol = std::move(protocol);
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
        updateInternalId(*msgInfo);
        updateMsgType(*msgInfo, MsgType::Received);

        static const DataInfo::Timestamp DefaultTimestamp;
        if (dataInfoPtr->m_timestamp != DefaultTimestamp) {
            updateMsgTimestamp(*msgInfo, dataInfoPtr->m_timestamp);
        }
        else {
            auto now = DataInfo::TimestampClock::now();
            updateMsgTimestamp(*msgInfo, now);
        }
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


}  // namespace comms_champion

