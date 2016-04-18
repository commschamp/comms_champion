//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/MsgMgr.h"

namespace comms_champion
{

class MsgMgrImpl
{
public:
    typedef MsgMgr::MsgsList MsgsList;

    typedef MsgMgr::MsgType MsgType;

    MsgMgrImpl();
    ~MsgMgrImpl();

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

    typedef MsgMgr::MsgAddedCallbackFunc MsgAddedCallbackFunc;
    typedef MsgMgr::ErrorReportCallbackFunc ErrorReportCallbackFunc;

    template <typename TFunc>
    void setMsgAddedCallbackFunc(TFunc&& func)
    {
        m_msgAddedCallback = std::forward<TFunc>(func);
    }

    template <typename TFunc>
    void setErrorReportCallbackFunc(TFunc&& func)
    {
        m_errorReportCallback = std::forward<TFunc>(func);
    }

private:
    typedef unsigned long long MsgNumberType;

    void socketDataReceived(DataInfoPtr dataInfoPtr);
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
