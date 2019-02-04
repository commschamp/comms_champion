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

#pragma once

#include <type_traits>

#include "comms/Message.h"
#include "comms/details/message_check.h"

namespace comms
{

namespace details
{

template <typename TAllMessages, std::size_t TFrom, std::size_t TCount>
class DispatchMsgStaticBinSearchHelper    
{
    static_assert(TFrom + TCount <= std::tuple_size<TAllMessages>::value, 
        "Invalid template params");

    static_assert(2 <= TCount, "Invalid invocation");
    static const std::size_t TMid = TFrom + (TCount / 2);        
    using MidElem = typename std::tuple_element<TMid, TAllMessages>::type;
    static_assert(messageHasStaticNumId<MidElem>(), "Message must define static ID");

public:
    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, std::size_t idx, TMsg& msg, THandler& handler) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            details::MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        using IdType = typename std::decay<decltype(id)>::type;

        auto midId = static_cast<IdType>(MidElem::doGetId());
        if (id < midId) {
            return DispatchMsgStaticBinSearchHelper<TAllMessages, TFrom, TMid - TFrom>::dispatch(
                    std::forward<TId>(id), idx, msg, handler);
        }

        if (midId < id) {
            return DispatchMsgStaticBinSearchHelper<TAllMessages, TMid, TCount - (TMid - TFrom)>::dispatch(
                    std::forward<TId>(id), idx, msg, handler);
        }

        if (0U < idx) {
            return DispatchMsgStaticBinSearchHelper<TAllMessages, TFrom + 1, TCount - 1>::dispatch(
                    std::forward<TId>(id), idx - 1, msg, handler);
        }

        auto& castedMsg = static_cast<MidElem&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }
};

template <typename TAllMessages, std::size_t TFrom>
class DispatchMsgStaticBinSearchHelper<TAllMessages, TFrom, 1>
{
    static_assert(TFrom + 1 <= std::tuple_size<TAllMessages>::value, 
        "Invalid template params");

    using Elem = typename std::tuple_element<TFrom, TAllMessages>::type;
    static_assert(messageHasStaticNumId<Elem>(), "Message must define static ID");

public:
    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, std::size_t idx, TMsg& msg, THandler& handler) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            details::MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        using IdType = typename std::decay<decltype(id)>::type;

        auto elemId = static_cast<IdType>(Elem::doGetId());
        if ((id != elemId) || (0U < idx)) {
            return static_cast<RetType>(handler.handle(msg));
        }

        auto& castedMsg = static_cast<Elem&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }
};

template <typename TAllMessages, std::size_t TFrom>
class DispatchMsgStaticBinSearchHelper<TAllMessages, TFrom, 0>
{
public:
    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, std::size_t idx, TMsg& msg, THandler& handler) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(id);
        static_cast<void>(idx);

        using RetType = 
            details::MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        return static_cast<RetType>(handler.handle(msg));
    }
};



} // namespace details

} // namespace comms