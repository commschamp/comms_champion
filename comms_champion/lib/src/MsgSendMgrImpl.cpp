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

#include "MsgSendMgrImpl.h"

#include <cassert>

#include "comms_champion/property/message.h"

namespace comms_champion
{

MsgSendMgrImpl::MsgSendMgrImpl()
  : m_timer(this)
{
    connect(
        &m_timer, SIGNAL(timeout()),
        this, SLOT(sendPendingAndWait()));
}

MsgSendMgrImpl::~MsgSendMgrImpl() noexcept = default;

void MsgSendMgrImpl::start(ProtocolPtr protocol, const MessagesList& msgs)
{
    static constexpr bool The_previous_sending_must_be_stopped_first = false;
    static_cast<void>(The_previous_sending_must_be_stopped_first);
    assert(m_msgsToSend.empty() || The_previous_sending_must_be_stopped_first);
    m_protocol = std::move(protocol);
    for (auto& m : msgs) {
        auto clonedMsg = m_protocol->cloneMessage(*m);
        property::message::Delay().copyFromTo(*m, *clonedMsg);
        property::message::DelayUnits().copyFromTo(*m, *clonedMsg);
        property::message::RepeatDuration().copyFromTo(*m, *clonedMsg);
        property::message::RepeatDurationUnits().copyFromTo(*m, *clonedMsg);
        property::message::RepeatCount().copyFromTo(*m, *clonedMsg);
        property::message::Comment().copyFromTo(*m, *clonedMsg);
        auto extraProps = property::message::ExtraInfo().getFrom(*m);
        if (!extraProps.isEmpty()) {
            property::message::ExtraInfo().setTo(std::move(extraProps), *clonedMsg);
            m_protocol->updateMessage(*clonedMsg);
            assert(!property::message::ExtraInfo().getFrom(*clonedMsg).isEmpty());
        }

        m_msgsToSend.push_back(std::move(clonedMsg));
    }
    sendPendingAndWait();
}

void MsgSendMgrImpl::stop()
{
    m_timer.stop();
    m_protocol.reset();
    m_msgsToSend.clear();
}

void MsgSendMgrImpl::sendPendingAndWait()
{
    m_timer.stop();
    auto iter = m_msgsToSend.begin();
    for (; iter != m_msgsToSend.end(); ++iter) {
        auto& msg = *iter;
        assert(msg);
        auto delay = property::message::Delay().getFrom(*msg);
        if (delay != 0U) {
            break;
        }
    }

    decltype(m_msgsToSend) nextMsgsToSend;
    nextMsgsToSend.splice(
        nextMsgsToSend.end(), m_msgsToSend, m_msgsToSend.begin(), iter);

    for (auto& msgToSend : nextMsgsToSend) {
        auto repeatMs = property::message::RepeatDuration().getFrom(*msgToSend);
        auto repeatCount = property::message::RepeatCount().getFrom(*msgToSend, 1U);

        bool reinsert =
            (0U < repeatMs) &&
            ((repeatCount == 0U) || (1U < repeatCount));

        if (reinsert) {
            if (!m_protocol) {
                static constexpr bool Protocol_must_be_valid = false;
                static_cast<void>(Protocol_must_be_valid);
                assert(Protocol_must_be_valid);                 
                continue;
            }

            auto newDelay = repeatMs;
            auto reinsertIter =
                std::find_if(
                    m_msgsToSend.begin(), m_msgsToSend.end(),
                    [&newDelay](MessagePtr mPtr) mutable -> bool
                    {
                        assert(mPtr);
                        auto mDelay = property::message::Delay().getFrom(*mPtr);
                        if (newDelay < mDelay) {
                            return true;
                        }
                        newDelay -= mDelay;
                        return false;
                    });

            if (reinsertIter != m_msgsToSend.end()) {
                auto& msgToUpdate = *reinsertIter;
                assert(msgToUpdate);
                auto mDelay = property::message::Delay().getFrom(*msgToUpdate);
                property::message::Delay().setTo(mDelay - newDelay, *msgToUpdate);
            }

            auto clonedMsg = m_protocol->cloneMessage(*msgToSend);
            auto extraProps = property::message::ExtraInfo().getFrom(*msgToSend);
            if (!extraProps.isEmpty()) {
                property::message::ExtraInfo().setTo(std::move(extraProps), *clonedMsg);
                m_protocol->updateMessage(*clonedMsg);
            }

            std::swap(clonedMsg, msgToSend);
            property::message::Delay().setTo(newDelay, *clonedMsg);

            if (repeatCount != 0) {
                property::message::RepeatCount().setTo(repeatCount - 1, *clonedMsg);
            }

            m_msgsToSend.insert(reinsertIter, std::move(clonedMsg));
        }
    }

    if (!m_msgsToSend.empty()) {
        auto& msg = m_msgsToSend.front();
        assert(msg);
        auto delay = property::message::Delay().getFrom(*msg);
        assert(0 < delay);
        property::message::Delay().setTo(0, *msg);
        m_timer.setSingleShot(true);
        m_timer.start(static_cast<int>(delay));
    }

    if ((!nextMsgsToSend.empty()) && m_sendCallback) {
        m_sendCallback(std::move(nextMsgsToSend));
    }

    if (m_msgsToSend.empty() && m_sendCompleteCallback) {
        m_sendCompleteCallback();
    }
}
}  // namespace comms_champion
