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


#pragma once

#include <memory>
#include <vector>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
CC_ENABLE_WARNINGS()

#include "comms_champion/Message.h"
#include "comms_champion/Protocol.h"
#include "comms_champion/MessageInfo.h"
#include "comms_champion/Socket.h"

namespace comms_champion
{

class MsgMgr
{
public:
    typedef std::vector<MessageInfoPtr> MsgsList;

    typedef DataInfo::Timestamp Timestamp;

    typedef MessageInfo::MsgType MsgType;

    static MsgMgr* instance();
    static MsgMgr& instanceRef();

    ~MsgMgr();

    void start();
    void stop();
    void clear();

    ProtocolPtr getProtocol() const;
    void setRecvEnabled(bool enabled);

    void deleteMsg(MessageInfoPtr msgInfo);
    void deleteAllMsgs();

    void sendMsgs(MsgInfosList&& msgs);

    const MsgsList& getAllMsgs() const;

    void setSocket(SocketPtr socket);
    void setProtocol(ProtocolPtr protocol);

    typedef std::function<void (MessageInfoPtr msgInfo)> MsgAddedCallbackFunc;
    typedef std::function<void (const QString& error)> ErrorReportCallbackFunc;

    void setMsgAddedCallbackFunc(MsgAddedCallbackFunc&& func);
    void setErrorReportCallbackFunc(ErrorReportCallbackFunc&& func);

    void socketDataReceived(DataInfoPtr dataInfoPtr);

private:
    typedef unsigned long long MsgNumberType;

    MsgMgr();
    void updateInternalId(MessageInfo& msgInfo);
    void reportMsgAdded(MessageInfoPtr msgInfo);
    void reportError(const QString& error);

    MsgsList m_allMsgs;
    bool m_recvEnabled = false;

    SocketPtr m_socket;
    ProtocolPtr m_protocol;
    MsgNumberType m_nextMsgNum = 1;
    bool m_running = false;

    MsgAddedCallbackFunc m_msgAddedCallback;
    ErrorReportCallbackFunc m_errorReportCallback;
};

}  // namespace comms_champion
