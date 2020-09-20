//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// Contains definition of MsgDispatcher class which facilitates
/// dispatching of message object to appropriate handler

#pragma once

#include <type_traits>

#include "comms/dispatch.h"
#include "comms/Message.h"
#include "comms/details/MsgDispatcherOptionsParser.h"
#include "comms/traits.h"
#include "comms/util/type_traits.h"

namespace comms
{

namespace details
{

template <class T, class R = void>
struct EnableIfHasMsgDispatcherTag { using Type = R; };

template <class T, class Enable = void>
struct HasMsgDispatcherTag
{
    static const bool Value = false;
};

template <class T>
struct HasMsgDispatcherTag<T, typename EnableIfHasMsgDispatcherTag<typename T::MsgDispatcherTag>::Type>
{
    static const bool Value = true;
};

template <class T>
constexpr bool hasMsgDispatcherTag()
{
    return HasMsgDispatcherTag<T>::Value;
}

} // namespace details

/// @brief An auxiliary class to force a particular way of dispatching message to its handler
/// @details If not options are provided, the dispatching is performed by invocation of
///     @ref comms::dispatchMsg() function.
/// @tparam TOptions Options to force a particular dispatch way. Supported ones are:
///     @li @ref comms::option::ForceDispatchPolymorphic - Force dispatch using
///         @ref comms::dispatchMsgPolymorphic()
///     @li @ref comms::option::ForceDispatchStaticBinSearch - Force dispatch using
///         @ref comms::dispatchMsgStaticBinSearch()
///     @li @ref comms::option::ForceDispatchLinearSwitch - Force dispatch using
///         @ref comms::dispatchMsgLinearSwitch()
template <typename... TOptions>
class MsgDispatcher
{
    using NoForcingTag = comms::details::tag::Tag1<>;

    using ParsedOptionsInternal = details::MsgDispatcherOptionsParser<TOptions...>;

    using Tag = 
        typename comms::util::Conditional<
            ParsedOptionsInternal::HasForcedDispatch
        >::template Type<
            typename ParsedOptionsInternal::ForcedDispatch,
            NoForcingTag
        >;

    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsgId&& id, std::size_t idx, TMsg& msg, THandler& handler, NoForcingTag) ->
        decltype(comms::dispatchMsg<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler))
    {
        return comms::dispatchMsg<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler);
    }

    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsgId&& id, TMsg& msg, THandler& handler, NoForcingTag) ->
        decltype(comms::dispatchMsg<TAllMessages>(std::forward<TMsgId>(id), msg, handler))
    {
        return comms::dispatchMsg<TAllMessages>(std::forward<TMsgId>(id), msg, handler);
    }

    template <typename TAllMessages, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsg& msg, THandler& handler, NoForcingTag) ->
        decltype(comms::dispatchMsg<TAllMessages>(msg, handler))
    {
        return comms::dispatchMsg<TAllMessages>(msg, handler);
    }

    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsgId&& id, std::size_t idx, TMsg& msg, THandler& handler, comms::traits::dispatch::Polymorphic) ->
        decltype(comms::dispatchMsgPolymorphic<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler))
    {
        return comms::dispatchMsgPolymorphic<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler);
    }

    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsgId&& id, TMsg& msg, THandler& handler, comms::traits::dispatch::Polymorphic) ->
        decltype(comms::dispatchMsgPolymorphic<TAllMessages>(std::forward<TMsgId>(id), msg, handler))
    {
        return comms::dispatchMsgPolymorphic<TAllMessages>(std::forward<TMsgId>(id), msg, handler);
    }

    template <typename TAllMessages, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsg& msg, THandler& handler, comms::traits::dispatch::Polymorphic) ->
        decltype(comms::dispatchMsgPolymorphic<TAllMessages>(msg, handler))
    {
        return comms::dispatchMsgPolymorphic<TAllMessages>(msg, handler);
    }

    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsgId&& id, std::size_t idx, TMsg& msg, THandler& handler, comms::traits::dispatch::StaticBinSearch) ->
        decltype(comms::dispatchMsgStaticBinSearch<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler))
    {
        return comms::dispatchMsgStaticBinSearch<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler);
    }

    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsgId&& id, TMsg& msg, THandler& handler, comms::traits::dispatch::StaticBinSearch) ->
        decltype(comms::dispatchMsgStaticBinSearch<TAllMessages>(std::forward<TMsgId>(id), msg, handler))
    {
        return comms::dispatchMsgStaticBinSearch<TAllMessages>(std::forward<TMsgId>(id), msg, handler);
    }

    template <typename TAllMessages, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsg& msg, THandler& handler, comms::traits::dispatch::StaticBinSearch) ->
        decltype(comms::dispatchMsgStaticBinSearch<TAllMessages>(msg, handler))
    {
        return comms::dispatchMsgStaticBinSearch<TAllMessages>(msg, handler);
    }

    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsgId&& id, std::size_t idx, TMsg& msg, THandler& handler, comms::traits::dispatch::LinearSwitch) ->
        decltype(comms::dispatchMsgLinearSwitch<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler))
    {
        return comms::dispatchMsgLinearSwitch<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler);
    }

    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsgId&& id, TMsg& msg, THandler& handler, comms::traits::dispatch::LinearSwitch) ->
        decltype(comms::dispatchMsgLinearSwitch<TAllMessages>(std::forward<TMsgId>(id), msg, handler))
    {
        return comms::dispatchMsgLinearSwitch<TAllMessages>(std::forward<TMsgId>(id), msg, handler);
    }

    template <typename TAllMessages, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsg& msg, THandler& handler, comms::traits::dispatch::LinearSwitch) ->
        decltype(comms::dispatchMsgLinearSwitch<TAllMessages>(msg, handler))
    {
        return comms::dispatchMsgLinearSwitch<TAllMessages>(msg, handler);
    }

    template <typename TAllMessages>
    static constexpr bool isDispatchPolymorphicInternal(NoForcingTag)
    {
        return comms::dispatchMsgTypeIsPolymorphic<TAllMessages>();
    }

    template <typename TAllMessages, typename TTag>
    static constexpr bool isDispatchPolymorphicInternal(TTag)
    {
        static_assert(!std::is_same<TTag, NoForcingTag>::value, "Invalid tag dispatch");
        return std::is_same<TTag, comms::traits::dispatch::Polymorphic>::value;
    }

    template <typename TAllMessages>
    static constexpr bool isDispatchStaticBinSearchInternal(NoForcingTag)
    {
        return comms::dispatchMsgTypeIsStaticBinSearch<TAllMessages>();
    }

    template <typename TAllMessages, typename TTag>
    static constexpr bool isDispatchStaticBinSearchInternal(TTag)
    {
        static_assert(!std::is_same<TTag, NoForcingTag>::value, "Invalid tag dispatch");
        return std::is_same<TTag, comms::traits::dispatch::StaticBinSearch>::value;
    }

    template <typename TAllMessages>
    static constexpr bool isDispatchLinearSwitchInternal(NoForcingTag)
    {
        return false;
    }

    template <typename TAllMessages, typename TTag>
    static constexpr bool isDispatchLinearSwitchInternal(TTag)
    {
        static_assert(!std::is_same<TTag, NoForcingTag>::value, "Invalid tag dispatch");
        return std::is_same<TTag, comms::traits::dispatch::LinearSwitch>::value;
    }

public:
    /// @brief Parsed Options
    using ParsedOptions = ParsedOptionsInternal;

    /// @brief Class detection tag
    using MsgDispatcherTag = typename ParsedOptions::ForcedDispatch;

    /// @brief Dispatch message to its handler.
    /// @details Uses @ref comms::dispatchMsg(), @ref comms::dispatchMsgPolymorphic(),
    ///     @ref comms::dispatchMsgStaticBinSearch(), or @ref comms::dispatchMsgLinearSwitch()
    ///     based on class definition option(s).
    /// @tparam TAllMessages Bundle (std::tuple) of all supported message classes
    /// @param[in] id ID of the message.
    /// @param[in] idx Index (or offset) of the message among those having the same numeric ID in the @b TAllMessages.
    /// @param[in] msg Reference to message object.
    /// @param[in] handler Reference to handler object
    /// @return What the @b handle() member function(s) of the @b hander return.
    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatch(TMsgId&& id, std::size_t idx, TMsg& msg, THandler& handler) ->
        decltype(dispatchInternal<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler, Tag()))
    {
        return dispatchInternal<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler, Tag());
    }

    /// @brief Dispatch message to its handler.
    /// @details Similar to other @ref dispatch(), but suitable for cases when messages tuple
    ///     does @b NOT contain message classes with the same ID value.
    /// @tparam TAllMessages Bundle (std::tuple) of all supported message classes
    /// @param[in] id ID of the message.
    /// @param[in] msg Reference to message object.
    /// @param[in] handler Reference to handler object
    /// @return What the @b handle() member function(s) of the @b hander return.
    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatch(TMsgId&& id, TMsg&& msg, THandler&& handler) ->
        decltype(dispatchInternal<TAllMessages>(std::forward<TMsgId>(id), msg, handler, Tag()))
    {
        return dispatchInternal<TAllMessages>(std::forward<TMsgId>(id), msg, handler, Tag());
    }

    /// @brief Dispatch message to its handler.
    /// @details Similar to other @ref dispatch(), but suitable for cases when message
    ///     interface class provides a way to polymorphically dispatch message object
    ///     to its handler (see @ref page_use_prot_interface_handle) and/or ability
    ///     to polymorphically retrieve message ID information (see
    ///     @ref page_use_prot_interface_id_retrieve) as well as requiring messages tuple
    ///     does @b NOT contain message classes with the same ID value.
    /// @tparam TAllMessages Bundle (std::tuple) of all supported message classes
    /// @param[in] msg Reference to message object.
    /// @param[in] handler Reference to handler objectz
    /// @return What the @b handle() member function(s) of the @b hander return.
    template <typename TAllMessages, typename TMsg, typename THandler>
    static auto dispatch(TMsg&& msg, THandler&& handler) ->
        decltype(dispatchInternal<TAllMessages>(msg, handler, Tag()))
    {
        return dispatchInternal<TAllMessages>(msg, handler, Tag());
    }

    /// @brief Compile time inquiry whether polymorphic dispatch tables are
    ///     generated internally to map message ID to actual type.
    /// @see @ref page_dispatch
    /// @see @ref isDispatchStaticBinSearch()
    /// @see @ref isDispatchLinearSwitch()
    template <typename TAllMessages>
    static constexpr bool isDispatchPolymorphic()
    {
        return isDispatchPolymorphicInternal<TAllMessages>(Tag());
    }

    /// @brief Compile time inquiry whether static binary search dispatch is
    ///     generated internally to map message ID to actual type.
    /// @see @ref page_dispatch
    /// @see @ref isDispatchPolymorphic()
    /// @see @ref isDispatchLinearSwitch()
    template <typename TAllMessages>
    static constexpr bool isDispatchStaticBinSearch()
    {
        return isDispatchStaticBinSearchInternal<TAllMessages>(Tag());
    }

    /// @brief Compile time inquiry whether linear switch dispatch is
    ///     generated internally to map message ID to actual type.
    /// @see @ref page_dispatch
    /// @see @ref isDispatchStaticBinSearch()
    /// @see @ref isDispatchLinearSwitch()
    template <typename TAllMessages>
    static constexpr bool isDispatchLinearSwitch()
    {
        return isDispatchLinearSwitchInternal<TAllMessages>(Tag());
    }
};

namespace details
{

template <typename T>
struct IsMsgDispatcher
{
    static constexpr bool Value = false;
};

template <typename... TOptions>
struct IsMsgDispatcher<MsgDispatcher<TOptions...> >
{
    static constexpr bool Value = true;
};


} // namespace details

/// @brief Compile time check whether the provided class is a variant of @ref comms::MsgDispatcher.
/// @related MsgDispatcher
template <typename T>
constexpr bool isMsgDispatcher()
{
    return 
        details::IsMsgDispatcher<T>::Value || 
        details::hasMsgDispatcherTag<T>();
}

} // namespace comms
