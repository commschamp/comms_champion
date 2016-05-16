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


#include "comms_champion/MsgMgr.h"

#include "MsgMgrImpl.h"

namespace comms_champion
{

MsgMgr::MsgMgr()
  : m_impl(new MsgMgrImpl())
{
}

MsgMgr::~MsgMgr() = default;

void MsgMgr::start()
{
    m_impl->start();
}

void MsgMgr::stop()
{
    m_impl->stop();
}

void MsgMgr::clear()
{
    m_impl->clear();
}

ProtocolPtr MsgMgr::getProtocol() const
{
    return m_impl->getProtocol();
}

void MsgMgr::setRecvEnabled(bool enabled)
{
    m_impl->setRecvEnabled(enabled);
}

void MsgMgr::deleteMsg(MessagePtr msg)
{
    m_impl->deleteMsg(std::move(msg));
}

void MsgMgr::deleteAllMsgs()
{
    m_impl->deleteAllMsgs();
}

void MsgMgr::sendMsgs(MessagesList&& msgs)
{
    m_impl->sendMsgs(std::move(msgs));
}

const MsgMgr::AllMessages& MsgMgr::getAllMsgs() const
{
    return m_impl->getAllMsgs();
}

void MsgMgr::setSocket(SocketPtr socket)
{
    m_impl->setSocket(std::move(socket));
}

void MsgMgr::setProtocol(ProtocolPtr protocol)
{
    m_impl->setProtocol(std::move(protocol));
}

void MsgMgr::setMsgAddedCallbackFunc(MsgAddedCallbackFunc&& func)
{
    m_impl->setMsgAddedCallbackFunc(std::move(func));
}

void MsgMgr::setErrorReportCallbackFunc(ErrorReportCallbackFunc&& func)
{
    m_impl->setErrorReportCallbackFunc(std::move(func));
}

}  // namespace comms_champion

