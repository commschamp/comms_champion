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
    typename THandler>
bool dispatchMsgTypePolymorphic(TId&& id, THandler& handler) 
{
    using HandlerType = typename std::decay<decltype(handler)>::type;
    return 
        details::DispatchMsgTypePolymorphicHelper<TAllMessages, HandlerType>::
            dispatch(std::forward<TId>(id), handler);
}

template <
    typename TAllMessages,
    typename TId,
    typename THandler>
bool dispatchMsgTypePolymorphic(TId&& id, std::size_t offset, THandler& handler) 
{
    using HandlerType = typename std::decay<decltype(handler)>::type;
    return 
        details::DispatchMsgTypePolymorphicHelper<TAllMessages, HandlerType>::
            dispatch(std::forward<TId>(id), offset, handler);
}

template <
    typename TAllMessages,
    typename TId,
    typename TMsg,
    typename THandler>
auto dispatchMsgStaticBinSearch(TId&& id, std::size_t offset, TMsg& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    static_assert(details::allMessagesHaveStaticNumId<TAllMessages>(), 
        "All messages in the provided tuple must statically define their numeric ID");


    return 
        details::DispatchMsgStaticBinSearchHelper<TAllMessages>::dispatch(
            std::forward<TId>(id),
            offset,
            msg,
            handler);
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

    return 
        details::DispatchMsgStaticBinSearchHelper<TAllMessages>::dispatch(
            std::forward<TId>(id),
            msg,
            handler);
}

template <
    typename TAllMessages,
    typename TMsg,
    typename THandler>
auto dispatchMsgStaticBinSearch(TMsg& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    static_assert(details::allMessagesHaveStaticNumId<TAllMessages>(), 
        "All messages in the provided tuple must statically define their numeric ID");
    using MsgType = typename std::decay<decltype(msg)>::type;
    static_assert(MsgType::hasGetId(), 
        "The used message object must provide polymorphic ID retrieval function");

    return 
        details::DispatchMsgStaticBinSearchHelper<TAllMessages>::dispatch(
            msg,
            handler);
}

template <
    typename TAllMessages,
    typename TId,
    typename THandler>
bool dispatchMsgTypeStaticBinSearch(TId&& id, std::size_t offset, THandler& handler) 
{
    static_assert(details::allMessagesHaveStaticNumId<TAllMessages>(), 
        "All messages in the provided tuple must statically define their numeric ID");

    return 
        details::DispatchMsgStaticBinSearchHelper<TAllMessages>::
            dispatchType(std::forward<TId>(id), offset, handler);
}

template <
    typename TAllMessages,
    typename TId,
    typename THandler>
bool dispatchMsgTypeStaticBinSearch(TId&& id, THandler& handler) 
{
    static_assert(details::allMessagesHaveStaticNumId<TAllMessages>(), 
        "All messages in the provided tuple must statically define their numeric ID");

    return 
        details::DispatchMsgStaticBinSearchHelper<TAllMessages>::
            dispatchType(std::forward<TId>(id), handler);
}

template <
    typename TAllMessages,
    typename TId,
    typename TMsg,
    typename THandler>
auto dispatchMsgLinearSwitch(TId&& id, std::size_t offset, TMsg& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    static_assert(details::allMessagesHaveStaticNumId<TAllMessages>(), 
        "All messages in the provided tuple must statically define their numeric ID");


    return 
        details::DispatchMsgLinearSwitchHelper<TAllMessages>::dispatch(
            std::forward<TId>(id),
            offset,
            msg,
            handler);
}

template <
    typename TAllMessages,
    typename TId,
    typename TMsg,
    typename THandler>
auto dispatchMsgLinearSwitch(TId&& id, TMsg& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    static_assert(details::allMessagesHaveStaticNumId<TAllMessages>(), 
        "All messages in the provided tuple must statically define their numeric ID");

    return 
        details::DispatchMsgLinearSwitchHelper<TAllMessages>::dispatch(
            std::forward<TId>(id),
            msg,
            handler);
}

template <
    typename TAllMessages,
    typename TMsg,
    typename THandler>
auto dispatchMsgLinearSwitch(TMsg& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    static_assert(details::allMessagesHaveStaticNumId<TAllMessages>(), 
        "All messages in the provided tuple must statically define their numeric ID");
    using MsgType = typename std::decay<decltype(msg)>::type;
    static_assert(MsgType::hasGetId(), 
        "The used message object must provide polymorphic ID retrieval function");

    return 
        details::DispatchMsgLinearSwitchHelper<TAllMessages>::dispatch(
            msg,
            handler);
}


} // namespace comms
