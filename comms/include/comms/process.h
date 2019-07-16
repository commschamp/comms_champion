//
// Copyright 2019 (C). Alex Robenko. All rights reserved.
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

/// @file
/// Provides auxiliary functions for processing input and dispatching messages

#pragma once

#include <type_traits>
#include <iterator>

#include "comms/ErrorStatus.h"
#include "comms/iterator.h"
#include "comms/dispatch.h"
#include "comms/Message.h"
#include "comms/Assert.h"
#include "comms/MsgDispatcher.h"
#include "comms/details/detect.h"
#include "comms/details/process.h"
#include "comms/util/ScopeGuard.h"
#include "comms/protocol/ProtocolLayerBase.h"

namespace  comms
{

/// @brief Process input until first message is recognized and its object is created
///     or missing data is reported.
/// @details Can be used to implement @ref page_use_prot_transport_read.
/// @param[in, out] bufIter Iterator to input buffer. Passed by reference and is updated
///     when buffer is iterated over. Number of consumed bytes cat be determined by
///     calculating the distance between originally passed value and the one after
///     function returns.
/// @param[in] len Number of remaining bytes in input buffer.
/// @param[in] frame Protocol frame / stack (see @ref page_use_prot_transport) that
///     is used to process the raw input.
/// @param[in, out] msg Smart pointer (see @ref comms::protocol::ProtocolLayerBase::MsgPtr "MsgPtr"
///     defintion of the @ref page_use_prot_transport) to message object to be allocated,
///     or reference to actual message object (extending @ref comms::MessageBase) when
///     such is known.
/// @param[in, out] extraValues Extra values that are passed as variadic parameters to
///     @ref comms::protocol::ProtocolLayerBase::read() "read()" member function
///     of the protocol frame / stack.
/// @return ErrorStatus of the protocol frame / stack @ref comms::protocol::ProtocolLayerBase::read() "read()"
///     operation.
/// @note Defined in comms/process.h
/// @see @ref page_dispatch
template <typename TBufIter, typename TFrame, typename TMsg, typename... TExtraValues>
comms::ErrorStatus processSingle(
    TBufIter& bufIter,
    std::size_t len,
    TFrame&& frame,
    TMsg& msg,
    TExtraValues... extraValues)
{
    std::size_t consumed = 0U;
    auto onExit =
        comms::util::makeScopeGuard(
            [&bufIter, &consumed]()
            {
                std::advance(bufIter, consumed);
            });
    static_cast<void>(onExit);

    while (consumed < len) {
        auto begIter = comms::readIteratorFor(msg, bufIter + consumed);
        auto iter = begIter;

        // Do the read
        auto es = frame.read(msg, iter, len - consumed, extraValues...);
        if (es == comms::ErrorStatus::NotEnoughData) {
            return es;
        }

        if (es == comms::ErrorStatus::ProtocolError) {
            // Something is not right with the data, remove one character and try again
           ++consumed;
            continue;
        }

        consumed += std::distance(begIter, iter);
        return es;
    }

    return comms::ErrorStatus::NotEnoughData;
}

/// @brief Process input until first message is recognized, its object is created
///     and dispatched to appropriate handling function, or missing data is reported.
/// @details Similar to @ref comms::processSingle(), but adds dispatch stage.
///     Can be used to implement @ref page_use_prot_transport_read.
/// @param[in, out] bufIter Iterator to input buffer. Passed by reference and is updated
///     when buffer is iterated over. Number of consumed bytes cat be determined by
///     calculating the distance between originally passed value and the one after
///     function returns.
/// @param[in] len Number of remaining bytes in input buffer.
/// @param[in] frame Protocol frame / stack (see @ref page_use_prot_transport) that
///     is used to process the raw input.
/// @param[in, out] msg Smart pointer (see @ref comms::protocol::ProtocolLayerBase::MsgPtr "MsgPtr"
///     defintion of the @ref page_use_prot_transport) to message object to be allocated,
///     or reference to actual message object (extending @ref comms::MessageBase) when
///     such is known.
/// @param[in] handler Handler to handle message object when dispatched. The dispatch
///     is performed using @ref comms::dispatchMsg() function.
/// @param[in, out] extraValues Extra values that are passed as variadic parameters to
///     @ref comms::protocol::ProtocolLayerBase::read() "read()" member function
///     of the protocol frame / stack.
/// @return ErrorStatus of the protocol frame / stack @ref comms::protocol::ProtocolLayerBase::read() "read()"
///     operation.
/// @note Defined in comms/process.h
/// @note If default dispatch behaviour of the @ref comms::dispatchMsg()
///     function doesn't suit the application needs, consider using
///     @ref comms::processSingleWithDispatchViaDispatcher() instead.
/// @see @ref page_dispatch
template <typename TBufIter, typename TFrame, typename TMsg, typename THandler, typename... TExtraValues>
comms::ErrorStatus processSingleWithDispatch(
    TBufIter& bufIter,
    std::size_t len,
    TFrame&& frame,
    TMsg& msg,
    THandler& handler,
    TExtraValues... extraValues)
{
    using LocalMsgIdType = details::ProcessMsgIdType<typename std::decay<decltype(msg)>::type>;
    LocalMsgIdType id = LocalMsgIdType();
    std::size_t idx = 0U;

    auto es =
        processSingle(
            bufIter,
            len,
            std::forward<TFrame>(frame),
            msg,
            comms::protocol::msgId(id),
            comms::protocol::msgIndex(idx),
            extraValues...);

    if (es != comms::ErrorStatus::Success) {
        return es;
    }

    static_cast<void>(handler);
    using FrameType = typename std::decay<decltype(frame)>::type;
    using AllMessagesType = typename FrameType::AllMessages;
    auto& msgObj = details::processMsgCastToMsgObj(msg);
    comms::dispatchMsg<AllMessagesType>(id, idx, msgObj, handler);
    return es;
}

/// @brief Process input until first message is recognized, its object is created
///     and dispatched to appropriate handling function, or missing data is reported.
/// @details Similar to @ref comms::processSingleWithDispatch(), but allows forcing
///     a particular dispatch policy.
/// @tparam TDispatcher A variant of @ref comms::MsgDispatcher class. It's going
///     to be used to dispatch message object into appropriate handling function
///     instead of using @ref comms::dispatchMsg() like @ref comms::processSingleWithDispatch()
///     does.
/// @param[in, out] bufIter Iterator to input buffer. Passed by reference and is updated
///     when buffer is iterated over. Number of consumed bytes cat be determined by
///     calculating the distance between originally passed value and the one after
///     function returns.
/// @param[in] len Number of remaining bytes in input buffer.
/// @param[in] frame Protocol frame / stack (see @ref page_use_prot_transport) that
///     is used to process the raw input.
/// @param[in, out] msg Smart pointer (see @ref comms::protocol::ProtocolLayerBase::MsgPtr "MsgPtr"
///     defintion of the @ref page_use_prot_transport) to message object to be allocated,
///     or reference to actual message object (extending @ref comms::MessageBase) when
///     such is known.
/// @param[in] handler Handler to handle message object when dispatched. The dispatch
///     is performed via provded @b TDispatcher class (see @ref comms::MsgDispatcher).
/// @param[in, out] extraValues Extra values that are passed as variadic parameters to
///     @ref comms::protocol::ProtocolLayerBase::read() "read()" member function
///     of the protocol frame / stack.
/// @return ErrorStatus of the protocol frame / stack @ref comms::protocol::ProtocolLayerBase::read() "read()"
///     operation.
/// @note Defined in comms/process.h
/// @see @ref comms::processSingleWithDispatch().
/// @see @ref page_dispatch
/// @see @ref page_use_prot_transport_read
template <typename TDispatcher, typename TBufIter, typename TFrame, typename TMsg, typename THandler, typename... TExtraValues>
comms::ErrorStatus processSingleWithDispatchViaDispatcher(
    TBufIter& bufIter,
    std::size_t len,
    TFrame&& frame,
    TMsg& msg,
    THandler& handler,
    TExtraValues... extraValues)
{
    using LocalMsgIdType = details::ProcessMsgIdType<typename std::decay<decltype(msg)>::type>;
    static_assert(!std::is_void<LocalMsgIdType>(), "Invalid type of msg param");

    LocalMsgIdType id = LocalMsgIdType();
    std::size_t idx = 0U;

    auto es =
        processSingle(
            bufIter,
            len,
            std::forward<TFrame>(frame),
            msg,
            comms::protocol::msgId(id),
            comms::protocol::msgIndex(idx),
            extraValues...);

    if (es != comms::ErrorStatus::Success) {
        return es;
    }

    using FrameType = typename std::decay<decltype(frame)>::type;
    using AllMessagesType = typename FrameType::AllMessages;
    static_assert(
        comms::isMsgDispatcher<TDispatcher>(),
        "TDispatcher is expected to be a variant of comms::MsgDispatcher");

    auto& msgObj = details::processMsgCastToMsgObj(msg);
    TDispatcher::template dispatch<AllMessagesType>(id, idx, msgObj, handler);
    return es;
}

/// @brief Process all available input and dispatch all created message objects
///     to appropriate handling function.
/// @details All the created message objects are immediatelly destructed after
///     dispatching.
/// @param[in, out] bufIter Iterator to input buffer. Passed by value and is @b NOT updated
///     when buffer is iterated over (unlike @ref comms::processSingle(),
///     @ref comms::processSingleWithDispatch(), @ref comms::processSingleWithDispatchViaDispatcher()).
/// @param[in] len Number of remaining bytes in input buffer.
/// @param[in] frame Protocol frame / stack (see @ref page_use_prot_transport) that
///     is used to process the raw input.
/// @param[in] handler Handler to handle message object when dispatched. The dispatch
///     is performed using @ref comms::dispatchMsg() function.
/// @return Number of consumed bytes from the buffer. The caller is responsible to
///     remove them from the buffer.
/// @note Defined in comms/process.h
/// @note If default dispatch behaviour of the @ref comms::dispatchMsg()
///     function doesn't suit the application needs, consider using
///     @ref comms::processAllWithDispatchViaDispatcher() instead.
/// @see @ref page_dispatch
/// @see @ref comms::processAllWithDispatchViaDispatcher().
/// @see @ref page_dispatch
/// @see @ref page_use_prot_transport_read
template <typename TBufIter, typename TFrame, typename THandler, typename... TExtraValues>
std::size_t processAllWithDispatch(
    TBufIter bufIter,
    std::size_t len,
    TFrame&& frame,
    THandler& handler)
{
    std::size_t consumed = 0U;
    using FrameType = typename std::decay<decltype(frame)>::type;
    using MsgPtr = typename FrameType::MsgPtr;
    while (consumed < len) {
        auto begIter = bufIter + consumed;
        auto iter = begIter;

        MsgPtr msg;
        auto es = processSingleWithDispatch(iter, len - consumed, std::forward<TFrame>(frame), msg, handler);
        consumed += std::distance(begIter, iter);
        if (es == comms::ErrorStatus::NotEnoughData) {
            break;
        }
        COMMS_ASSERT(consumed <= len);
    }

    return consumed;
}

/// @brief Process all available input and dispatch all created message objects
///     to appropriate handling function.
/// @details Similar to @ref comms::processAllWithDispatch(), but allows forcing
///     a particular dispatch policy. All the created message objects are
///     immediatelly destructed after dispatching.
/// @tparam TDispatcher A variant of @ref comms::MsgDispatcher class. It's going
///     to be used to dispatch message object into appropriate handling function
///     instead of using @ref comms::dispatchMsg() like @ref comms::processSingleWithDispatch()
///     does.
/// @param[in, out] bufIter Iterator to input buffer. Passed by value and is @b NOT updated
///     when buffer is iterated over (unlike @ref comms::processSingle(),
///     @ref comms::processSingleWithDispatch(), @ref comms::processSingleWithDispatchViaDispatcher()).
/// @param[in] len Number of remaining bytes in input buffer.
/// @param[in] frame Protocol frame / stack (see @ref page_use_prot_transport) that
///     is used to process the raw input.
/// @param[in] handler Handler to handle message object when dispatched. The dispatch
///     is performed via provded @b TDispatcher class (see @ref comms::MsgDispatcher).
/// @return Number of consumed bytes from the buffer. The caller is responsible to
///     remove them from the buffer.
/// @note Defined in comms/process.h
/// @see @ref comms::processAllWithDispatch().
/// @see @ref page_dispatch
/// @see @ref page_use_prot_transport_read
template <typename TDispatcher, typename TBufIter, typename TFrame, typename THandler, typename... TExtraValues>
std::size_t processAllWithDispatchViaDispatcher(
    TBufIter bufIter,
    std::size_t len,
    TFrame&& frame,
    THandler& handler)
{
    std::size_t consumed = 0U;
    using FrameType = typename std::decay<decltype(frame)>::type;
    using MsgPtr = typename FrameType::MsgPtr;
    while (consumed < len) {
        auto begIter = bufIter + consumed;
        auto iter = begIter;

        MsgPtr msg;
        auto es = processSingleWithDispatchViaDispatcher<TDispatcher>(iter, len - consumed, std::forward<TFrame>(frame), msg, handler);
        consumed += std::distance(begIter, iter);
        if (es == comms::ErrorStatus::NotEnoughData) {
            break;
        }
        COMMS_ASSERT(consumed <= len);
    }

    return consumed;
}

} // namespace  comms
