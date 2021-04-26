//
// Copyright 2016 - 2021 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include <functional>

#include "Api.h"
#include "Message.h"
#include "Protocol.h"

namespace comms_champion
{

class MsgSendMgrImpl;
class CC_API MsgSendMgr
{
public:
    typedef Protocol::MessagesList MessagesList;
    typedef std::function<void (MessagesList&&)> SendMsgsCallbackFunc;
    typedef std::function<void ()> SendCompleteCallbackFunc;

    MsgSendMgr();
    ~MsgSendMgr() noexcept;

    void setSendMsgsCallbackFunc(SendMsgsCallbackFunc&& func);
    void setSendCompeteCallbackFunc(SendCompleteCallbackFunc&& func);

    void start(ProtocolPtr protocol, const MessagesList& msgs);

    void stop();

private:
    std::unique_ptr<MsgSendMgrImpl> m_impl;
};

}  // namespace comms_champion


