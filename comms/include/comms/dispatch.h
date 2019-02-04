//
// Copyright 2019 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
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
/// Contains extra logic to help with dispatching message types and objects

#pragma once

#include <type_traits>

#include "comms/Message.h"
#include "comms/details/dispatch_impl.h"

namespace comms
{

template <
    typename TAllMessages,
    typename TMsg,
    typename THandler>
auto dispatchMsgPolymorphic(TMsg&& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    using MsgType = typename std::decay<decltype(msg)>::type;
    static_assert(comms::isMessage<MsgType>(), "msg param must be a valid message");
    using HandlerType = typename std::decay<decltype(handler)>::type;
    return 
        details::DispatchMsgPolymorphicHelper<TAllMessages, MsgType, HandlerType>::
            dispatch(msg, handler);
}

template <
    typename TAllMessages,
    typename TId,
    typename TMsg,
    typename THandler>
auto dispatchMsgPolymorphic(TId&& id, TMsg&& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    using MsgType = typename std::decay<decltype(msg)>::type;
    static_assert(comms::isMessage<MsgType>(), "msg param must be a valid message");
    using HandlerType = typename std::decay<decltype(handler)>::type;
    return 
        details::DispatchMsgPolymorphicHelper<TAllMessages, MsgType, HandlerType>::
            dispatch(std::forward<TId>(id), msg, handler);
}

template <
    typename TAllMessages,
    typename TId,
    typename TMsg,
    typename THandler>
auto dispatchMsgPolymorphic(TId&& id, std::size_t offset, TMsg&& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    using MsgType = typename std::decay<decltype(msg)>::type;
    static_assert(comms::isMessage<MsgType>(), "msg param must be a valid message");
    using HandlerType = typename std::decay<decltype(handler)>::type;
    return 
        details::DispatchMsgPolymorphicHelper<TAllMessages, MsgType, HandlerType>::
            dispatch(std::forward<TId>(id), offset, msg, handler);
}

template <
    typename TAllMessages,
    typename TId,
    typename TMsg,
    typename THandler>
auto dispatchMsgStaticBinSearch(TId&& id, std::size_t idx, TMsg& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    static_assert(details::allMessagesHaveStaticNumId<TAllMessages>(), 
        "All messages in the provided tuple must statically define their numeric ID");

    using HandlerType = typename std::decay<decltype(handler)>::type;
    using RetType = details::MessageInterfaceDispatchRetType<HandlerType>;

    return static_cast<RetType>(
        details::DispatchMsgStaticBinSearchHelper<TAllMessages, 0U, std::tuple_size<TAllMessages>::value>::dispatch(
            std::forward<TId>(id),
            idx,
            msg,
            handler));
}

template <
    typename TAllMessages,
    typename TId,
    typename TMsg,
    typename THandler>
auto dispatchMsgStaticBinSearch(TId&& id, TMsg& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    static_assert(details::allMessagesHaveStaticNumId<TAllMessages>(), 
        "All messages in the provided tuple must statically define their numeric ID");
    static_assert(details::allMessagesAreStrongSorted<TAllMessages>(),
        "All messages in the provided tuple must be sorted with unique IDs");
    return dispatchMsgStaticBinSearch<TAllMessages>(std::forward<TId>(id), 0U, msg, handler);
}

template <
    typename TAllMessages,
    typename TId,
    typename TMsg,
    typename THandler>
auto dispatchMsgStaticBinSearch(TMsg& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    using MsgType = typename std::decay<decltype(msg)>::type;
    static_assert(comms::isMessage<MsgType>(), "msg param must be a valid message");

    static_assert(MsgType::hasGetId(), 
        "To be able to use this function, message object must have polymorphic ID retrieval member function.");

    return dispatchMsgStaticBinSearch<TAllMessages>(msg.getId(), msg, handler);
}

} // namespace comms
