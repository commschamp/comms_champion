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

#include <memory>
#include <vector>

#include "Api.h"
#include "Message.h"
#include "Protocol.h"
#include "Message.h"
#include "Socket.h"
#include "Filter.h"

namespace comms_champion
{

class MsgMgrImpl;
class CC_API MsgMgr
{
public:
    typedef std::vector<MessagePtr> AllMessages;
    typedef Protocol::MessagesList MessagesList;

    typedef Message::Type MsgType;

    MsgMgr();
    ~MsgMgr();

    void start();
    void stop();
    void clear();

    ProtocolPtr getProtocol() const;
    void setRecvEnabled(bool enabled);

    void deleteMsg(MessagePtr msg);
    void deleteAllMsgs();

    void sendMsgs(MessagesList&& msgs);

    const AllMessages& getAllMsgs() const;
    void addMsgs(const MessagesList& msgs, bool reportAdded = true);

    void setSocket(SocketPtr socket);
    void setProtocol(ProtocolPtr protocol);
    void addFilter(FilterPtr filter);

    typedef std::function<void (MessagePtr msg)> MsgAddedCallbackFunc;
    typedef std::function<void (const QString& error)> ErrorReportCallbackFunc;

    void setMsgAddedCallbackFunc(MsgAddedCallbackFunc&& func);
    void setErrorReportCallbackFunc(ErrorReportCallbackFunc&& func);

private:
    std::unique_ptr<MsgMgrImpl> m_impl;
};

}  // namespace comms_champion
