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
class DispatchMsgStrongLinearSwitchHelper // <DispatchMsgTypeEnum::Multiple>  
{
    template <typename TAllMessages, std::size_t TCount>
    using FromElem =
        typename std::tuple_element<
            std::tuple_size<TAllMessages>::value - TCount,
            TAllMessages
        >::type;

    // static_assert(messageHasStaticNumId<FromElem>(), "Message must define static ID");

public:
    template <
        typename TAllMessages, 
        std::size_t TCount,
        typename TMsg,
        typename THandler>
    static auto dispatch(typename TMsg::MsgIdParamType id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        using Elem = FromElem<TAllMessages, TCount>;

        static constexpr typename TMsg::MsgIdParamType fromId = Elem::doGetId();
        switch(id) {
            case fromId: {
                auto& castedMsg = static_cast<Elem&>(msg);
                return static_cast<RetType>(handler.handle(castedMsg));
                break;
            }
            default:
                static constexpr std::size_t NextCount = TCount - 1U;
                static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
                return 
                    DispatchMsgStrongLinearSwitchHelper<HelperType>::template
                        dispatch<TAllMessages, NextCount>(id, msg, handler);
                
        };
        // dead code (just in case), should not reach here
        return static_cast<RetType>(handler.handle(msg));
    }

    template <
        typename TAllMessages, 
        std::size_t TCount,
        typename TId, 
        typename THandler>
    static bool dispatchType(TId&& id, THandler& handler) 
    {
        using Elem = FromElem<TAllMessages, TCount>;
        static constexpr typename Elem::MsgIdParamType fromId = Elem::doGetId();
        switch(id) {
            case fromId: {
                handler.template handle<Elem>();
                return true;
                break;
            }
            default:
                static constexpr std::size_t NextCount = TCount - 1U;
                static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;            
                return 
                    DispatchMsgStrongLinearSwitchHelper<HelperType>::template
                        dispatchType<TAllMessages, NextCount>(id, handler);
                
        };
        // dead code (just in case), should not reach here
        return false;
    }
  
};

template <>
class DispatchMsgStrongLinearSwitchHelper<DispatchMsgTypeEnum::Single>
{
    // static_assert(1 <= std::tuple_size<TAllMessages>::value, 
    //     "Invalid template params");

    template <typename TAllMessages>
    using FromElem =
        typename std::tuple_element<
            std::tuple_size<TAllMessages>::value - 1U,
            TAllMessages
        >::type;    

    // static_assert(messageHasStaticNumId<Elem>(), "Message must define static ID");

public:
    template <
        typename TAllMessages, 
        std::size_t TCount,    
        typename TMsg,
        typename THandler>
    static auto dispatch(typename TMsg::MsgIdParamType id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_assert(TCount == 1U, "Invalid invocation");

        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        using Elem = FromElem<TAllMessages>;
        typename TMsg::MsgIdParamType elemId = Elem::doGetId();
        if (id != elemId) {
            return static_cast<RetType>(handler.handle(msg));
        }

        auto& castedMsg = static_cast<Elem&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }

    template <
        typename TAllMessages, 
        std::size_t TCount,    
        typename TId,
        typename THandler>
    static bool dispatchType(TId&& id, THandler& handler) 
    {
        static_assert(TCount == 1U, "Invalid invocation");
        using Elem = FromElem<TAllMessages>;
        typename Elem::MsgIdParamType elemId = Elem::doGetId();
        if (id != elemId) {
            return false;
        }

        handler.template handle<Elem>();
        return true;
    }
};

template <>
class DispatchMsgStrongLinearSwitchHelper<DispatchMsgTypeEnum::None>
{
public:
    template <
        typename TAllMessages, 
        std::size_t TCount,     
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

    template <typename TId, typename THandler>
    static bool dispatchType(TId&& id, THandler& handler) 
    {
        static_cast<void>(id);
        static_cast<void>(handler);
        return false;
    }
};

template <bool THasElems>
class DispatchMsgLinearSwitchWeakCountFinder // <true>
{
    template <typename TAllMessages, std::size_t TIdx>
    using OrigMsgType = typename std::tuple_element<TIdx, TAllMessages>::type;

    template <typename TAllMessages, std::size_t TIdx, std::size_t TCount>
    using CurrMsgType = 
        typename std::tuple_element<
            std::tuple_size<TAllMessages>::value - TCount, 
            TAllMessages
        >::type;    

public:
    template <typename TAllMessages, std::size_t TOrigIdx, std::size_t TRem>
    using Type = 
        typename comms::util::Conditional<
            OrigMsgType<TAllMessages, TOrigIdx>::doGetId() == CurrMsgType<TAllMessages, TOrigIdx, TRem>::doGetId()
        >::template Type<
            std::integral_constant<
                std::size_t,
                1U + DispatchMsgLinearSwitchWeakCountFinder<(1U < TRem)>::template Type<TAllMessages, TOrigIdx, TRem - 1>::value
            >,
            std::integral_constant<std::size_t, 0U>
        >;
};

template <>
class DispatchMsgLinearSwitchWeakCountFinder<false>
{
public:
    template <typename TAllMessages, std::size_t TOrigIdx, std::size_t TRem>
    using Type = std::integral_constant<std::size_t, 0U>;
};

template <DispatchMsgTypeEnum TType>
class DispatchMsgWeakLinearSwitchHelper // <DispatchMsgTypeEnum::Multiple>  
{
    // static_assert(TFrom + TCount <= std::tuple_size<TAllMessages>::value, 
    //     "Invalid template params");
    // static_assert(2 <= TCount, "Invalid invocation");

    template <typename TAllMessages, std::size_t TFrom>
    using FromElem = typename std::tuple_element<TFrom, TAllMessages>::type;

    // static_assert(messageHasStaticNumId<FromElem>(), "Message must define static ID");

    template <typename TAllMessages, std::size_t TFrom, std::size_t TCount>
    using SameIdsCount = 
        typename DispatchMsgLinearSwitchWeakCountFinder<
            0 < TCount
        >::template Type<
            TAllMessages, 
            TFrom, 
            TCount
        >;
    
    // static_assert(SameIdsCount <= TCount, "Invalid template params");
    // static_assert(0U < SameIdsCount, "Invalid template params");

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

        using Elem = FromElem<TAllMessages, TFrom>;
        static constexpr typename TMsg::MsgIdParamType fromId = Elem::doGetId();
        switch(id) {
            case fromId:
            {
                static constexpr std::size_t NextCount = SameIdsCount<TAllMessages, TFrom, TCount>::value;
                static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
                return 
                    DispatchMsgWeakLinearSwitchHelper<HelperType>::template
                        dispatchOffset<TAllMessages, TFrom, NextCount>(offset, msg, handler);
            }
            default:
            {
                static constexpr std::size_t NextFrom = TFrom + SameIdsCount<TAllMessages, TFrom, TCount>::value;
                static constexpr std::size_t NextCount = TCount - NextFrom;
                static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;

                return 
                    DispatchMsgWeakLinearSwitchHelper<HelperType>::template
                        dispatch<TAllMessages, NextFrom, NextCount>(id, offset, msg, handler);
            }
        };
        // dead code (just in case), should not reach here
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
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        using Elem = FromElem<TAllMessages, TFrom>;
        switch(offset) {
            case 0:
            {
                auto& castedMsg = static_cast<Elem&>(msg);
                return static_cast<RetType>(handler.handle(castedMsg));
            }
            default:
                static constexpr std::size_t NextCount = TCount - 1U;
                static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
                return 
                    DispatchMsgWeakLinearSwitchHelper<HelperType>::template
                        dispatchOffset<TAllMessages, TFrom + 1, NextCount>(offset - 1, msg, handler);
        };

        // dead code (just in case), should not reach here
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
        using Elem = FromElem<TAllMessages, TFrom>;
        static constexpr typename Elem::MsgIdParamType fromId = Elem::doGetId();
        switch(id) {
            case fromId:
            {
                static constexpr std::size_t NextCount = SameIdsCount<TAllMessages, TFrom, TCount>::value;
                static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;
                return 
                    DispatchMsgWeakLinearSwitchHelper<HelperType>::template
                        dispatchTypeOffset<TAllMessages, TFrom, NextCount>(offset, handler);
            }
            default:
            {
                static constexpr std::size_t NextFrom = TFrom + SameIdsCount<TAllMessages, TFrom, TCount>::value;
                static constexpr std::size_t NextCount = TCount - NextFrom;
                static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;            
                return 
                    DispatchMsgWeakLinearSwitchHelper<HelperType>::template
                        dispatchType<TAllMessages, NextFrom, NextCount>(id, offset, handler);
            }
        };
        // dead code (just in case), should not reach here
        return false;
    }

    template <
        typename TAllMessages, 
        std::size_t TFrom, 
        std::size_t TCount,    
        typename THandler>
    static bool dispatchTypeOffset(std::size_t offset, THandler& handler)
    {
        switch(offset) {
            case 0:
                handler.template handle<FromElem>();
                return true;
            default:
                static constexpr std::size_t NextCount = TCount - 1U;
                static constexpr auto HelperType = DispatchMsgHelperIntType<NextCount>::value;            
                return 
                    DispatchMsgWeakLinearSwitchHelper<HelperType>::template
                        dispatchTypeOffset<TAllMessages, TFrom + 1, NextCount>(offset - 1, handler);
        };

        // dead code (just in case), should not reach here
        return false;
    }
};

template <>
class DispatchMsgWeakLinearSwitchHelper<DispatchMsgTypeEnum::Single> 
{
    // static_assert(TFrom + 1 <= std::tuple_size<TAllMessages>::value, 
    //     "Invalid template params");
    
    template <typename TAllMessages, std::size_t TFrom>
    using FromElem = typename std::tuple_element<TFrom, TAllMessages>::type;
    // static_assert(messageHasStaticNumId<Elem>(), "Message must define static ID");

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

        using Elem = FromElem<TAllMessages, TFrom>;
        typename TMsg::MsgIdParamType elemId = Elem::doGetId();
        if (id != elemId) {
            return static_cast<RetType>(handler.handle(msg));
        }

        auto& castedMsg = static_cast<Elem&>(msg);
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

        using Elem = FromElem<TAllMessages, TFrom>;
        auto& castedMsg = static_cast<Elem&>(msg);
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

        using Elem = FromElem<TAllMessages, TFrom>;
        typename Elem::MsgIdParamType elemId = Elem::doGetId();
        if (id != elemId) {
            return false;
        }

        handler.template handle<Elem>();
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

        using Elem = FromElem<TAllMessages, TFrom>;
        handler.template handle<Elem>();
        return true;
    }
};

template <>
class DispatchMsgWeakLinearSwitchHelper<DispatchMsgTypeEnum::None> 
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
};

template <typename...>
class DispatchMsgLinearSwitchHelper    
{
    template <typename... TParams>
    using EmptyTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using StrongTag = comms::details::tag::Tag2<>;

    template <typename... TParams>
    using WeakTag = comms::details::tag::Tag3<>;    

    template <typename TAllMessages, typename...>
    using StrongWeakTag = 
        typename comms::util::LazyShallowConditional<
            allMessagesAreStrongSorted<TAllMessages>()
        >::template Type<
            StrongTag,
            WeakTag
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
        return dispatchInternal<TAllMessages>(msg.getId(), msg, handler, AdjustedTag<TAllMessages, MsgType>());
    }

    template <typename TAllMessages, typename TId, typename TMsg, typename THandler>
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

    template <typename TAllMessages, typename TId, typename TMsg, typename THandler>
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

private:
    template <typename TAllMessages, typename TMsg, typename THandler, typename... TParams>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, TMsg& msg, THandler& handler, EmptyTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(id);
        return handler.handle(msg);
    }

    template <typename TAllMessages, typename TMsg, typename THandler, typename... TParams>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, std::size_t offset, TMsg& msg, THandler& handler, EmptyTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(id);
        static_cast<void>(offset);
        return handler.handle(msg);
    }

    template <typename TAllMessages, typename TMsg, typename THandler, typename... TParams>
    static auto dispatchInternal(typename TMsg::MsgIdParamType id, TMsg& msg, THandler& handler, StrongTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static constexpr std::size_t Count = std::tuple_size<TAllMessages>::value;
        static constexpr auto HelperType = DispatchMsgHelperIntType<Count>::value;
        return 
            DispatchMsgStrongLinearSwitchHelper<HelperType>::template
                dispatch<TAllMessages, Count>(id, msg, handler);
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
            DispatchMsgStrongLinearSwitchHelper<HelperType>::template
                dispatch<TAllMessages, Count>(id, msg, handler);

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
        static constexpr std::size_t Count = std::tuple_size<TAllMessages>::value;
        static constexpr auto HelperType = DispatchMsgHelperIntType<Count>::value;          
        return 
            DispatchMsgWeakLinearSwitchHelper<HelperType>::template
                dispatch<TAllMessages, 0, Count>(id, offset, msg, handler);

    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchTypeInternal(TId&& id, THandler& handler, EmptyTag<TParams...>) 
    {
        static_cast<void>(id); 
        static_cast<void>(handler);
        return false;
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
        using FirstMsgType = typename std::tuple_element<0, TAllMessages>::type;
        static_assert(comms::isMessageBase<FirstMsgType>(), 
            "The type in the tuple are expected to be proper messages");
        static_assert(FirstMsgType::hasMsgIdType(), "The messages must define their ID type");
        using MsgIdParamType = typename FirstMsgType::MsgIdParamType;        

        static constexpr std::size_t Count = std::tuple_size<TAllMessages>::value;
        static constexpr auto HelperType = DispatchMsgHelperIntType<Count>::value;  
        return 
            DispatchMsgStrongLinearSwitchHelper<HelperType>::template
                dispatchType<TAllMessages, Count>(static_cast<MsgIdParamType>(id), handler);
    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchTypeInternal(TId&& id, std::size_t offset, THandler& handler, StrongTag<TParams...>) 
    {
        if (offset != 0U) {
            return false;
        }

        return dispatchTypeInternal<TAllMessages>(std::forward<TId>(id), handler, StrongTag<TParams...>());
    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchTypeInternal(TId&& id, THandler& handler, WeakTag<TParams...>) 
    {
        return dispatchTypeInternal<TAllMessages>(std::forward<TId>(id), 0U, handler, WeakTag<>());
    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchTypeInternal(TId&& id, std::size_t offset, THandler& handler, WeakTag<TParams...>) 
    {
        using FirstMsgType = typename std::tuple_element<0, TAllMessages>::type;
        static_assert(comms::isMessageBase<FirstMsgType>(), 
            "The type in the tuple are expected to be proper messages");
        static_assert(FirstMsgType::hasMsgIdType(), "The messages must define their ID type");
        using MsgIdParamType = typename FirstMsgType::MsgIdParamType;        

        static constexpr std::size_t Count = std::tuple_size<TAllMessages>::value;
        static constexpr auto HelperType = DispatchMsgHelperIntType<Count>::value;  
        return 
            DispatchMsgWeakLinearSwitchHelper<HelperType>::template
                dispatchType<TAllMessages, 0, Count>(static_cast<MsgIdParamType>(id), offset, handler);
    }

};

} // namespace details

} // namespace comms
