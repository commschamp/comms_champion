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
class DispatchMsgStrongStaticBinSearchHelper    
{
    static_assert(TFrom + TCount <= std::tuple_size<TAllMessages>::value, 
        "Invalid template params");

    static_assert(2 <= TCount, "Invalid invocation");
    static const std::size_t Mid = TFrom + (TCount / 2);        
    using MidElem = typename std::tuple_element<Mid, TAllMessages>::type;
    static_assert(messageHasStaticNumId<MidElem>(), "Message must define static ID");

public:
    template <
        typename TMsg,
        typename THandler>
    static auto dispatch(typename TMsg::MsgIdParamType id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        typename TMsg::MsgIdParamType midId = MidElem::doGetId();
        if (id < midId) {
            return DispatchMsgStrongStaticBinSearchHelper<TAllMessages, TFrom, Mid - TFrom>::dispatch(
                    id, msg, handler);
        }

        if (midId < id) {
            return DispatchMsgStrongStaticBinSearchHelper<TAllMessages, Mid, TCount - (Mid - TFrom)>::dispatch(
                    id, msg, handler);
        }

        auto& castedMsg = static_cast<MidElem&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }
};

template <typename TAllMessages, std::size_t TFrom>
class DispatchMsgStrongStaticBinSearchHelper<TAllMessages, TFrom, 1>
{
    static_assert(TFrom + 1 <= std::tuple_size<TAllMessages>::value, 
        "Invalid template params");

    using Elem = typename std::tuple_element<TFrom, TAllMessages>::type;
    static_assert(messageHasStaticNumId<Elem>(), "Message must define static ID");

public:
    template <
        typename TMsg,
        typename THandler>
    static auto dispatch(typename TMsg::MsgIdParamType id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        typename TMsg::MsgIdParamType elemId = Elem::doGetId();
        if (id != elemId) {
            return static_cast<RetType>(handler.handle(msg));
        }

        auto& castedMsg = static_cast<Elem&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }
};

template <typename TAllMessages, std::size_t TFrom>
class DispatchMsgStrongStaticBinSearchHelper<TAllMessages, TFrom, 0>
{
public:
    template <
        typename TMsg,
        typename THandler>
    static auto dispatch(typename TMsg::MsgIdParamType id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(id);

        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        return static_cast<RetType>(handler.handle(msg));
    }
};

template <typename TAllMessages, std::size_t TIdx>
class DispatchMsgWeakStartIdxFinder
{
    using CurrMsgType = typename std::tuple_element<TIdx, TAllMessages>::type;
    using PrevMsgType = typename std::tuple_element<TIdx - 1, TAllMessages>::type;
    static const bool IdsMatch = CurrMsgType::doGetId() == PrevMsgType::doGetId();
public:
    static const std::size_t Value = IdsMatch ? DispatchMsgWeakStartIdxFinder<TAllMessages, TIdx - 1>::Value : TIdx;
};

template <typename TAllMessages>
class DispatchMsgWeakStartIdxFinder<TAllMessages, 0U>
{
public:
    static const std::size_t Value = 0U;
};

template <typename TAllMessages, std::size_t TOrigIdx, std::size_t TRem>
class DispatchMsgWeakCountFinder
{
    using OrigMsgType = typename std::tuple_element<TOrigIdx, TAllMessages>::type;
    static const std::size_t Idx = std::tuple_size<TAllMessages>::value - TRem;
    using CurrMsgType = typename std::tuple_element<Idx, TAllMessages>::type;
    static const bool IdsMatch = OrigMsgType::doGetId() == CurrMsgType::doGetId();
public:
    static const std::size_t Value = IdsMatch ? DispatchMsgWeakCountFinder<TAllMessages, TOrigIdx, TRem - 1>::Value + 1 : 0U;
};

template <typename TAllMessages, std::size_t TOrigIdx>
class DispatchMsgWeakCountFinder<TAllMessages, TOrigIdx, 0U>
{
public:
    static const std::size_t Value = 0U;
};

template <typename TAllMessages, std::size_t TFrom, std::size_t TCount>
class DispatchMsgWeakStaticBinSearchHelper    
{
    static_assert(TFrom < std::tuple_size<TAllMessages>::value, 
        "Invalid template params");

    static_assert(TFrom + TCount <= std::tuple_size<TAllMessages>::value, 
        "Invalid template params");

    static_assert(2 <= TCount, "Invalid invocation");
    static const std::size_t Mid = TFrom + (TCount / 2);        
    static_assert(Mid < std::tuple_size<TAllMessages>::value, 
        "Invalid template params");

    using MidElem = typename std::tuple_element<Mid, TAllMessages>::type;
    static_assert(messageHasStaticNumId<MidElem>(), "Message must define static ID");
    
    static const std::size_t MidRangeStartIdxTmp = DispatchMsgWeakStartIdxFinder<TAllMessages, Mid>::Value;
    static_assert(MidRangeStartIdxTmp <= Mid, "Invalid calculation");
    static const std::size_t MidRangeStartIdx = 
        MidRangeStartIdxTmp < TFrom ? TFrom : MidRangeStartIdxTmp;

    static const std::size_t MidRangeCountTmp = 
        DispatchMsgWeakCountFinder<
            TAllMessages, 
            MidRangeStartIdx, 
            std::tuple_size<TAllMessages>::value - MidRangeStartIdx
        >::Value;
    static const std::size_t MidRangeCount = 
        ((TCount - MidRangeStartIdx) < MidRangeCountTmp) ? (TCount - MidRangeStartIdx) : MidRangeCountTmp;
    
    static_assert(MidRangeStartIdx <= Mid, "Invalid calculation");
    static_assert(MidRangeStartIdx < std::tuple_size<TAllMessages>::value, 
        "Invalid template params");

    static_assert(MidRangeStartIdx + MidRangeCount <= std::tuple_size<TAllMessages>::value, 
        "Invalid template params");

public:
    template <
        typename TMsg,
        typename THandler>
    static auto dispatch(typename TMsg::MsgIdParamType id, std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        typename TMsg::MsgIdParamType midId = MidElem::doGetId();
        if (id < midId) {
            return 
                DispatchMsgWeakStaticBinSearchHelper<TAllMessages, TFrom, MidRangeStartIdx - TFrom>::
                    dispatch(id, offset, msg, handler);
        }

        if (midId < id) {
            static const std::size_t NewStart = MidRangeStartIdx + MidRangeCount; 
            return 
                DispatchMsgWeakStaticBinSearchHelper<TAllMessages, NewStart, TCount - (NewStart - TFrom)>::
                    dispatch(id, offset, msg, handler);
        }

        if (MidRangeCount <= offset) {
            return static_cast<RetType>(handler.handle(msg));
        }

        return 
            DispatchMsgWeakStaticBinSearchHelper<TAllMessages, MidRangeStartIdx, MidRangeCount>::
                dispatchOffset(offset, msg, handler);

        auto& castedMsg = static_cast<MidElem&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }

    template <
        typename TMsg,
        typename THandler>
    static auto dispatchOffset(std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        static const std::size_t MidOffset = TCount / 2;

        if (offset < MidOffset) {
            return 
                DispatchMsgWeakStaticBinSearchHelper<TAllMessages, TFrom, MidOffset>::
                    dispatchOffset(offset, msg, handler);
        }

        if (MidOffset < offset) {
            return 
                DispatchMsgWeakStaticBinSearchHelper<TAllMessages, MidOffset, TCount - (MidOffset - TFrom) >::
                    dispatchOffset(offset - MidOffset, msg, handler);
        }

        auto& castedMsg = static_cast<MidElem&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }
};

template <typename TAllMessages, std::size_t TFrom>
class DispatchMsgWeakStaticBinSearchHelper<TAllMessages, TFrom, 1>
{
    static_assert(TFrom + 1 <= std::tuple_size<TAllMessages>::value, 
        "Invalid template params");

    using Elem = typename std::tuple_element<TFrom, TAllMessages>::type;
    static_assert(messageHasStaticNumId<Elem>(), "Message must define static ID");

public:
    template <
        typename TMsg,
        typename THandler>
    static auto dispatch(typename TMsg::MsgIdParamType id, std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        if (offset != 0U) {
            return static_cast<RetType>(handler.handle(msg));
        }

        typename TMsg::MsgIdParamType elemId = Elem::doGetId();
        if (id != elemId) {
            return static_cast<RetType>(handler.handle(msg));
        }

        auto& castedMsg = static_cast<Elem&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }

    template <
        typename TMsg,
        typename THandler>
    static auto dispatchOffset(std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        if (offset != 0U) {
            return static_cast<RetType>(handler.handle(msg));
        }

        auto& castedMsg = static_cast<Elem&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }
};

template <typename TAllMessages, std::size_t TFrom>
class DispatchMsgWeakStaticBinSearchHelper<TAllMessages, TFrom, 0>
{
public:
    template <
        typename TMsg,
        typename THandler>
    static auto dispatch(typename TMsg::MsgIdParamType id, std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(id);
        static_cast<void>(offset);

        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        return static_cast<RetType>(handler.handle(msg));
    }

    template <
        typename TMsg,
        typename THandler>
    static auto dispatchOffset(std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(offset);

        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        return static_cast<RetType>(handler.handle(msg));
    }
};

template <typename TAllMessages>
class DispatchMsgStaticBinSearchHelper    
{
    struct StrongTag {};
    struct WeakTag {};

    using BinSearchTag = 
        typename std::conditional<
            allMessagesAreStrongSorted<TAllMessages>(),
            StrongTag,
            WeakTag
        >::type;

public:
    template <
        typename TMsg,
        typename THandler>
    static auto dispatch(TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(MsgType::hasGetId(), 
            "The used message object must provide polymorphic ID retrieval function");
        static_assert(MsgType::hasMsgIdType(), 
            "Message interface class must define its id type");            
        return dispatch(msg.getId(), msg, handler, BinSearchTag());
    }

    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(MsgType::hasMsgIdType(), 
            "Message interface class must define its id type");            

        using MsgIdParamType = typename MsgType::MsgIdParamType;
        return dispatchInternal(static_cast<MsgIdParamType>(id), msg, handler, BinSearchTag());
    }

    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(MsgType::hasMsgIdType(), 
            "Message interface class must define its id type");            

        using MsgIdParamType = typename MsgType::MsgIdParamType;
        return dispatchInternal(static_cast<MsgIdParamType>(id), offset, msg, handler, BinSearchTag());
    }

private:
    template <
        typename TMsg,
        typename THandler>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, TMsg& msg, THandler& handler, StrongTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return 
            DispatchMsgStrongStaticBinSearchHelper<TAllMessages, 0, std::tuple_size<TAllMessages>::value>::
                dispatch(id, msg, handler);
    }

    template <
        typename TMsg,
        typename THandler>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, std::size_t offset, TMsg& msg, THandler& handler, StrongTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        if (offset != 0U) {
            using RetType = 
                MessageInterfaceDispatchRetType<
                    typename std::decay<decltype(handler)>::type>;
            return static_cast<RetType>(handler.handle(msg));

        }
        return 
            DispatchMsgStrongStaticBinSearchHelper<TAllMessages, 0, std::tuple_size<TAllMessages>::value>::
                dispatch(id, msg, handler);

    }

    template <
        typename TMsg,
        typename THandler>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, TMsg& msg, THandler& handler, WeakTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return dispatchInternal(id, 0U, msg, handler, WeakTag());
    }

    template <
        typename TMsg,
        typename THandler>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, std::size_t offset, TMsg& msg, THandler& handler, WeakTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return 
            DispatchMsgWeakStaticBinSearchHelper<TAllMessages, 0, std::tuple_size<TAllMessages>::value>::
                dispatch(id, offset, msg, handler);

    }
};

} // namespace details

} // namespace comms