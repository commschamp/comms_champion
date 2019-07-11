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
/// Contains definition of MsgDispatcher class which facilitates
/// dispatching of message object to appropriate handler

#pragma once

#include <type_traits>

#include "comms/dispatch.h"
#include "comms/Message.h"
#include "comms/details/MsgDispatcherOptionsParser.h"
#include "comms/traits.h"

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

struct MsgDispatcherNoForcingTag {};
struct MsgDispatcherHasForcingTag {};


template <bool THasForcedDispatch>
struct MsgDispatcherTagHelper;

template <>
struct MsgDispatcherTagHelper<false>
{
    template <typename TOpt>
    using Primary = MsgDispatcherNoForcingTag;

    template <typename TOpt>
    using Secondary = void;
};

template <>
struct MsgDispatcherTagHelper<true>
{
    template <typename TOpt>
    using Primary = MsgDispatcherHasForcingTag;

    template <typename TOpt>
    using Secondary = typename TOpt::ForcedDispatch;
};

} // namespace details

template <typename... TOptions>
class MsgDispatcher
{
    using NoForcingTag = details::MsgDispatcherNoForcingTag;
    using HasForcingTag = details::MsgDispatcherHasForcingTag;

    using ParsedOptionsInternal = details::MsgDispatcherOptionsParser<TOptions...>;
    using PrimaryDispatchTag =
        typename details::MsgDispatcherTagHelper<ParsedOptionsInternal::HasForcedDispatch>::
            template Primary<ParsedOptionsInternal>;

    using SecondaryDispatchTag =
        typename details::MsgDispatcherTagHelper<ParsedOptionsInternal::HasForcedDispatch>::
            template Secondary<ParsedOptionsInternal>;


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


    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsgId&& id, std::size_t idx, TMsg& msg, THandler& handler, HasForcingTag) ->
        decltype(dispatchInternal<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler, SecondaryDispatchTag()))
    {
        return dispatchInternal<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler, SecondaryDispatchTag());
    }

    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsgId&& id, TMsg& msg, THandler& handler, HasForcingTag) ->
        decltype(dispatchInternal<TAllMessages>(std::forward<TMsgId>(id), msg, handler, SecondaryDispatchTag()))
    {
        return dispatchInternal<TAllMessages>(std::forward<TMsgId>(id), msg, handler, SecondaryDispatchTag());
    }

    template <typename TAllMessages, typename TMsg, typename THandler>
    static auto dispatchInternal(TMsg& msg, THandler& handler, HasForcingTag) ->
        decltype(dispatchInternal<TAllMessages>(msg, handler, SecondaryDispatchTag()))
    {
        return dispatchInternal<TAllMessages>(msg, handler, SecondaryDispatchTag());
    }

    template <typename TAllMessages>
    static constexpr bool isDispatchPolymorphicInternal(NoForcingTag)
    {
        return comms::dispatchMsgTypeIsPolymorphic<TAllMessages>();
    }

    template <typename TAllMessages>
    static constexpr bool isDispatchPolymorphicInternal(HasForcingTag)
    {
        return std::is_same<SecondaryDispatchTag, comms::traits::dispatch::Polymorphic>::value;
    }

    template <typename TAllMessages>
    static constexpr bool isDispatchStaticBinSearchInternal(NoForcingTag)
    {
        return comms::dispatchMsgTypeIsStaticBinSearch<TAllMessages>();
    }

    template <typename TAllMessages>
    static constexpr bool isDispatchStaticBinSearchInternal(HasForcingTag)
    {
        return std::is_same<SecondaryDispatchTag, comms::traits::dispatch::StaticBinSearch>::value;
    }

    template <typename TAllMessages>
    static constexpr bool isDispatchLinearSwitchInternal(NoForcingTag)
    {
        return false;
    }

    template <typename TAllMessages>
    static constexpr bool isDispatchLinearSwitchInternal(HasForcingTag)
    {
        return std::is_same<SecondaryDispatchTag, comms::traits::dispatch::LinearSwitch>::value;
    }

public:
    using ParsedOptions = ParsedOptionsInternal;
    using MsgDispatcherTag = void;

    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatch(TMsgId&& id, std::size_t idx, TMsg&& msg, THandler&& handler) ->
        decltype(dispatchInternal<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler, PrimaryDispatchTag()))
    {

        return dispatchInternal<TAllMessages>(std::forward<TMsgId>(id), idx, msg, handler, PrimaryDispatchTag());
    }

    template <typename TAllMessages, typename TMsgId, typename TMsg, typename THandler>
    static auto dispatch(TMsgId&& id, TMsg&& msg, THandler&& handler) ->
        decltype(dispatchInternal<TAllMessages>(std::forward<TMsgId>(id), msg, handler, PrimaryDispatchTag()))
    {
        return dispatchInternal<TAllMessages>(std::forward<TMsgId>(id), msg, handler, PrimaryDispatchTag());
    }

    template <typename TAllMessages, typename TMsg, typename THandler>
    static auto dispatch(TMsg&& msg, THandler&& handler) ->
        decltype(dispatchInternal<TAllMessages>(msg, handler, PrimaryDispatchTag()))
    {
        return dispatchInternal<TAllMessages>(msg, handler, PrimaryDispatchTag());
    }

    /// @brief Compile time inquiry whether polymorphic dispatch tables are
    ///     generated internally to map message ID to actual type.
    /// @see @ref page_dispatch
    /// @see @ref isDispatchStaticBinSearch()
    /// @see @ref isDispatchLinearSwitch()
    template <typename TAllMessages>
    static constexpr bool isDispatchPolymorphic()
    {
        return isDispatchPolymorphicInternal<TAllMessages>(PrimaryDispatchTag());
    }

    /// @brief Compile time inquiry whether static binary search dispatch is
    ///     generated internally to map message ID to actual type.
    /// @see @ref page_dispatch
    /// @see @ref isDispatchPolymorphic()
    /// @see @ref isDispatchLinearSwitch()
    template <typename TAllMessages>
    static constexpr bool isDispatchStaticBinSearch()
    {
        return isDispatchStaticBinSearchInternal<TAllMessages>(PrimaryDispatchTag());
    }

    /// @brief Compile time inquiry whether linear switch dispatch is
    ///     generated internally to map message ID to actual type.
    /// @see @ref page_dispatch
    /// @see @ref isDispatchStaticBinSearch()
    /// @see @ref isDispatchLinearSwitch()
    template <typename TAllMessages>
    static constexpr bool isDispatchLinearSwitch()
    {
        return isDispatchLinearSwitchInternal<TAllMessages>(PrimaryDispatchTag());
    }
};

template <typename T>
constexpr bool isMsgDispatcher()
{
    return details::hasMsgDispatcherTag<T>();
}

} // namespace comms
