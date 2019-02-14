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

/// @brief Dispatch message object into appropriate @b handle() function in the
///     provided handler using polymorphic behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] msg Message object held by reference to its interface class.
/// @param[in] handler Handler object
template <
    typename TAllMessages,
    typename TMsg,
    typename THandler>
auto dispatchMsgPolymorphic(TMsg& msg, THandler& handler) ->
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

/// @brief Dispatch message object into appropriate @b handle() function in the
///     provided handler using polymorphic behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] msg Message object held by reference to its interface class.
/// @param[in] handler Handler object
template <
    typename TAllMessages,
    typename TId,
    typename TMsg,
    typename THandler>
auto dispatchMsgPolymorphic(TId&& id, TMsg& msg, THandler& handler) ->
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

/// @brief Dispatch message object into appropriate @b handle() function in the
///     provided handler using polymorphic behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] offset Offset (or index) of the message type among those having the same ID.
/// @param[in] msg Message object held by reference to its interface class.
/// @param[in] handler Handler object
template <
    typename TAllMessages,
    typename TId,
    typename TMsg,
    typename THandler>
auto dispatchMsgPolymorphic(TId&& id, std::size_t offset, TMsg& msg, THandler& handler) ->
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

/// @brief Dispatch message id into appropriate @b handle() function in the
///     provided handler using polymorphic behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] handler Handler object
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

/// @brief Dispatch message id into appropriate @b handle() function in the
///     provided handler using polymorphic behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] offset Offset (or index) of the message type among those having the same ID.
/// @param[in] handler Handler object
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

/// @brief Dispatch message object into appropriate @b handle() function in the
///     provided handler using static binary search behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] offset Offset (or index) of the message type among those having the same ID.
/// @param[in] msg Message object held by reference to its interface class.
/// @param[in] handler Handler object
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

/// @brief Dispatch message object into appropriate @b handle() function in the
///     provided handler using static binary search behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] msg Message object held by reference to its interface class.
/// @param[in] handler Handler object
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

/// @brief Dispatch message object into appropriate @b handle() function in the
///     provided handler using static binary search behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] msg Message object held by reference to its interface class.
/// @param[in] handler Handler object
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

/// @brief Dispatch message id into appropriate @b handle() function in the
///     provided handler using static binary search behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] offset Offset (or index) of the message type among those having the same ID.
/// @param[in] handler Handler object
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

/// @brief Dispatch message id into appropriate @b handle() function in the
///     provided handler using static binary search behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] handler Handler object
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

/// @brief Count number of message types in the provided tuple that
///     have the requested numeric ID.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
template <typename TAllMessages, typename TId>
std::size_t dispatchMsgTypeCountStaticBinSearch(TId&& id) 
{
    static_assert(details::allMessagesHaveStaticNumId<TAllMessages>(), 
        "All messages in the provided tuple must statically define their numeric ID");

    return 
        details::DispatchMsgStaticBinSearchHelper<TAllMessages>::
            dispatchTypeCount(std::forward<TId>(id));
}

/// @brief Dispatch message object into appropriate @b handle() function in the
///     provided handler using linear switch behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] offset Offset (or index) of the message type among those having the same ID.
/// @param[in] msg Message object held by reference to its interface class.
/// @param[in] handler Handler object
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

/// @brief Dispatch message object into appropriate @b handle() function in the
///     provided handler using linear switch behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] msg Message object held by reference to its interface class.
/// @param[in] handler Handler object
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

/// @brief Dispatch message object into appropriate @b handle() function in the
///     provided handler using linear switch behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] msg Message object held by reference to its interface class.
/// @param[in] handler Handler object
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

/// @brief Dispatch message id into appropriate @b handle() function in the
///     provided handler using linear switch behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] offset Offset (or index) of the message type among those having the same ID.
/// @param[in] handler Handler object
template <
    typename TAllMessages,
    typename TId,
    typename THandler>
bool dispatchMsgTypeLinearSwitch(TId&& id, std::size_t offset, THandler& handler) 
{
    static_assert(details::allMessagesHaveStaticNumId<TAllMessages>(), 
        "All messages in the provided tuple must statically define their numeric ID");

    return 
        details::DispatchMsgLinearSwitchHelper<TAllMessages>::
            dispatchType(std::forward<TId>(id), offset, handler);
}

/// @brief Dispatch message id into appropriate @b handle() function in the
///     provided handler using linear switch behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] handler Handler object
template <
    typename TAllMessages,
    typename TId,
    typename THandler>
bool dispatchMsgTypeLinearSwitch(TId&& id, THandler& handler) 
{
    static_assert(details::allMessagesHaveStaticNumId<TAllMessages>(), 
        "All messages in the provided tuple must statically define their numeric ID");

    return 
        details::DispatchMsgLinearSwitchHelper<TAllMessages>::
            dispatchType(std::forward<TId>(id), handler);
}

namespace details
{
template <typename TAllMessages>
class DispatchMsgHelper
{
public:
    template <typename TMsg, typename THandler>
    static auto dispatchMsg(TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return dispatchMsgInternal(msg, handler, Tag());
    }

    template <typename TId, typename TMsg, typename THandler>
    static auto dispatchMsg(TId&& id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return dispatchMsgInternal(std::forward<TId>(id), msg, handler, Tag());
    }

    template <typename TId, typename TMsg, typename THandler>
    static auto dispatchMsg(TId&& id, std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return dispatchMsgInternal(std::forward<TId>(id), offset, msg, handler, Tag());
    }

    template <typename TId, typename THandler>
    static bool dispatchMsgType(TId&& id, THandler& handler) 
    {
        return dispatchMsgTypeInternal(std::forward<TId>(id), handler, Tag());
    }

    template <typename TId, typename THandler>
    static bool dispatchMsgType(TId&& id, std::size_t offset, THandler& handler) 
    {
        return dispatchMsgTypeInternal(std::forward<TId>(id), offset, handler, Tag());
    }

    static constexpr bool isPolymorphic()
    {
        return std::is_same<Tag, DirectPolymorphicTag>::value;
    }

    static constexpr bool isStaticBinSearch()
    {
        return std::is_same<Tag, StaticBinSearchTag>::value;
    }

private:
    struct DirectPolymorphicTag {};
    struct StaticBinSearchTag {};

    using Tag = 
        typename std::conditional<
            dispatchMsgPolymorphicIsDirectSuitable<TAllMessages>(),
            DirectPolymorphicTag,
            StaticBinSearchTag
        >::type;

    template <typename TMsg, typename THandler>
    static auto dispatchMsgInternal(TMsg& msg, THandler& handler, DirectPolymorphicTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return comms::dispatchMsgPolymorphic<TAllMessages>(msg, handler);
    }

    template <typename TMsg, typename THandler>
    static auto dispatchMsgInternal(TMsg& msg, THandler& handler, StaticBinSearchTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return comms::dispatchMsgStaticBinSearch<TAllMessages>(msg, handler);
    }

    template <typename TId, typename TMsg, typename THandler>
    static auto dispatchMsgInternal(TId&& id, TMsg& msg, THandler& handler, DirectPolymorphicTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return comms::dispatchMsgPolymorphic<TAllMessages>(std::forward<TId>(id), msg, handler);
    }

    template <typename TId, typename TMsg, typename THandler>
    static auto dispatchMsgInternal(TId&& id, TMsg& msg, THandler& handler, StaticBinSearchTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return comms::dispatchMsgStaticBinSearch<TAllMessages>(std::forward<TId>(id), msg, handler);
    }

    template <typename TId, typename TMsg, typename THandler>
    static auto dispatchMsgInternal(TId&& id, std::size_t offset, TMsg& msg, THandler& handler, DirectPolymorphicTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return comms::dispatchMsgPolymorphic<TAllMessages>(std::forward<TId>(id), offset, msg, handler);
    }

    template <typename TId, typename TMsg, typename THandler>
    static auto dispatchMsgInternal(TId&& id, std::size_t offset, TMsg& msg, THandler& handler, StaticBinSearchTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return comms::dispatchMsgStaticBinSearch<TAllMessages>(std::forward<TId>(id), offset, msg, handler);
    }

    template <typename TId, typename THandler>
    static bool dispatchMsgTypeInternal(TId&& id, THandler& handler, DirectPolymorphicTag) 
    {
        return comms::dispatchMsgTypePolymorphic<TAllMessages>(std::forward<TId>(id), handler);
    }

    template <typename TId, typename THandler>
    static bool dispatchMsgTypeInternal(TId&& id, THandler& handler, StaticBinSearchTag) 
    {
        return comms::dispatchMsgTypeStaticBinSearch<TAllMessages>(std::forward<TId>(id), handler);
    }

    template <typename TId, typename THandler>
    static bool dispatchMsgTypeInternal(TId&& id, std::size_t offset, THandler& handler, DirectPolymorphicTag) 
    {
        return comms::dispatchMsgTypePolymorphic<TAllMessages>(std::forward<TId>(id), offset, handler);
    }

    template <typename TId, typename THandler>
    static bool dispatchMsgTypeInternal(TId&& id, std::size_t offset, THandler& handler, StaticBinSearchTag) 
    {
        return comms::dispatchMsgTypeStaticBinSearch<TAllMessages>(std::forward<TId>(id), offset, handler);
    }
};

} // namespace details

/// @brief Dispatch message object into appropriate @b handle() function in the
///     provided handler using either "polymorphic" or "static binary search" behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] msg Message object held by reference to its interface class.
/// @param[in] handler Handler object
template <
    typename TAllMessages,
    typename TMsg,
    typename THandler>
auto dispatchMsg(TMsg& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    return details::DispatchMsgHelper<TAllMessages>::dispatchMsg(msg, handler); 
}

/// @brief Dispatch message object into appropriate @b handle() function in the
///     provided handler using either "polymorphic" or "static binary search" behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] msg Message object held by reference to its interface class.
/// @param[in] handler Handler object
template <
    typename TAllMessages,
    typename TId,
    typename TMsg,
    typename THandler>
auto dispatchMsg(TId&& id, TMsg& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    return details::DispatchMsgHelper<TAllMessages>::dispatchMsg(std::forward<TId>(id), msg, handler); 
}

/// @brief Dispatch message object into appropriate @b handle() function in the
///     provided handler using either "polymorphic" or "static binary search" behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] offset Offset (or index) of the message type among those having the same ID.
/// @param[in] msg Message object held by reference to its interface class.
/// @param[in] handler Handler object
template <
    typename TAllMessages,
    typename TId,
    typename TMsg,
    typename THandler>
auto dispatchMsg(TId&& id, std::size_t offset, TMsg& msg, THandler& handler) ->
    details::MessageInterfaceDispatchRetType<
        typename std::decay<decltype(handler)>::type>
{
    return details::DispatchMsgHelper<TAllMessages>::dispatchMsg(std::forward<TId>(id), offset, msg, handler); 
}

/// @brief Dispatch message id into appropriate @b handle() function in the
///     provided handler using either "polymorphic" or "static binary search" behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] handler Handler object
template <typename TAllMessages, typename TId, typename THandler>
bool dispatchMsgType(TId&& id, THandler& handler)
{
    return details::DispatchMsgHelper<TAllMessages>::dispatchMsgType(std::forward<TId>(id), handler); 
}

/// @brief Dispatch message id into appropriate @b handle() function in the
///     provided handler using either "polymorphic" or "static binary search" behavior.
/// @tparam TAllMessages @b std::tuple of supported message classes
/// @param[in] id ID of the message known at runtime.
/// @param[in] offset Offset (or index) of the message type among those having the same ID.
/// @param[in] handler Handler object
template <typename TAllMessages, typename TId, typename THandler>
bool dispatchMsgType(TId&& id, std::size_t offset, THandler& handler)
{
    return details::DispatchMsgHelper<TAllMessages>::dispatchMsgType(std::forward<TId>(id), offset, handler); 
}

/// @brief Compile time check whether the @ref dispatchMsgType() or 
///     @ref dispatchMsgType() will use "polymorphic" dispatch for provided
///     tuple of messages.
template <typename TAllMessages>
constexpr bool dispatchMsgTypeIsPolymorphic()
{
    return details::DispatchMsgHelper<TAllMessages>::isPolymorphic(); 
}

/// @brief Compile time check whether the @ref dispatchMsgType() or 
///     @ref dispatchMsgType() will use "static binary search" dispatch for provided
///     tuple of messages.
template <typename TAllMessages>
constexpr bool dispatchMsgTypeIsStaticBinSearch()
{
    return details::DispatchMsgHelper<TAllMessages>::isStaticBinSearch(); 
}

} // namespace comms
