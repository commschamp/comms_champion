//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

namespace comms_champion
{

MsgSendMgrImpl::MsgSendMgrImpl()
  : m_timer(this)
{
    connect(
        &m_timer, SIGNAL(timeout()),
        this, SLOT(sendPendingAndWait()));
}

MsgSendMgrImpl::~MsgSendMgrImpl() = default;

void MsgSendMgrImpl::start(ProtocolPtr protocol, const MsgInfosList& msgs)
{
    assert(m_msgsToSend.empty() || !"The previous sending must be stopped first.");
    m_protocol = std::move(protocol);
    for (auto& m : msgs) {
        auto clonedMsg = m_protocol->cloneMessage(*m);
        clonedMsg->setAllProperties(m->getAllProperties());
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
        auto& msgInfo = *iter;
        assert(msgInfo);
        auto delay = msgInfo->getDelay();
        if (delay != 0U) {
            break;
        }
    }

    MsgInfosList nextMsgsToSend;
    nextMsgsToSend.splice(
        nextMsgsToSend.end(), m_msgsToSend, m_msgsToSend.begin(), iter);

    for (auto& msgToSend : nextMsgsToSend) {
        auto repeatMs = msgToSend->getRepeatDuration();
        auto repeatCount = msgToSend->getRepeatCount();

        bool reinsert =
            (0U < repeatMs) &&
            ((repeatCount == 0U) || (1U < repeatCount));

        if (reinsert) {
            if (!m_protocol) {
                assert(!"Expecting protocol to be valid");
                continue;
            }

            auto newDelay = repeatMs;
            auto reinsertIter =
                std::find_if(
                    m_msgsToSend.begin(), m_msgsToSend.end(),
                    [&newDelay](MessageInfoPtr mInfo) mutable -> bool
                    {
                        assert(mInfo);
                        auto mDelay = mInfo->getDelay();
                        if (newDelay < mDelay) {
                            return true;
                        }
                        newDelay -= mDelay;
                        return false;
                    });

            if (reinsertIter != m_msgsToSend.end()) {
                auto& msgToUpdate = *reinsertIter;
                assert(msgToUpdate);
                auto mDelay = msgToUpdate->getDelay();
                msgToUpdate->setDelay(mDelay - newDelay);
            }

            auto clonedMsg = m_protocol->cloneMessage(*msgToSend);
            // TODO copy extra properties

            std::swap(clonedMsg, msgToSend);
            clonedMsg->setDelay(newDelay);

            if (repeatCount != 0) {
                clonedMsg->setRepeatCount(repeatCount - 1);
            }

            m_msgsToSend.insert(reinsertIter, std::move(clonedMsg));
        }
    }

    if (!m_msgsToSend.empty()) {
        auto& msgInfo = m_msgsToSend.front();
        assert(msgInfo);
        auto delay = msgInfo->getDelay();
        assert(0 < delay);
        msgInfo->setDelay(0);
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
