//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <array>
#include <algorithm>
#include <limits>

#include "comms/Assert.h"
#include "comms/Message.h"
#include "comms/MessageBase.h"
#include "comms/util/Tuple.h"
#include "comms/util/type_traits.h"
#include "comms/details/message_check.h"
#include "comms/CompileControl.h"
#include "comms/details/DispatchMsgIdRetrieveHelper.h"
#include "comms/details/tag.h"

namespace comms
{

namespace details
{

template <typename TMsgBase, typename THandler>
class PolymorphicDirectDispatchMethod
{
public:
    PolymorphicDirectDispatchMethod(const PolymorphicDirectDispatchMethod&) = delete;
    PolymorphicDirectDispatchMethod& operator=(const PolymorphicDirectDispatchMethod&) = delete;

    virtual auto dispatch(TMsgBase& msg, THandler& handler) const ->
        MessageInterfaceDispatchRetType<THandler>
    {
        return dispatchImpl(msg, handler);
    }

protected:
    PolymorphicDirectDispatchMethod() = default;
    ~PolymorphicDirectDispatchMethod() = default;

    virtual auto dispatchImpl(TMsgBase& msg, THandler& handler) const ->
        MessageInterfaceDispatchRetType<THandler> = 0;
};

template <typename TMsgBase, typename THandler, typename TMessage>
class PolymorphicDirectDispatchMethodImpl : public 
                PolymorphicDirectDispatchMethod<TMsgBase, THandler>
{
public:
    PolymorphicDirectDispatchMethodImpl() = default;
    PolymorphicDirectDispatchMethodImpl(const PolymorphicDirectDispatchMethodImpl&) = delete;
    PolymorphicDirectDispatchMethodImpl& operator=(const PolymorphicDirectDispatchMethodImpl&) = delete;

protected:
    virtual auto dispatchImpl(TMsgBase& msg, THandler& handler) const ->
        MessageInterfaceDispatchRetType<THandler> 
#ifndef CC_COMPILER_GCC47        
            override final 
#endif        
    {
        static_assert(std::is_base_of<TMsgBase, TMessage>::value, "TMessage must extend TMsgBase");
        auto& castedMsg = static_cast<TMessage&>(msg);
        return handler.handle(castedMsg);
    }
};

template <typename TMsgBase, typename THandler>
class PolymorphicBinSearchDispatchMethod
{
public:
    PolymorphicBinSearchDispatchMethod(const PolymorphicBinSearchDispatchMethod&) = delete;
    PolymorphicBinSearchDispatchMethod& operator=(const PolymorphicBinSearchDispatchMethod&) = delete;

    static_assert(TMsgBase::hasMsgIdType(), "Message interface class must define its id type");
    using MsgIdParamType = typename TMsgBase::MsgIdParamType;
    using MsgIdType = typename TMsgBase::MsgIdType;

    virtual MsgIdParamType getId() const
    {
        return getIdImpl();
    }

    virtual auto dispatch(TMsgBase& msg, THandler& handler) const ->
        MessageInterfaceDispatchRetType<THandler>
    {
        return dispatchImpl(msg, handler);
    }

protected:
    PolymorphicBinSearchDispatchMethod() = default;
    ~PolymorphicBinSearchDispatchMethod() = default;

    virtual MsgIdParamType getIdImpl() const = 0;
    virtual auto dispatchImpl(TMsgBase& msg, THandler& handler) const ->
        MessageInterfaceDispatchRetType<THandler> = 0;
};

template <typename TMsgBase, typename THandler, typename TMessage>
class PolymorphicBinSearchDispatchMethodImpl : public 
                PolymorphicBinSearchDispatchMethod<TMsgBase, THandler>
{
    using Base = PolymorphicBinSearchDispatchMethod<TMsgBase, THandler>;
public:
    static_assert(TMsgBase::hasMsgIdType(), "Message interface class must define its id type");
    using MsgIdParamType = typename Base::MsgIdParamType;
    using MsgIdType = typename Base::MsgIdType;

    PolymorphicBinSearchDispatchMethodImpl() = default;
    PolymorphicBinSearchDispatchMethodImpl(const PolymorphicBinSearchDispatchMethodImpl&) = delete;
    PolymorphicBinSearchDispatchMethodImpl& operator=(const PolymorphicBinSearchDispatchMethodImpl&) = delete;


    static MsgIdParamType doGetId()
    {
        return dispatchMsgGetMsgId<TMessage>();
    }

protected:
    
    virtual MsgIdParamType getIdImpl() const 
#ifndef CC_COMPILER_GCC47        
        override final 
#endif  
    {
        return doGetId();
    }

    virtual auto dispatchImpl(TMsgBase& msg, THandler& handler) const ->
        MessageInterfaceDispatchRetType<THandler> 
#ifndef CC_COMPILER_GCC47        
            override final 
#endif  
    {
        static_assert(std::is_base_of<TMsgBase, TMessage>::value, "TMessage must extend TMsgBase");
        auto& castedMsg = static_cast<TMessage&>(msg);
        return handler.handle(castedMsg);
    }
};

template <typename TMsgBase, typename THandler, std::size_t TSize>
using PolymorphicDirectDispatchMsgRegistry = 
    std::array<const PolymorphicDirectDispatchMethod<TMsgBase, THandler>*, TSize>;

template <typename TMsgBase, typename THandler, std::size_t TSize>
using PolymorphicBinSearchDispatchMsgRegistry = 
    std::array<const PolymorphicBinSearchDispatchMethod<TMsgBase, THandler>*, TSize>;


template <typename TMsgBase, typename THandler>
class PolymorphicDirectDispatchRegistryFiller
{
public:
    using DispatchMethod = PolymorphicDirectDispatchMethod<TMsgBase, THandler>;
    PolymorphicDirectDispatchRegistryFiller(const DispatchMethod** registry)
      : m_registry(registry)
    {
    }

    template <typename TMessage>
    void operator()()
    {
        static_assert(comms::isMessageBase<TMessage>(), "Must be actual message");
        static_assert(messageHasStaticNumId<TMessage>(), "Message must define static ID");
        static const PolymorphicDirectDispatchMethodImpl<TMsgBase, THandler, TMessage> Method{};
        m_registry[static_cast<std::size_t>(TMessage::doGetId())] = &Method;
    }
private:
    const DispatchMethod** m_registry;
};

template <typename TMsgBase, typename THandler, std::size_t TSize>
PolymorphicDirectDispatchRegistryFiller<TMsgBase, THandler>
polymorphicDirectDispatchMakeRegistryFiller(
    PolymorphicDirectDispatchMsgRegistry<TMsgBase, THandler, TSize>& registry)
{
    return PolymorphicDirectDispatchRegistryFiller<TMsgBase, THandler>(&registry[0]);
}

template <typename TRegistry, typename TAllMessages>
class PolymorphicDirectDispatchRegistryInitializer
{
public:
    PolymorphicDirectDispatchRegistryInitializer() = delete;
    PolymorphicDirectDispatchRegistryInitializer(const PolymorphicDirectDispatchRegistryInitializer&) = delete;
    explicit PolymorphicDirectDispatchRegistryInitializer(TRegistry& registry)
    {
        std::fill(registry.begin(), registry.end(), nullptr);
        comms::util::tupleForEachType<TAllMessages>(polymorphicDirectDispatchMakeRegistryFiller(registry));
    }
};

template <typename TMsgBase, typename THandler>
class PolymorphicBinSearchDispatchRegistryFiller
{
public:
    using DispatchMethod = PolymorphicBinSearchDispatchMethod<TMsgBase, THandler>;
    PolymorphicBinSearchDispatchRegistryFiller(const DispatchMethod** registry)
      : m_registry(registry)
    {
    }

    template <typename TMessage>
    void operator()()
    {
        static_assert(comms::isMessageBase<TMessage>(), "Must be actual message");
        static_assert(messageHasStaticNumId<TMessage>(), "Message must define static ID");
        static const PolymorphicBinSearchDispatchMethodImpl<TMsgBase, THandler, TMessage> Method{};
        m_registry[m_idx] = &Method;
        ++m_idx;
    }
    
private:
    const DispatchMethod** m_registry;
    std::size_t m_idx = 0U;
};

template <typename TMsgBase, typename THandler, std::size_t TSize>
PolymorphicBinSearchDispatchRegistryFiller<TMsgBase, THandler>
polymorphicBinSearchDispatchMakeRegistryFiller(
    PolymorphicBinSearchDispatchMsgRegistry<TMsgBase, THandler, TSize>& registry)
{
    return PolymorphicBinSearchDispatchRegistryFiller<TMsgBase, THandler>(&registry[0]);
}

template <typename TRegistry, typename TAllMessages>
class PolymorphicBinSearchDispatchRegistryInitializer
{
public:
    explicit PolymorphicBinSearchDispatchRegistryInitializer(TRegistry& registry)
    {
        std::fill(registry.begin(), registry.end(), nullptr);
        comms::util::tupleForEachType<TAllMessages>(polymorphicBinSearchDispatchMakeRegistryFiller(registry));
    }
};

template <typename TAllMessages, std::size_t TMaxSize>
class PolymorphicDirectDispatchRegSizeDetect
{
    using MsgType = 
        typename comms::util::TupleElement<TAllMessages>::template Type<TMaxSize - 1>;
    static_assert(comms::isMessageBase<MsgType>(), "Must be actual message");
    static_assert(messageHasStaticNumId<MsgType>(), "Message must define static numeric ID");
    static_assert(MsgType::hasMsgIdType(), "Message interface class must define its id type");
    static const typename MsgType::MsgIdParamType MsgId = MsgType::doGetId();
public:
    static const std::size_t Value = static_cast<std::size_t>(MsgId) + 1U;
};

template <typename TAllMessages>
class PolymorphicDirectDispatchRegSizeDetect<TAllMessages, 0U>
{
public:
    static const std::size_t Value = 0;
};

template <typename TAllMessages, typename TMsgBase, typename THandler>
class DispatchMsgDirectPolymorphicHelper    
{
public:
    static_assert(TMsgBase::hasMsgIdType(), "Message interface class must define its id type");
    using MsgIdParamType = typename TMsgBase::MsgIdParamType;
    static auto dispatch(MsgIdParamType id, TMsgBase& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        // Access initializer object to ensure it hasn't been erased by the optimizer
        static_cast<void>(s_initializer);

        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        auto regIdx = static_cast<std::size_t>(id);
        if ((s_registry.size() <= regIdx) ||
            (s_registry[regIdx] == nullptr)) {
            return static_cast<RetType>(handler.handle(msg));
        }

        return static_cast<RetType>(s_registry[regIdx]->dispatch(msg, handler));
    }

private:
    static const std::size_t RegistrySize = 
        PolymorphicDirectDispatchRegSizeDetect<TAllMessages, std::tuple_size<TAllMessages>::value>::Value;
    using Registry = PolymorphicDirectDispatchMsgRegistry<TMsgBase, THandler, RegistrySize>;
    using Initializer = PolymorphicDirectDispatchRegistryInitializer<Registry, TAllMessages>;

    static Registry s_registry;
    static const Initializer s_initializer;
};

template <typename TAllMessages, typename TMsgBase, typename THandler>
typename DispatchMsgDirectPolymorphicHelper<TAllMessages, TMsgBase, THandler>::Registry 
DispatchMsgDirectPolymorphicHelper<TAllMessages, TMsgBase, THandler>::s_registry;

template <typename TAllMessages, typename TMsgBase, typename THandler>
const typename DispatchMsgDirectPolymorphicHelper<TAllMessages, TMsgBase, THandler>::Initializer 
DispatchMsgDirectPolymorphicHelper<TAllMessages, TMsgBase, THandler>::s_initializer(s_registry);

template <typename TAllMessages, typename TMsgBase, typename THandler>
class DispatchMsgBinSearchPolymorphicHelperBase
{
protected:
    static const std::size_t RegistrySize = std::tuple_size<TAllMessages>::value;
    using Registry = PolymorphicBinSearchDispatchMsgRegistry<TMsgBase, THandler, RegistrySize>;
    using Initializer = PolymorphicBinSearchDispatchRegistryInitializer<Registry, TAllMessages>;

    static Registry s_registry;
    static Initializer s_initializer;
};

template <typename TAllMessages, typename TMsgBase, typename THandler>
typename DispatchMsgBinSearchPolymorphicHelperBase<TAllMessages, TMsgBase, THandler>::Registry 
DispatchMsgBinSearchPolymorphicHelperBase<TAllMessages, TMsgBase, THandler>::s_registry;

template <typename TAllMessages, typename TMsgBase, typename THandler>
typename DispatchMsgBinSearchPolymorphicHelperBase<TAllMessages, TMsgBase, THandler>::Initializer 
DispatchMsgBinSearchPolymorphicHelperBase<TAllMessages, TMsgBase, THandler>::s_initializer(s_registry);

template <typename TAllMessages, typename TMsgBase, typename THandler>
class DispatchMsgBinSearchStrongPolymorphicHelper : public
    DispatchMsgBinSearchPolymorphicHelperBase<TAllMessages, TMsgBase, THandler>
{
    using Base = DispatchMsgBinSearchPolymorphicHelperBase<TAllMessages, TMsgBase, THandler>;
    using Registry = typename Base::Registry;
public:
    static_assert(TMsgBase::hasMsgIdType(), "Message interface class must define its id type");
    using MsgIdParamType = typename TMsgBase::MsgIdParamType;
    static auto dispatch(MsgIdParamType id, TMsgBase& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        // Access initializer object to ensure it hasn't been erased by the optimizer
        static_cast<void>(Base::s_initializer);

        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        auto iter = 
            std::lower_bound(
                Base::s_registry.begin(), Base::s_registry.end(), id, 
                [](typename Registry::value_type method, MsgIdParamType idParam) -> bool
                {
                    COMMS_ASSERT(method != nullptr);
                    return method->getId() < idParam;
                });

        if ((iter == Base::s_registry.end()) || ((*iter)->getId() != id)) {
            return static_cast<RetType>(handler.handle(msg));    
        }

        return static_cast<RetType>((*iter)->dispatch(msg, handler));
    }
};

template <typename TAllMessages, typename TMsgBase, typename THandler>
class DispatchMsgBinSearchWeakPolymorphicHelper : public
    DispatchMsgBinSearchPolymorphicHelperBase<TAllMessages, TMsgBase, THandler>
{
    using Base = DispatchMsgBinSearchPolymorphicHelperBase<TAllMessages, TMsgBase, THandler>;
    using Registry = typename Base::Registry;
public:
    template <typename TId>
    static auto dispatch(TId&& id, std::size_t offset, TMsgBase& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        // Access initializer object to ensure it hasn't been erased by the optimizer
        static_cast<void>(Base::s_initializer);

        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        using IdType = typename std::decay<decltype(id)>::type;

        auto lowerIter = 
            std::lower_bound(
                Base::s_registry.begin(), Base::s_registry.end(), id, 
                [](typename Registry::value_type method, IdType idParam) -> bool
                {
                    COMMS_ASSERT(method != nullptr);
                    return static_cast<IdType>(method->getId()) < idParam;
                });

        if ((lowerIter == Base::s_registry.end()) || 
            (static_cast<IdType>((*lowerIter)->getId()) != id)) {
            return static_cast<RetType>(handler.handle(msg));    
        }

        auto upperIter = 
            std::upper_bound(
                lowerIter, Base::s_registry.end(), id,
                [](IdType idParam, typename Registry::value_type method)
                {
                    return idParam < static_cast<IdType>(method->getId());
                }
            );

        COMMS_ASSERT(lowerIter < upperIter);

        auto dist = static_cast<std::size_t>(upperIter - lowerIter);
        if (dist <= offset) {
            return static_cast<RetType>(handler.handle(msg));
        }

        auto actualIter = lowerIter + offset;
        return static_cast<RetType>((*actualIter)->dispatch(msg, handler));
    }
};

template <typename TElem, bool TIsMessage>
struct DispatchMsgPolymorphicLastIdRetriever;

template <typename TElem>
struct DispatchMsgPolymorphicLastIdRetriever<TElem, true>
{
    static_assert(messageHasStaticNumId<TElem>(), "TElem must define static numeric id");
    static const std::size_t Value = 
        static_cast<std::size_t>(TElem::doGetId());
};

template <typename TElem>
struct DispatchMsgPolymorphicLastIdRetriever<TElem, false>
{
    static const std::size_t Value = std::numeric_limits<std::size_t>::max();
};

template <typename TAllMessages, std::size_t TCount>
class DispatchMsgPolymorphicIsDirectSuitable
{
    using LastMsg = 
        typename comms::util::TupleElement<TAllMessages>::template Type<
            std::tuple_size<TAllMessages>::value - 1
        >;

    static const std::size_t MaxId = 
        DispatchMsgPolymorphicLastIdRetriever<LastMsg, comms::isMessageBase<LastMsg>()>::Value;

    static const std::size_t MaxAllowedId = (TCount * 11) / 10;

public:
    static const bool Value = 
        (MaxId <= (TCount + 10U)) || (MaxId <= MaxAllowedId);
};

template <typename TAllMessages>
class DispatchMsgPolymorphicIsDirectSuitable<TAllMessages, 0U>
{
public:
    static const bool Value = true;
};

template <typename TAllMessages>
static constexpr bool dispatchMsgPolymorphicIsDirectSuitable()
{
    return 
        allMessagesHaveStaticNumId<TAllMessages>() && 
        DispatchMsgPolymorphicIsDirectSuitable<TAllMessages, std::tuple_size<TAllMessages>::value>::Value;
}

template <typename...>
struct DispatchMsgPolymorphicCompatibleHandlerDetector
{
    template <typename TMsg, typename THandler>
    using Type = std::is_base_of<typename TMsg::Handler, THandler>;
};

template <typename TMsg, typename THandler>
using DispatchMsgPolymorphicCompatibleHandlerDetectBoolType = 
    typename comms::util::LazyDeepConditional<
        TMsg::hasDispatch()
    >::template Type<
        DispatchMsgPolymorphicCompatibleHandlerDetector,
        comms::util::FalseType,
        TMsg, THandler
    >;


template <typename TMsg, typename THandler>
constexpr bool dispatchMsgPolymorphicIsCompatibleHandler()
{
    return DispatchMsgPolymorphicCompatibleHandlerDetectBoolType<TMsg, THandler>::value;
}

template <typename ...>
class DispatchMsgPolymorphicHelper
{
    template <typename... TParams>
    using EmptyTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using DispatchInterfaceTag = comms::details::tag::Tag2<>;
    
    template <typename... TParams>
    using DirectTag = comms::details::tag::Tag3<>;

    template <typename... TParams>
    using StrongBinSearchTag = comms::details::tag::Tag4<>;

    template <typename... TParams>
    using WeakBinSearchTag = comms::details::tag::Tag5<>;

    template <typename... TParams>
    using IdInterfaceTag = comms::details::tag::Tag6<>;

    template <typename... TParams>
    using NoIdInterfaceTag = comms::details::tag::Tag7<>;

    template <typename TAllMessages, typename...>
    using DirectStrongTag = 
        typename comms::util::LazyShallowConditional<
            dispatchMsgPolymorphicIsDirectSuitable<TAllMessages>()
        >::template Type<
            DirectTag,
            StrongBinSearchTag
        >;

    template <typename TAllMessages, typename...>
    using DirectStrongWeakTag = 
        typename comms::util::LazyShallowConditional<
            allMessagesAreStrongSorted<TAllMessages>()
        >::template Type<
            DirectStrongTag,
            WeakBinSearchTag,
            TAllMessages
        >;


    template <typename TAllMessages, typename TMsgBase, typename THandler, typename...>
    using Tag = 
        typename comms::util::LazyShallowConditional<
            dispatchMsgPolymorphicIsCompatibleHandler<TMsgBase, THandler>()
        >::template Type<
            DispatchInterfaceTag,
            DirectStrongWeakTag,
            TAllMessages
        >;

    template <typename TAllMessages, typename TMsgBase, typename THandler, typename...>
    using AdjustedTag =
        typename comms::util::LazyShallowConditional<
            comms::isMessageBase<TMsgBase>()
        >::template Type<
            EmptyTag,
            Tag,
            TAllMessages, TMsgBase, THandler
        >;

        template <typename TMsgBase, typename...>
        using InterfaceDispatchTag =
            typename comms::util::LazyShallowConditional<
                TMsgBase::hasGetId()
            >::template Type<
                IdInterfaceTag,
                NoIdInterfaceTag
            >;

        template <typename TMsgBase, typename THandler, typename...>
        using PolymorphicCheckDispatchTag = 
            typename comms::util::LazyShallowConditional<
                dispatchMsgPolymorphicIsCompatibleHandler<TMsgBase, THandler>()
            >::template Type<
                NoIdInterfaceTag,
                InterfaceDispatchTag,
                TMsgBase
            >;

public:
    template <typename TAllMessages, typename TMsgBase, typename THandler>
    static auto dispatch(TMsgBase& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {

        static_assert(allMessagesAreWeakSorted<TAllMessages>(),
            "Message types must be sorted by their ID");

        static_assert(comms::isMessage<TMsgBase>(), 
            "TMsgBase is expected to be message interface class");

        static_assert(TMsgBase::hasMsgIdType(), "Message interface class must define its id type");

        using MsgType = typename std::decay<decltype(msg)>::type;
        using HandlerType = typename std::decay<decltype(handler)>::type;
        using IdRetrieveTag = 
            typename comms::util::LazyShallowConditional<
                comms::isMessageBase<MsgType>()
            >::template Type<
                EmptyTag,
                PolymorphicCheckDispatchTag,
                MsgType, HandlerType
            >;

        return dispatchInternal<TAllMessages>(msg, handler, IdRetrieveTag());
    }

    template <typename TAllMessages, typename TId, typename TMsgBase, typename THandler>
    static auto dispatch(TId&& id, TMsgBase& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_assert(allMessagesAreWeakSorted<TAllMessages>(),
            "Message types must be sorted by their ID");

        static_assert(comms::isMessage<TMsgBase>(), 
            "TMsgBase is expected to be message interface class");

        static_assert(TMsgBase::hasMsgIdType(), "Message interface class must define its id type");

        using MsgType = typename std::decay<decltype(msg)>::type;
        using HandlerType = typename std::decay<decltype(handler)>::type;
        return 
            dispatchInternal<TAllMessages>(
                static_cast<typename MsgType::MsgIdParamType>(id), 
                msg, 
                handler, 
                AdjustedTag<TAllMessages, MsgType, HandlerType>());
    }

    template <typename TAllMessages, typename TId, typename TMsgBase, typename THandler>
    static auto dispatch(TId&& id, std::size_t offset, TMsgBase& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_assert(allMessagesAreWeakSorted<TAllMessages>(),
            "Message types must be sorted by their ID");
                    
        static_assert(comms::isMessage<TMsgBase>(), 
            "TMsgBase is expected to be message interface class");

        static_assert(TMsgBase::hasMsgIdType(), "Message interface class must define its id type");

        using MsgType = typename std::decay<decltype(msg)>::type;
        using HandlerType = typename std::decay<decltype(handler)>::type;
        return 
            dispatchInternal<TAllMessages>(
                static_cast<typename MsgType::MsgIdParamType>(id), 
                offset, 
                msg, 
                handler, 
                AdjustedTag<TAllMessages, MsgType, HandlerType>());
    }

private:
    template <typename TAllMessages, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TMsgBase& msg, THandler& handler, EmptyTag<TParams...>) ->
        decltype(handler.handle(msg))
    {
        return handler.handle(msg);
    }

    template <typename TAllMessages, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TMsgBase& msg, THandler& handler, IdInterfaceTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        typename TMsgBase::MsgIdParamType id = msg.getId();
        return dispatch<TAllMessages>(id, msg, handler);
    }

    template <typename TAllMessages, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TMsgBase& msg, THandler& handler, NoIdInterfaceTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_assert(TMsgBase::hasDispatch(), "The message must provide polymorphic dispatch");
        static_assert(std::is_base_of<typename TMsgBase::Handler, THandler>::value,
            "Incompatible handlers");

        using RetType = MessageInterfaceDispatchRetType<THandler>;
        return static_cast<RetType>(msg.dispatch(handler));
    }

    template <typename TAllMessages, typename TId, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TId&& id, TMsgBase& msg, THandler& handler, EmptyTag<TParams...>) ->
        decltype(handler.handle(msg))
    {
        static_cast<void>(id);
        return handler.handle(msg);
    }

    template <typename TAllMessages, typename TId, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TId&& id, TMsgBase& msg, THandler& handler, DispatchInterfaceTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(id);
        return dispatchInternal<TAllMessages>(msg, handler, NoIdInterfaceTag<>());
    }

    template <typename TAllMessages, typename TId, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TId&& id, TMsgBase& msg, THandler& handler, DirectTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        using HandlerType = typename std::decay<decltype(handler)>::type;

        return DispatchMsgDirectPolymorphicHelper<TAllMessages, MsgType, HandlerType>::
            dispatch(std::forward<TId>(id), msg, handler);
    }

    template <typename TAllMessages, typename TId, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TId&& id, TMsgBase& msg, THandler& handler, StrongBinSearchTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        using HandlerType = typename std::decay<decltype(handler)>::type;

        return DispatchMsgBinSearchStrongPolymorphicHelper<TAllMessages, MsgType, HandlerType>::
            dispatch(std::forward<TId>(id), msg, handler);
    }

    template <typename TAllMessages, typename TId, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TId&& id, TMsgBase& msg, THandler& handler, WeakBinSearchTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return dispatchInternal<TAllMessages>(std::forward<TId>(id), 0U, msg, handler, WeakBinSearchTag<>());
    }

    template <typename TAllMessages, typename TId, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TId&& id, std::size_t offset, TMsgBase& msg, THandler& handler, DispatchInterfaceTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(id);
        static_cast<void>(offset);
        static_assert(std::is_base_of<typename TMsgBase::Handler, THandler>::value,
            "Incompatible handlers");

        static_assert(allMessagesAreWeakSorted<TAllMessages>(),
                "The message types in the provided tuple must be sorted by their IDs");

        using RetType = MessageInterfaceDispatchRetType<THandler>;
        return static_cast<RetType>(msg.dispatch(handler));
    }

    template <typename TAllMessages, typename TId, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TId&& id, std::size_t offset, TMsgBase& msg, THandler& handler, EmptyTag<TParams...>) ->
        decltype(handler.handle(msg))
    {
        static_cast<void>(id);
        static_cast<void>(offset);
        return handler.handle(msg);
    }

    template <typename TAllMessages, typename TId, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TId&& id, std::size_t offset, TMsgBase& msg, THandler& handler, DirectTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        if (offset != 0U) {
            using RetType = MessageInterfaceDispatchRetType<THandler>;
            return static_cast<RetType>(handler.handle(msg));
        }

        return dispatchInternal<TAllMessages>(std::forward<TId>(id), msg, handler, DirectTag<>());
    }

    template <typename TAllMessages, typename TId, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TId&& id, std::size_t offset, TMsgBase& msg, THandler& handler, StrongBinSearchTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        if (offset != 0U) {
            using RetType = MessageInterfaceDispatchRetType<THandler>;
            return static_cast<RetType>(handler.handle(msg));
        }

        return dispatchInternal<TAllMessages>(std::forward<TId>(id), msg, handler, StrongBinSearchTag<>());
    }

    template <typename TAllMessages, typename TId, typename TMsgBase, typename THandler, typename... TParams>
    static auto dispatchInternal(TId&& id, std::size_t offset, TMsgBase& msg, THandler& handler, WeakBinSearchTag<TParams...>) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        using HandlerType = typename std::decay<decltype(handler)>::type;
        
        return DispatchMsgBinSearchWeakPolymorphicHelper<TAllMessages, MsgType, HandlerType>::
            dispatch(std::forward<TId>(id), offset, msg, handler);
    }
};

// ------------------------------------------------

template <typename THandler>
class PolymorphicTypeDirectDispatchMethod
{
public:
    PolymorphicTypeDirectDispatchMethod(const PolymorphicTypeDirectDispatchMethod&) = delete;
    PolymorphicTypeDirectDispatchMethod& operator=(const PolymorphicTypeDirectDispatchMethod&) = delete;

    virtual void dispatch(THandler& handler) const
    {
        dispatchImpl(handler);
    }

protected:
    PolymorphicTypeDirectDispatchMethod() = default;
    ~PolymorphicTypeDirectDispatchMethod() = default;

    virtual void dispatchImpl(THandler& handler) const = 0;
};

template <typename THandler, typename TMessage>
class PolymorphicTypeDirectDispatchMethodImpl : public 
                PolymorphicTypeDirectDispatchMethod<THandler>
{
public:
    PolymorphicTypeDirectDispatchMethodImpl() = default;
    PolymorphicTypeDirectDispatchMethodImpl(const PolymorphicTypeDirectDispatchMethodImpl&) = delete;
    PolymorphicTypeDirectDispatchMethodImpl& operator=(const PolymorphicTypeDirectDispatchMethodImpl&) = delete;

protected:
    virtual void dispatchImpl(THandler& handler) const 
#ifndef CC_COMPILER_GCC47        
        override final 
#endif  
    {
        return handler.template handle<TMessage>();
    }
};

template <typename TMsgIdType, typename THandler>
class PolymorphicTypeBinSearchDispatchMethod
{
public:
    PolymorphicTypeBinSearchDispatchMethod(const PolymorphicTypeBinSearchDispatchMethod&) = delete;
    PolymorphicTypeBinSearchDispatchMethod& operator=(const PolymorphicTypeBinSearchDispatchMethod&) = delete;

    virtual TMsgIdType getId() const
    {
        return getIdImpl();
    }

    virtual void dispatch(THandler& handler) const
    {
        return dispatchImpl(handler);
    }

protected:
    PolymorphicTypeBinSearchDispatchMethod() = default;
    ~PolymorphicTypeBinSearchDispatchMethod() = default;

    virtual TMsgIdType getIdImpl() const = 0;
    virtual void dispatchImpl(THandler& handler) const = 0;
};

template <typename TMsgIdType, typename THandler, typename TMessage>
class PolymorphicTypeBinSearchDispatchMethodImpl : public 
                PolymorphicTypeBinSearchDispatchMethod<TMsgIdType, THandler>
{
    using Base = PolymorphicTypeBinSearchDispatchMethod<TMsgIdType, THandler>;
public:
    PolymorphicTypeBinSearchDispatchMethodImpl() = default;
    PolymorphicTypeBinSearchDispatchMethodImpl(const PolymorphicTypeBinSearchDispatchMethodImpl&) = delete;
    PolymorphicTypeBinSearchDispatchMethodImpl& operator=(const PolymorphicTypeBinSearchDispatchMethodImpl&) = delete;

    static TMsgIdType doGetId()
    {
        return dispatchMsgGetMsgId<TMessage>();
    }

protected:
    virtual TMsgIdType getIdImpl() const
#ifndef CC_COMPILER_GCC47        
        override final 
#endif      
    {
        return doGetId();
    }

    virtual void dispatchImpl(THandler& handler) const 
#ifndef CC_COMPILER_GCC47        
        override final 
#endif  
    {
        return handler.template handle<TMessage>();
    }
};

template <typename THandler, std::size_t TSize>
using PolymorphicTypeDirectDispatchMsgRegistry = 
    std::array<const PolymorphicTypeDirectDispatchMethod<THandler>*, TSize>;

template <typename TMsgIdType, typename THandler, std::size_t TSize>
using PolymorphicTypeBinSearchDispatchMsgRegistry = 
    std::array<const PolymorphicTypeBinSearchDispatchMethod<TMsgIdType, THandler>*, TSize>;

template <typename THandler>
class PolymorphicTypeDirectDispatchRegistryFiller
{
public:
    using DispatchMethod = PolymorphicTypeDirectDispatchMethod<THandler>;
    PolymorphicTypeDirectDispatchRegistryFiller(const DispatchMethod** registry)
      : m_registry(registry)
    {
    }

    template <typename TMessage>
    void operator()()
    {
        static_assert(comms::isMessageBase<TMessage>(), "Must be actual message");
        static_assert(messageHasStaticNumId<TMessage>(), "Message must define static ID");
        static const PolymorphicTypeDirectDispatchMethodImpl<THandler, TMessage> Method{};
        m_registry[static_cast<std::size_t>(TMessage::doGetId())] = &Method;
    }
private:
    const DispatchMethod** m_registry;
};

template <typename THandler, std::size_t TSize>
PolymorphicTypeDirectDispatchRegistryFiller<THandler>
polymorphicTypeDirectDispatchMakeRegistryFiller(
    PolymorphicTypeDirectDispatchMsgRegistry<THandler, TSize>& registry)
{
    return PolymorphicTypeDirectDispatchRegistryFiller<THandler>(&registry[0]);
}

template <typename TRegistry, typename TAllMessages>
class PolymorphicTypeDirectDispatchRegistryInitializer
{
public:
    PolymorphicTypeDirectDispatchRegistryInitializer() = delete;
    PolymorphicTypeDirectDispatchRegistryInitializer(const PolymorphicTypeDirectDispatchRegistryInitializer&) = delete;
    explicit PolymorphicTypeDirectDispatchRegistryInitializer(TRegistry& registry)
    {
        std::fill(registry.begin(), registry.end(), nullptr);
        comms::util::tupleForEachType<TAllMessages>(polymorphicTypeDirectDispatchMakeRegistryFiller(registry));
    }
};

template <typename TMsgIdType, typename THandler>
class PolymorphicTypeBinSearchDispatchRegistryFiller
{
public:
    using DispatchMethod = PolymorphicTypeBinSearchDispatchMethod<TMsgIdType, THandler>;
    PolymorphicTypeBinSearchDispatchRegistryFiller(const DispatchMethod** registry)
      : m_registry(registry)
    {
    }

    template <typename TMessage>
    void operator()()
    {
        // static_assert(comms::isMessageBase<TMessage>(), "Must be actual message");
        // static_assert(messageHasStaticNumId<TMessage>(), "Message must define static ID");
        static const PolymorphicTypeBinSearchDispatchMethodImpl<TMsgIdType, THandler, TMessage> Method{};
        m_registry[m_idx] = &Method;
        ++m_idx;
    }
    
private:
    const DispatchMethod** m_registry;
    std::size_t m_idx = 0U;
};

template <typename TMsgIdType, typename THandler, std::size_t TSize>
PolymorphicTypeBinSearchDispatchRegistryFiller<TMsgIdType, THandler>
polymorphicTypeBinSearchDispatchMakeRegistryFiller(
    PolymorphicTypeBinSearchDispatchMsgRegistry<TMsgIdType, THandler, TSize>& registry)
{
    return PolymorphicTypeBinSearchDispatchRegistryFiller<TMsgIdType, THandler>(&registry[0]);
}

template <typename TRegistry, typename TAllMessages>
class PolymorphicTypeBinSearchDispatchRegistryInitializer
{
public:
    explicit PolymorphicTypeBinSearchDispatchRegistryInitializer(TRegistry& registry)
    {
        std::fill(registry.begin(), registry.end(), nullptr);
        comms::util::tupleForEachType<TAllMessages>(polymorphicTypeBinSearchDispatchMakeRegistryFiller(registry));
    }
};

template <typename TAllMessages, typename THandler>
class DispatchMsgTypeDirectPolymorphicHelper    
{
public:
    using FirstMsgType = 
        typename comms::util::TupleElement<TAllMessages>::template Type<0>;
    static_assert(FirstMsgType::hasMsgIdType(), "Message interface class must define its id type");
    using MsgIdParamType = typename FirstMsgType::MsgIdParamType;
    static bool dispatch(MsgIdParamType id, THandler& handler)
    {
        // Access initializer object to ensure it hasn't been erased by the optimizer
        static_cast<void>(s_initializer);

        auto regIdx = static_cast<std::size_t>(id);
        if ((s_registry.size() <= regIdx) ||
            (s_registry[regIdx] == nullptr)) {
            return false;
        }

        s_registry[regIdx]->dispatch(handler);
        return true;
    }

private:
    static const std::size_t RegistrySize = 
        PolymorphicDirectDispatchRegSizeDetect<TAllMessages, std::tuple_size<TAllMessages>::value>::Value;
    using Registry = PolymorphicTypeDirectDispatchMsgRegistry<THandler, RegistrySize>;
    using Initializer = PolymorphicTypeDirectDispatchRegistryInitializer<Registry, TAllMessages>;

    static Registry s_registry;
    static const Initializer s_initializer;
};

template <typename TAllMessages, typename THandler>
typename DispatchMsgTypeDirectPolymorphicHelper<TAllMessages, THandler>::Registry 
DispatchMsgTypeDirectPolymorphicHelper<TAllMessages, THandler>::s_registry;

template <typename TAllMessages, typename THandler>
const typename DispatchMsgTypeDirectPolymorphicHelper<TAllMessages, THandler>::Initializer 
DispatchMsgTypeDirectPolymorphicHelper<TAllMessages, THandler>::s_initializer(s_registry);

template <typename TAllMessages, typename THandler>
class DispatchMsgTypeBinSearchPolymorphicHelperBase
{
protected:
    using FirstMsgType = 
        typename comms::util::TupleElement<TAllMessages>::template Type<0>;
    static_assert(comms::isMessage<FirstMsgType>(), 
            "The type in the tuple are expected to be proper messages");
    static_assert(FirstMsgType::hasMsgIdType(), "Message interface class must define its id type");
    using MsgIdParamType = typename FirstMsgType::MsgIdParamType;

    static const std::size_t RegistrySize = std::tuple_size<TAllMessages>::value;
    using Registry = PolymorphicTypeBinSearchDispatchMsgRegistry<MsgIdParamType, THandler, RegistrySize>;
    using Initializer = PolymorphicTypeBinSearchDispatchRegistryInitializer<Registry, TAllMessages>;

    static Registry s_registry;
    static Initializer s_initializer;
};

template <typename TAllMessages, typename THandler>
typename DispatchMsgTypeBinSearchPolymorphicHelperBase<TAllMessages, THandler>::Registry 
DispatchMsgTypeBinSearchPolymorphicHelperBase<TAllMessages, THandler>::s_registry;

template <typename TAllMessages, typename THandler>
typename DispatchMsgTypeBinSearchPolymorphicHelperBase<TAllMessages, THandler>::Initializer 
DispatchMsgTypeBinSearchPolymorphicHelperBase<TAllMessages, THandler>::s_initializer(s_registry);

template <typename TAllMessages, typename THandler>
class DispatchMsgTypeBinSearchStrongPolymorphicHelper : public
    DispatchMsgTypeBinSearchPolymorphicHelperBase<TAllMessages, THandler>
{
    using Base = DispatchMsgTypeBinSearchPolymorphicHelperBase<TAllMessages, THandler>;
    using Registry = typename Base::Registry;
public:
    using MsgIdParamType = typename Base::MsgIdParamType;
    static bool dispatch(MsgIdParamType id, THandler& handler)
    {
        // Access initializer object to ensure it hasn't been erased by the optimizer
        static_cast<void>(Base::s_initializer);

        auto iter = 
            std::lower_bound(
                Base::s_registry.begin(), Base::s_registry.end(), id, 
                [](typename Registry::value_type method, MsgIdParamType idParam) -> bool
                {
                    COMMS_ASSERT(method != nullptr);
                    return method->getId() < idParam;
                });

        if ((iter == Base::s_registry.end()) || ((*iter)->getId() != id)) {
            return false;    
        }

        (*iter)->dispatch(handler);
        return true;
    }
};

template <typename TAllMessages, typename THandler>
class DispatchMsgTypeBinSearchWeakPolymorphicHelper : public
    DispatchMsgTypeBinSearchPolymorphicHelperBase<TAllMessages, THandler>
{
    using Base = DispatchMsgTypeBinSearchPolymorphicHelperBase<TAllMessages, THandler>;
    using Registry = typename Base::Registry;

public:
    using MsgIdParamType = typename Base::MsgIdParamType;
    static bool dispatch(MsgIdParamType id, std::size_t offset, THandler& handler)
    {
        // Access initializer object to ensure it hasn't been erased by the optimizer
        static_cast<void>(Base::s_initializer);

        using IdType = typename std::decay<decltype(id)>::type;
        auto lowerIter = 
            std::lower_bound(
                Base::s_registry.begin(), Base::s_registry.end(), id, 
                [](typename Registry::value_type method, IdType idParam) -> bool
                {
                    COMMS_ASSERT(method != nullptr);
                    return static_cast<IdType>(method->getId()) < idParam;
                });

        if ((lowerIter == Base::s_registry.end()) || 
            (static_cast<IdType>((*lowerIter)->getId()) != id)) {
            return false;    
        }

        auto upperIter = 
            std::upper_bound(
                lowerIter, Base::s_registry.end(), id,
                [](IdType idParam, typename Registry::value_type method)
                {
                    return idParam < static_cast<IdType>(method->getId());
                }
            );

        COMMS_ASSERT(lowerIter < upperIter);

        auto dist = static_cast<std::size_t>(upperIter - lowerIter);
        if (dist <= offset) {
            return false;
        }

        auto actualIter = lowerIter + offset;
        (*actualIter)->dispatch(handler);
        return true;
    }
};

template <typename...>
class DispatchMsgTypePolymorphicHelper
{
    template <typename... TParams>
    using EmptyTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using DirectTag = comms::details::tag::Tag2<>;

    template <typename... TParams>
    using StrongBinSearchTag = comms::details::tag::Tag3<>;

    template <typename... TParams>
    using WeakBinSearchTag = comms::details::tag::Tag4<>;

    template <typename TAllMessages, typename...>
    using DirectStrongTag = 
        typename comms::util::LazyShallowConditional<
            dispatchMsgPolymorphicIsDirectSuitable<TAllMessages>()
        >::template Type<
            DirectTag,
            StrongBinSearchTag
        >;

    template <typename TAllMessages, typename...>
    using DirectStrongWeakTag = 
        typename comms::util::LazyShallowConditional<
            allMessagesAreStrongSorted<TAllMessages>()
        >::template Type<
            DirectStrongTag,
            WeakBinSearchTag,
            TAllMessages
        >;


    template <typename TAllMessages, typename...>
    using Tag = 
        typename comms::util::LazyShallowConditional<
            std::tuple_size<TAllMessages>::value == 0U
        >::template Type<
            EmptyTag,
            DirectStrongWeakTag,
            TAllMessages
        >;

public:
    template <typename TAllMessages, typename TId, typename THandler>
    static bool dispatch(TId&& id, THandler& handler) 
    {
        static_assert(allMessagesAreWeakSorted<TAllMessages>(),
            "Message types must be sorted by their ID");

        return dispatchInternal<TAllMessages>(std::forward<TId>(id), handler, Tag<TAllMessages>());
    }

    template <typename TAllMessages, typename TId, typename THandler>
    static bool dispatch(TId&& id, std::size_t offset, THandler& handler) 
    {
        static_assert(allMessagesAreWeakSorted<TAllMessages>(),
            "Message types must be sorted by their ID");

        return dispatchInternal<TAllMessages>(std::forward<TId>(id), offset, handler, Tag<TAllMessages>());
    }

private:

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchInternal(TId&& id, THandler& handler, EmptyTag<TParams...>) 
    {
        static_cast<void>(id);
        static_cast<void>(handler);
        return false;
    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchInternal(TId&& id, std::size_t offset, THandler& handler, EmptyTag<TParams...>) 
    {
        static_cast<void>(id);
        static_cast<void>(handler);
        static_cast<void>(offset);
        return false;
    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchInternal(TId&& id, THandler& handler, DirectTag<TParams...>) 
    {
        using FirstMsgType = 
            typename comms::util::TupleElement<TAllMessages>::template Type<0>;
        static_assert(comms::isMessage<FirstMsgType>(), 
            "The type in the tuple are expected to be proper messages");
        static_assert(FirstMsgType::hasMsgIdType(), "The messages must define their ID type");
        using MsgIdParamType = typename FirstMsgType::MsgIdParamType;
        using HandlerType = typename std::decay<decltype(handler)>::type;
        return DispatchMsgTypeDirectPolymorphicHelper<TAllMessages, HandlerType>::
            dispatch(static_cast<MsgIdParamType>(id), handler);
    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchInternal(TId&& id, std::size_t offset, THandler& handler, DirectTag<TParams...>) 
    {
        if (offset != 0U) {
            return false;
        }

        return dispatchInternal<TAllMessages>(std::forward<TId>(id), handler, DirectTag<>());
    }    

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchInternal(TId&& id, THandler& handler, StrongBinSearchTag<TParams...>) 
    {
        using FirstMsgType = 
            typename comms::util::TupleElement<TAllMessages>::template Type<0>;
        static_assert(comms::isMessage<FirstMsgType>(), 
            "The type in the tuple are expected to be proper messages");
        static_assert(FirstMsgType::hasMsgIdType(), "The messages must define their ID type");
        using MsgIdParamType = typename FirstMsgType::MsgIdParamType;
        using HandlerType = typename std::decay<decltype(handler)>::type;
        return DispatchMsgTypeBinSearchStrongPolymorphicHelper<TAllMessages, HandlerType>::
            dispatch(static_cast<MsgIdParamType>(id), handler);
    }   

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchInternal(TId&& id, std::size_t offset, THandler& handler, StrongBinSearchTag<TParams...>) 
    {
        if (offset != 0U) {
            return false;
        }

        return dispatchInternal<TAllMessages>(std::forward<TId>(id), handler, StrongBinSearchTag<>());
    }         

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchInternal(TId&& id, THandler& handler, WeakBinSearchTag<TParams...>)
    {
        return dispatchInternal<TAllMessages>(std::forward<TId>(id), 0U, handler, WeakBinSearchTag<>());
    }

    template <typename TAllMessages, typename TId, typename THandler, typename... TParams>
    static bool dispatchInternal(TId&& id, std::size_t offset, THandler& handler, WeakBinSearchTag<TParams...>) 
    {
        using FirstMsgType = 
            typename comms::util::TupleElement<TAllMessages>::template Type<0>;
        static_assert(comms::isMessage<FirstMsgType>(), 
            "The type in the tuple are expected to be proper messages");
        static_assert(FirstMsgType::hasMsgIdType(), "The messages must define their ID type");
        using MsgIdParamType = typename FirstMsgType::MsgIdParamType;
        using HandlerType = typename std::decay<decltype(handler)>::type;
        return DispatchMsgTypeBinSearchWeakPolymorphicHelper<TAllMessages, HandlerType>::
            dispatch(static_cast<MsgIdParamType>(id), offset, handler);
    }       
};

} // namespace details

} // namespace comms
