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
#include "comms/util/ScopeGuard.h"

namespace  comms
{

namespace details
{

template <typename T>
using ProcessMsgDecayType = typename std::decay<T>::type;

template <bool TIsMessage, bool TIsMsgPtr>
struct ProcessMsgIdRetrieveHelper
{
    template <typename T>
    using Type = void;
};

template <>
struct ProcessMsgIdRetrieveHelper<true, false>
{
    template <typename T>
    using Type = comms::MessageIdType<T>;
};

template <>
struct ProcessMsgIdRetrieveHelper<false, true>
{
private:
    template <typename T>
    using ElementType = typename T::element_type;
public:
    template <typename T>
    using Type =
        typename ProcessMsgIdRetrieveHelper<comms::isMessage<ElementType<T> >(),  hasElementType<ElementType<T> >()>::
            template Type<ElementType<T> >;
};

template <typename T>
using ProcessMsgIdType =
    typename ProcessMsgIdRetrieveHelper<comms::isMessage<ProcessMsgDecayType<T> >(), hasElementType<ProcessMsgDecayType<T> >()>::
        template Type<ProcessMsgDecayType<T> >;

\
template <bool TIsMessage, bool TIsMsgPtr>
struct ProcessMsgCastToMsgObjHelper;

template <>
struct ProcessMsgCastToMsgObjHelper<true, false>
{
    template <typename T>
    static auto cast(T& msg) -> decltype(msg)
    {
        return msg;
    }
};

template <>
struct ProcessMsgCastToMsgObjHelper<false, true>
{
    template <typename T>
    static auto cast(T& msg) -> decltype(*msg)
    {
        return *msg;
    }
};


template <typename T>
auto processMsgCastToMsgObj(T& msg) ->
    decltype(ProcessMsgCastToMsgObjHelper<comms::isMessage<ProcessMsgDecayType<decltype(msg)> >(), hasElementType<ProcessMsgDecayType<decltype(msg)> >()>::cast(msg))
{
    return ProcessMsgCastToMsgObjHelper<comms::isMessage<ProcessMsgDecayType<decltype(msg)> >(), hasElementType<ProcessMsgDecayType<decltype(msg)> >()>::cast(msg);
}

} // namespace details

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
