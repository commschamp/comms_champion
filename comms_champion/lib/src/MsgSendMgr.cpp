//
// Copyright 2016 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/MsgSendMgr.h"

#include "MsgSendMgrImpl.h"

namespace comms_champion
{

MsgSendMgr::MsgSendMgr()
  : m_impl(new MsgSendMgrImpl())
{
}

MsgSendMgr::~MsgSendMgr() noexcept = default;

void MsgSendMgr::setSendMsgsCallbackFunc(SendMsgsCallbackFunc&& func)
{
    m_impl->setSendMsgsCallbackFunc(std::move(func));
}

void MsgSendMgr::setSendCompeteCallbackFunc(SendCompleteCallbackFunc&& func)
{
    m_impl->setSendCompleteCallbackFunc(std::move(func));
}

void MsgSendMgr::start(ProtocolPtr protocol, const MessagesList& msgs)
{
    m_impl->start(std::move(protocol), msgs);
}

void MsgSendMgr::stop()
{
    m_impl->stop();
}

}  // namespace comms_champion

