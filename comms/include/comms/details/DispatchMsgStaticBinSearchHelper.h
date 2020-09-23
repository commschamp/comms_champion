//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>

#include "comms/Message.h"
#include "comms/details/message_check.h"
#include "comms/util/type_traits.h"
#include "comms/details/tag.h"
#include "DispatchMsgHelperType.h"

namespace comms
{

namespace details
{
    

template <DispatchMsgTypeEnum TType>
class DispatchMsgStrongStaticBinSearchHelper // <DispatchMsgTypeEnum::Multiple>
{
    template <std::size_t TFrom, std::size_t TCount>
    using MidIdx = 
        std::integral_constant<std::size_t, (TFrom + (TCount / 2))>; 

    template <typename TAllMessages, std::size_t TFrom, std::size_t TCount>
    using MidElem = 
        typename comms::util::TupleElement<TAllMessages>::template Type<
            MidIdx<TFrom, TCount>::value
        >;

    // static_assert(TFrom + TCount <= std::tuple_size<TAllMessages>::value, 
    //     "Invalid template params");

    // static_assert(2 <= TCount, "Invalid invocation");
    // static_assert(messageHasStaticNumId<MidElem>(), "Message must define static ID");

public:
    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,
        typename TId,   
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        static constexpr std::size_t Mid = MidIdx<TFrom, TCount>::value;
        using MidElemType = MidElem<TAllMessages, TFrom, TCount>;

        typename TMsg::MsgIdParamType midId = MidElemType::doGetId();
        if (id < midId) {
            static constexpr auto NextCount = Mid - TFrom;
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgStrongStaticBinSearchHelper<HelperType>::template 
                    dispatch<TAllMessages, TFrom, NextCount>(
                        id, msg, handler);
        }

        if (midId < id) {
            static constexpr auto NextCount = TCount - (Mid - TFrom);
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgStrongStaticBinSearchHelper<HelperType>::template 
                    dispatch<TAllMessages, Mid, NextCount>(
                        id, msg, handler);
        }

        auto& castedMsg = static_cast<MidElemType&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,    
        typename TId, 
        typename THandler>
    static bool dispatchType(TId&& id, THandler& handler)
    {
        static constexpr std::size_t Mid = MidIdx<TFrom, TCount>::value;
        using MidElemType = MidElem<TAllMessages, TFrom, TCount>;
        typename MidElemType::MsgIdParamType midId = MidElemType::doGetId();
        if (id < midId) {
            static constexpr auto NextCount = Mid - TFrom;
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgStrongStaticBinSearchHelper<HelperType>::template
                    dispatchType<TAllMessages, TFrom, NextCount>(id, handler);
        }

        if (midId < id) {
            static constexpr auto NextCount = TCount - (Mid - TFrom);
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgStrongStaticBinSearchHelper<HelperType>::template
                    dispatchType<TAllMessages, Mid, NextCount>(id, handler);
        }

        handler.template handle<MidElemType>();
        return true;
    }    

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,    
        typename TId>
    static std::size_t dispatchTypeCount(TId&& id)
    {
        static constexpr std::size_t Mid = MidIdx<TFrom, TCount>::value;
        using MidElemType = MidElem<TAllMessages, TFrom, TCount>;
        typename MidElemType::MsgIdParamType midId = MidElemType::doGetId();
        if (id < midId) {
            static constexpr auto NextCount = Mid - TFrom;
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgStrongStaticBinSearchHelper<HelperType>::template
                    dispatchTypeCount<TAllMessages, TFrom, NextCount>(id);
        }

        if (midId < id) {
            static constexpr auto NextCount = TCount - (Mid - TFrom);
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgStrongStaticBinSearchHelper<HelperType>::template
                    dispatchTypeCount<TAllMessages, Mid, NextCount>(id);
        }

        return 1U;
    }    
};

template <>
class DispatchMsgStrongStaticBinSearchHelper<DispatchMsgTypeEnum::Single>
{
    // static_assert(TFrom + 1 <= std::tuple_size<TAllMessages>::value, 
    //     "Invalid template params");

    template <typename TAllMessages, std::size_t TFrom>
    using Elem = 
        typename comms::util::TupleElement<TAllMessages>::template Type<TFrom>;
    // static_assert(messageHasStaticNumId<Elem>(), "Message must define static ID");

public:
    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        using ElemType = Elem<TAllMessages, TFrom>;

        typename TMsg::MsgIdParamType elemId = ElemType::doGetId();
        if (id != elemId) {
            return static_cast<RetType>(handler.handle(msg));
        }

        auto& castedMsg = static_cast<ElemType&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,
        typename TId,
        typename THandler>
    static bool dispatchType(TId&& id, THandler& handler)
    {
        using ElemType = Elem<TAllMessages, TFrom>;
        typename ElemType::MsgIdParamType elemId = ElemType::doGetId();
        if (id != elemId) {
            return false;
        }

        handler.template handle<ElemType>();
        return true;
    }      

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,
        typename TId>
    static std::size_t dispatchTypeCount(TId&& id)
    {
        using ElemType = Elem<TAllMessages, TFrom>;
        typename ElemType::MsgIdParamType elemId = ElemType::doGetId();
        if (id != elemId) {
            return 0U;
        }

        return 1U;
    }      
};

template <>
class DispatchMsgStrongStaticBinSearchHelper<DispatchMsgTypeEnum::None>
{
public:
    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(id);

        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        return static_cast<RetType>(handler.handle(msg));
    }

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,
        typename TId,
        typename THandler>
    static bool dispatchType(TId&& id, THandler& handler)
    {
        static_cast<void>(id);
        static_cast<void>(handler);
        return false;
    }     

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,
        typename TId>
    static std::size_t dispatchTypeCount(TId&& id)
    {
        static_cast<void>(id);
        return 0U;
    }      
};

template <bool TFirst>
class DispatchMsgWeakStartIdxFinder //<false>
{
    template <typename TAllMessages, std::size_t TIdx>
    using CurrMsgType = 
        typename comms::util::TupleElement<TAllMessages>::template Type<TIdx>;

    template <typename TAllMessages, std::size_t TIdx>
    using PrevMsgType = 
        typename comms::util::TupleElement<TAllMessages>::template Type<TIdx - 1>;

public:
    template <typename TAllMessages, std::size_t TIdx>
    using Type = 
        typename comms::util::Conditional<
            CurrMsgType<TAllMessages, TIdx>::doGetId() == PrevMsgType<TAllMessages, TIdx>::doGetId()
        >::template Type<
            typename DispatchMsgWeakStartIdxFinder<(TIdx <= 1)>::template Type<TAllMessages, TIdx - 1U>,
            std::integral_constant<std::size_t, TIdx>
        >;
};

template <>
class DispatchMsgWeakStartIdxFinder<true>
{
public:
    template <typename TAllMessages, std::size_t TIdx>
    using Type = std::integral_constant<std::size_t, TIdx>;
};

template <bool THasElems>
class DispatchMsgWeakCountFinder //<true>
{
    template <typename TAllMessages, std::size_t TIdx>
    using OrigMsgType = 
        typename comms::util::TupleElement<TAllMessages>::template Type<TIdx>;

    template <typename TAllMessages, std::size_t TIdx, std::size_t TCount>
    using CurrMsgType = 
        typename comms::util::TupleElement<TAllMessages>::template Type<
            std::tuple_size<TAllMessages>::value - TCount
        >;

public:
    template <typename TAllMessages, std::size_t TOrigIdx, std::size_t TRem>
    using Type = 
        typename comms::util::Conditional<
            OrigMsgType<TAllMessages, TOrigIdx>::doGetId() == CurrMsgType<TAllMessages, TOrigIdx, TRem>::doGetId()
        >::template Type<
            std::integral_constant<
                std::size_t,
                1U + DispatchMsgWeakCountFinder<(1U < TRem)>::template Type<TAllMessages, TOrigIdx, TRem - 1>::value
            >,
            std::integral_constant<std::size_t, 0U>
        >;
};

template <>
class DispatchMsgWeakCountFinder<false>
{
public:
    template <typename TAllMessages, std::size_t TOrigIdx, std::size_t TRem>
    using Type = std::integral_constant<std::size_t, 0U>;
};

//template <typename TAllMessages, std::size_t TFrom, std::size_t TCount>
template <DispatchMsgTypeEnum TType>
class DispatchMsgWeakStaticBinSearchHelper // <DispatchMsgTypeEnum::Multiple>
{
    // static_assert(TFrom < std::tuple_size<TAllMessages>::value, 
    //     "Invalid template params");

    // static_assert(TFrom + TCount <= std::tuple_size<TAllMessages>::value, 
    //     "Invalid template params");

    // static_assert(2 <= TCount, "Invalid invocation");

    template <std::size_t TFrom, std::size_t TCount>
    using MidIdx = 
        std::integral_constant<std::size_t, (TFrom + (TCount / 2))>; 

    template <typename TAllMessages, std::size_t TFrom, std::size_t TCount>
    using MidElem = 
        typename comms::util::TupleElement<TAllMessages>::template Type<
            MidIdx<TFrom, TCount>::value
        >;

    // static_assert(Mid < std::tuple_size<TAllMessages>::value, 
    //     "Invalid template params");

    // static_assert(messageHasStaticNumId<MidElem>(), "Message must define static ID");
    
    template <typename TAllMessages, std::size_t TFrom, std::size_t TCount>
    using MidRangeStartIdxTmp = 
        typename DispatchMsgWeakStartIdxFinder<
            MidIdx<TFrom, TCount>::value == 0U
        >::template Type<
            TAllMessages,
            MidIdx<TFrom, TCount>::value
        >;

    // static_assert(MidRangeStartIdxTmp <= Mid, "Invalid calculation");
    template <typename TAllMessages, std::size_t TFrom, std::size_t TCount>
    using MidRangeStartIdx = 
        typename comms::util::IntMaxBinaryOp<>::template Type<
            MidRangeStartIdxTmp<TAllMessages, TFrom, TCount>,
            std::integral_constant<std::size_t, TFrom>
        >;

    template <typename TAllMessages, std::size_t TFrom, std::size_t TCount>
    using RemCount = 
        std::integral_constant<
            std::size_t,
            std::tuple_size<TAllMessages>::value - MidRangeStartIdx<TAllMessages, TFrom, TCount>::value
        >;

    template <typename TAllMessages, std::size_t TFrom, std::size_t TCount>
    using MidRangeCountTmp = 
        typename DispatchMsgWeakCountFinder<
            (0U < RemCount<TAllMessages, TFrom, TCount>::value)
        >::template Type<
            TAllMessages, 
            MidRangeStartIdx<TAllMessages, TFrom, TCount>::value, 
            RemCount<TAllMessages, TFrom, TCount>::value
        >;

    template <typename TAllMessages, std::size_t TFrom, std::size_t TCount>
    using MidRangeCount = 
        typename comms::util::IntMinBinaryOp<>::template Type<
            std::integral_constant<std::size_t, TCount - MidRangeStartIdx<TAllMessages, TFrom, TCount>::value>,
            MidRangeCountTmp<TAllMessages, TFrom, TCount>
        >;
    
    // static_assert(MidRangeStartIdx < std::tuple_size<TAllMessages>::value, 
    //     "Invalid template params");

    // static_assert(MidRangeStartIdx + MidRangeCount <= std::tuple_size<TAllMessages>::value, 
    //     "Invalid template params");

public:
    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,    
        typename TMsg,
        typename THandler>
    static auto dispatch(typename TMsg::MsgIdParamType id, std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        using MidElemType = MidElem<TAllMessages, TFrom, TCount>;

        typename TMsg::MsgIdParamType midId = MidElemType::doGetId();
        static constexpr std::size_t StartIdx = 
            MidRangeStartIdx<TAllMessages, TFrom, TCount>::value;

        // static_assert(StartIdx <= MidIdx<TFrom, TCount>::value, "Invalid calculation");

        if (id < midId) {
            static constexpr std::size_t NextCount = StartIdx - TFrom;
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgWeakStaticBinSearchHelper<HelperType>::template 
                    dispatch<TAllMessages, TFrom, NextCount>(id, offset, msg, handler);
        }

        static constexpr std::size_t MidCount = MidRangeCount<TAllMessages, TFrom, TCount>::value;

        if (midId < id) {
            static constexpr std::size_t NewStart = StartIdx + MidCount;
            static constexpr std::size_t NextCount = TCount - (NewStart - TFrom);
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            
            return 
                DispatchMsgWeakStaticBinSearchHelper<HelperType>::template 
                    dispatch<TAllMessages, NewStart, NextCount>(id, offset, msg, handler);
        }

        if (MidCount <= offset) {
            return static_cast<RetType>(handler.handle(msg));
        }

        static constexpr auto HelperType = DispatchMsgHelperIntType<MidCount>::value;
        return 
            DispatchMsgWeakStaticBinSearchHelper<HelperType>::template 
                dispatchOffset<TAllMessages, StartIdx, MidCount>(offset, msg, handler);
    }

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,        
        typename TMsg,
        typename THandler>
    static auto dispatchOffset(std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        static constexpr std::size_t MidOffset = TCount / 2;

        if (offset < MidOffset) {
            static constexpr auto HelperType = DispatchMsgHelperIntType<MidOffset>::value;
            return 
                DispatchMsgWeakStaticBinSearchHelper<HelperType>::template 
                    dispatchOffset<TAllMessages, TFrom, MidOffset>(offset, msg, handler);
        }

        if (MidOffset < offset) {
            static constexpr std::size_t NextCount = TCount - (MidOffset - TFrom);
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgWeakStaticBinSearchHelper<HelperType>::template 
                    dispatchOffset<TAllMessages, MidOffset, NextCount>(offset - MidOffset, msg, handler);
        }

        using MidElemType = MidElem<TAllMessages, TFrom, TCount>;
        auto& castedMsg = static_cast<MidElemType&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,
        typename TId,    
        typename THandler>
    static bool dispatchType(TId&& id, std::size_t offset, THandler& handler) 
    {
        using MidElemType = MidElem<TAllMessages, TFrom, TCount>;
        static constexpr std::size_t StartIdx = 
            MidRangeStartIdx<TAllMessages, TFrom, TCount>::value;

        static_assert(StartIdx <= MidIdx<TFrom, TCount>::value, "Invalid calculation");

        typename MidElemType::MsgIdParamType midId = MidElemType::doGetId();
        if (id < midId) {
            static constexpr std::size_t NextCount = StartIdx - TFrom;
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgWeakStaticBinSearchHelper<HelperType>::template 
                    dispatchType<TAllMessages, TFrom, NextCount>(id, offset, handler);
        }

        static constexpr std::size_t MidCount = MidRangeCount<TAllMessages, TFrom, TCount>::value;

        if (midId < id) {
            static constexpr std::size_t NewStart = StartIdx + MidCount; 
            static constexpr std::size_t NextCount = TCount - (NewStart - TFrom);
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgWeakStaticBinSearchHelper<HelperType>::template 
                    dispatchType<TAllMessages, NewStart, NextCount>(id, offset, handler);
        }

        if (MidCount <= offset) {
            return false;
        }

        static constexpr auto HelperType = DispatchMsgHelperIntType<MidCount>::value;
        return 
            DispatchMsgWeakStaticBinSearchHelper<HelperType>::template 
                dispatchTypeOffset<TAllMessages, StartIdx, MidCount>(offset, handler);
    }    

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,    
        typename THandler>
    static bool dispatchTypeOffset(std::size_t offset, THandler& handler)
    {
        static constexpr std::size_t MidOffset = TCount / 2;

        if (offset < MidOffset) {
            static constexpr auto HelperType = DispatchMsgHelperIntType<MidOffset>::value;
            return 
                DispatchMsgWeakStaticBinSearchHelper<HelperType>::template 
                    dispatchTypeOffset<TAllMessages, TFrom, MidOffset>(offset, handler);
        }

        if (MidOffset < offset) {
            static constexpr std::size_t NextCount = TCount - (MidOffset - TFrom);
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgWeakStaticBinSearchHelper<HelperType>::template 
                    dispatchTypeOffset<TAllMessages, MidOffset, NextCount>(offset - MidOffset, handler);
        }

        using MidElemType = MidElem<TAllMessages, TFrom, TCount>;
        handler.template handle<MidElemType>();
        return true;
    }    

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,    
        typename TId>
    static std::size_t dispatchTypeCount(TId&& id) 
    {
        static constexpr std::size_t StartIdx = MidRangeStartIdx<TAllMessages, TFrom, TCount>::value;
        static_assert(StartIdx <= MidIdx<TFrom, TCount>::value, "Invalid calculation");
        
        using MidElemType = MidElem<TAllMessages, TFrom, TCount>;
        typename MidElemType::MsgIdParamType midId = MidElemType::doGetId();
        if (id < midId) {
            static constexpr std::size_t NextCount = StartIdx - TFrom;
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgWeakStaticBinSearchHelper<HelperType>::template 
                    dispatchTypeCount<TAllMessages, TFrom, NextCount>(id);
        }

        static constexpr std::size_t MidCount = MidRangeCount<TAllMessages, TFrom, TCount>::value;

        if (midId < id) {
            static constexpr std::size_t NewStart = StartIdx + MidCount; 
            static constexpr std::size_t NextCount = TCount - (NewStart - TFrom);
            static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
            return 
                DispatchMsgWeakStaticBinSearchHelper<HelperType>::template 
                    dispatchTypeCount<TAllMessages, NewStart, NextCount>(id);
        }

        return MidCount;
    }    
};

template <>
class DispatchMsgWeakStaticBinSearchHelper<DispatchMsgTypeEnum::Single>
{
    // static_assert(TFrom + 1 <= std::tuple_size<TAllMessages>::value, 
    //     "Invalid template params");

    // static_assert(messageHasStaticNumId<Elem>(), "Message must define static ID");

    template <typename TAllMessages, std::size_t TFrom>
    using Elem = 
        typename comms::util::TupleElement<TAllMessages>::template Type<TFrom>;

public:
    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,     
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

        using ElemType = Elem<TAllMessages, TFrom>;
        typename TMsg::MsgIdParamType elemId = ElemType::doGetId();
        if (id != elemId) {
            return static_cast<RetType>(handler.handle(msg));
        }

        auto& castedMsg = static_cast<ElemType&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,    
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

        using ElemType = Elem<TAllMessages, TFrom>;
        auto& castedMsg = static_cast<ElemType&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount, 
        typename TId,    
        typename THandler>
    static bool dispatchType(TId&& id, std::size_t offset, THandler& handler) 
    {
        if (offset != 0U) {
            return false;
        }

        using ElemType = Elem<TAllMessages, TFrom>;
        typename ElemType::MsgIdParamType elemId = ElemType::doGetId();
        if (id != elemId) {
            return false;
        }

        handler.template handle<ElemType>();
        return true;
    }

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,     
        typename THandler>
    static bool dispatchTypeOffset(std::size_t offset, THandler& handler) 
    {
        if (offset != 0U) {
            return false;
        }

        using ElemType = Elem<TAllMessages, TFrom>;
        handler.template handle<ElemType>();
        return true;
    }    

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,     
        typename TId>
    static std::size_t dispatchTypeCount(TId&& id)
    {
        using ElemType = Elem<TAllMessages, TFrom>;
        typename ElemType::MsgIdParamType elemId = ElemType::doGetId();
        if (id != elemId) {
            return 0U;
        }

        return 1U;
    }
};

template <>
class DispatchMsgWeakStaticBinSearchHelper<DispatchMsgTypeEnum::None>
{
public:
    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,    
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
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,    
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

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,    
        typename TId, 
        typename THandler>
    static bool dispatchType(TId&& id, std::size_t offset, THandler& handler)
    {
        static_cast<void>(id);
        static_cast<void>(offset);
        static_cast<void>(handler);
        return false;
    }      

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,    
        typename THandler>
    static bool dispatchTypeOffset(std::size_t offset, THandler& handler) 
    {
        static_cast<void>(offset);
        static_cast<void>(handler);
        return false;
    }        

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,    
        typename TId>
    static std::size_t dispatchTypeCount(TId&& id)
    {
        static_cast<void>(id);
        return 0U;
    }

};

template <typename...>
class DispatchMsgStaticBinSearchHelper    
{
    template <typename... TParams>    
    using EmptyTag = comms::details::tag::Tag1<>;

    template <typename... TParams>    
    using StrongTag = comms::details::tag::Tag2<>;

    template <typename... TParams>    
    using WeakTag = comms::details::tag::Tag3<>;

    template<typename TAllMessages, typename...>
    using StrongWeakTag = 
        typename comms::util::LazyShallowConditional<
            allMessagesAreStrongSorted<TAllMessages>()
        >::template Type<
            StrongTag,
            WeakTag,
            TAllMessages
        >;
    
    template <typename TAllMessages, typename...>
    using BinSearchTag = 
        typename comms::util::LazyShallowConditional<
            std::tuple_size<TAllMessages>::value == 0U
        >::template Type<
            EmptyTag,
            StrongWeakTag,
            TAllMessages
        >;

    template <typename TAllMessages, typename TMsg>
    using AdjustedTag =
        typename comms::util::LazyShallowConditional<
            comms::isMessageBase<TMsg>()
        >::template Type<
            EmptyTag,
            BinSearchTag,
            TAllMessages
        >;

public:
    template <typename TAllMessages, typename TMsg, typename THandler>
    static auto dispatch(TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(MsgType::hasGetId(), 
            "The used message object must provide polymorphic ID retrieval function");
        static_assert(MsgType::hasMsgIdType(), 
            "Message interface class must define its id type");            
        return dispatch<TAllMessages>(msg.getId(), msg, handler, AdjustedTag<TAllMessages, MsgType>());
    }

    template <
        typename TAllMessages,
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
        return dispatchInternal<TAllMessages>(static_cast<MsgIdParamType>(id), msg, handler, AdjustedTag<TAllMessages, MsgType>());
    }

    template <
        typename TAllMessages,
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
        return dispatchInternal<TAllMessages>(static_cast<MsgIdParamType>(id), offset, msg, handler, AdjustedTag<TAllMessages, MsgType>());
    }

    template <typename TAllMessages, typename TId, typename THandler>
    static bool dispatchType(TId&& id, THandler& handler)
    {
        return dispatchTypeInternal<TAllMessages>(std::forward<TId>(id), handler, BinSearchTag<TAllMessages>());
    }

    template <typename TAllMessages, typename TId, typename THandler>
    static bool dispatchType(TId&& id, std::size_t offset, THandler& handler)
    {
        return dispatchTypeInternal<TAllMessages>(std::forward<TId>(id), offset, handler, BinSearchTag<TAllMessages>());
    }        

    template <typename TAllMessages, typename TId>
    static std::size_t dispatchTypeCount(TId&& id)
    {
        return dispatchTypeCountInternal<TAllMessages>(std::forward<TId>(id), BinSearchTag<TAllMessages>());
    }

private:
    template <typename TAllMessages, typename TMsg, typename THandler, typename... TParams>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, TMsg& msg, THandler& handler, EmptyTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return dispatchInternal<TAllMessages>(id, 0U, msg, handler, EmptyTag<>());
    }

    template <typename TAllMessages, typename TMsg, typename THandler, typename... TParams>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, std::size_t offset, TMsg& msg, THandler& handler, EmptyTag<TParams...>) ->
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

    template <typename TAllMessages, typename TMsg, typename THandler, typename... TParams>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, TMsg& msg, THandler& handler, StrongTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static constexpr std::size_t Count = std::tuple_size<TAllMessages>::value;
        static constexpr auto HelperType = DispatchMsgHelperIntType<Count>::value;
        return 
            DispatchMsgStrongStaticBinSearchHelper<HelperType>::template
                dispatch<TAllMessages, 0, Count>(id, msg, handler);
    }

    template <typename TAllMessages, typename TMsg, typename THandler, typename... TParams>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, std::size_t offset, TMsg& msg, THandler& handler, StrongTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        if (offset != 0U) {
            using RetType = 
                MessageInterfaceDispatchRetType<
                    typename std::decay<decltype(handler)>::type>;
            return static_cast<RetType>(handler.handle(msg));

        }

        static constexpr std::size_t Count = std::tuple_size<TAllMessages>::value;
        static constexpr auto HelperType = DispatchMsgHelperIntType<Count>::value;
        return 
            DispatchMsgStrongStaticBinSearchHelper<HelperType>::template
                dispatch<TAllMessages, 0, Count>(id, msg, handler);

    }

    template <typename TAllMessages, typename TMsg, typename THandler, typename... TParams>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, TMsg& msg, THandler& handler, WeakTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return dispatchInternal<TAllMessages>(id, 0U, msg, handler, WeakTag<>());
    }

    template <typename TAllMessages, typename TMsg, typename THandler, typename... TParams>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, std::size_t offset, TMsg& msg, THandler& handler, WeakTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_assert(allMessagesAreWeakSorted<TAllMessages>(),
                "The message types in the provided tuple must be sorted by their IDs");

        static constexpr std::size_t Count = std::tuple_size<TAllMessages>::value;
        static constexpr auto HelperType = DispatchMsgHelperIntType<Count>::value;
        return 
            DispatchMsgWeakStaticBinSearchHelper<HelperType>::template
                dispatch<TAllMessages, 0, Count>(id, offset, msg, handler);

    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchTypeInternal(TId&& id, THandler& handler, EmptyTag<TParams...>) 
    {
        return dispatchTypeInternal<TAllMessages>(id, 0U, handler, EmptyTag<>());
    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchTypeInternal(TId&& id, std::size_t offset, THandler& handler, EmptyTag<TParams...>) 
    {
        static_cast<void>(id);
        static_cast<void>(offset);
        static_cast<void>(handler);
        return false;
    }    

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchTypeInternal(TId&& id, THandler& handler, StrongTag<TParams...>)
    {
        using FirstMsgType = 
            typename comms::util::TupleElement<TAllMessages>::template Type<0>;
        static_assert(comms::isMessageBase<FirstMsgType>(), 
            "The type in the tuple are expected to be proper messages");
        static_assert(FirstMsgType::hasMsgIdType(), "The messages must define their ID type");
        using MsgIdParamType = typename FirstMsgType::MsgIdParamType;

        static constexpr std::size_t Count = std::tuple_size<TAllMessages>::value;
        static constexpr auto HelperType = DispatchMsgHelperIntType<Count>::value;
        return 
            DispatchMsgStrongStaticBinSearchHelper<HelperType>::template
                dispatchType<TAllMessages, 0, Count>(static_cast<MsgIdParamType>(id), handler);
    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchTypeInternal(TId&& id, std::size_t offset, THandler& handler, StrongTag<TParams...>)
    {
        if (offset != 0U) {
            return false;

        }
        return dispatchTypeInternal<TAllMessages>(std::forward<TId>(id), handler, StrongTag<>());
    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchTypeInternal(TId&& id, THandler& handler, WeakTag<TParams...>) 
    {
        return dispatchTypeInternal<TAllMessages>(std::forward<TId>(id), 0U, handler, WeakTag<>());
    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchTypeInternal(TId&& id, std::size_t offset, THandler& handler, WeakTag<TParams...>) 
    {
        static_assert(allMessagesAreWeakSorted<TAllMessages>(),
                "The message types in the provided tuple must be sorted by their IDs");

        using FirstMsgType = 
            typename comms::util::TupleElement<TAllMessages>::template Type<0>;
        static_assert(comms::isMessageBase<FirstMsgType>(), 
            "The type in the tuple are expected to be proper messages");
        static_assert(FirstMsgType::hasMsgIdType(), "The messages must define their ID type");
        using MsgIdParamType = typename FirstMsgType::MsgIdParamType;        
        static constexpr std::size_t Count = std::tuple_size<TAllMessages>::value;
        static constexpr auto HelperType = DispatchMsgHelperIntType<Count>::value;
        return 
            DispatchMsgWeakStaticBinSearchHelper<HelperType>::template
                dispatchType<TAllMessages, 0, Count>(static_cast<MsgIdParamType>(id), offset, handler);

    }    

    template <typename TAllMessages, typename TId, typename... TParams>
    static std::size_t dispatchTypeCountInternal(TId&& id, EmptyTag<TParams...>) 
    {
        static_cast<void>(id);
        return 0U;
    }

    template <typename TAllMessages, typename TId, typename... TParams>
    static std::size_t dispatchTypeCountInternal(TId&& id, StrongTag<TParams...>)
    {
        using FirstMsgType = 
            typename comms::util::TupleElement<TAllMessages>::template Type<0>;
        static_assert(comms::isMessageBase<FirstMsgType>(), 
            "The type in the tuple are expected to be proper messages");
        static_assert(FirstMsgType::hasMsgIdType(), "The messages must define their ID type");
        using MsgIdParamType = typename FirstMsgType::MsgIdParamType;

        static constexpr std::size_t Count = std::tuple_size<TAllMessages>::value;
        static constexpr auto HelperType = DispatchMsgHelperIntType<Count>::value;
        return 
            DispatchMsgStrongStaticBinSearchHelper<HelperType>::template
                dispatchTypeCount<TAllMessages, 0, Count>(static_cast<MsgIdParamType>(id));
    }

    template <typename TAllMessages, typename TId, typename... TParams>
    static std::size_t dispatchTypeCountInternal(TId&& id, WeakTag<TParams...>) 
    {
        static_assert(allMessagesAreWeakSorted<TAllMessages>(),
                "The message types in the provided tuple must be sorted by their IDs");

        using FirstMsgType = 
            typename comms::util::TupleElement<TAllMessages>::template Type<0>;
        static_assert(comms::isMessageBase<FirstMsgType>(), 
            "The type in the tuple are expected to be proper messages");
        static_assert(FirstMsgType::hasMsgIdType(), "The messages must define their ID type");
        using MsgIdParamType = typename FirstMsgType::MsgIdParamType; 
        static constexpr std::size_t Count = std::tuple_size<TAllMessages>::value;
        static constexpr auto HelperType = DispatchMsgHelperIntType<Count>::value;               
        return 
            DispatchMsgWeakStaticBinSearchHelper<HelperType>::template
                dispatchTypeCount<TAllMessages, 0, Count>(static_cast<MsgIdParamType>(id));

    }    
};

} // namespace details

} // namespace comms
