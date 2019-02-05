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
#include <array>
#include <algorithm>

#include "comms/Assert.h"
#include "comms/Message.h"
#include "comms/MessageBase.h"
#include "comms/util/Tuple.h"
#include "comms/details/message_check.h"

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

    using MsgIdParamType = typename TMsgBase::MsgIdParamType;
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
    using MsgIdParamType = typename Base::MsgIdParamType;

    PolymorphicBinSearchDispatchMethodImpl() = default;
    PolymorphicBinSearchDispatchMethodImpl(const PolymorphicBinSearchDispatchMethodImpl&) = delete;
    PolymorphicBinSearchDispatchMethodImpl& operator=(const PolymorphicBinSearchDispatchMethodImpl&) = delete;


    static MsgIdParamType doGetId()
    {
        static_assert(comms::isMessageBase<TMessage>(), "Must be actual message");
        static_assert(messageHasStaticNumId<TMessage>(), "Message must define static numeric ID");

        return TMessage::doGetId();
    }

protected:
    
    virtual MsgIdParamType getIdImpl() const
    {
        return doGetId();
    }

    virtual auto dispatchImpl(TMsgBase& msg, THandler& handler) const ->
        MessageInterfaceDispatchRetType<THandler>
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
        m_registry[TMessage::doGetId()] = &Method;
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
    using MsgType = typename std::tuple_element<TMaxSize - 1, TAllMessages>::type;
    static_assert(comms::isMessageBase<MsgType>(), "Must be actual message");
    static_assert(messageHasStaticNumId<MsgType>(), "Message must define static numeric ID");
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
    template <typename TId>
    static auto dispatch(TId&& id, TMsgBase& msg, THandler& handler) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        // Access initializer object to ensure it hasn't been erased by the optimizer
        static_cast<void>(s_initializer);

        using RetType = 
            details::MessageInterfaceDispatchRetType<
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
    template <typename TId>
    static auto dispatch(TId&& id, TMsgBase& msg, THandler& handler) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        // Access initializer object to ensure it hasn't been erased by the optimizer
        static_cast<void>(Base::s_initializer);

        using RetType = 
            details::MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        using IdType = typename std::decay<decltype(id)>::type;

        auto iter = 
            std::lower_bound(
                Base::s_registry.begin(), Base::s_registry.end(), id, 
                [](typename Registry::value_type method, IdType idParam) -> bool
                {
                    COMMS_ASSERT(method != nullptr);
                    return static_cast<IdType>(method->getId()) < idParam;
                });

        if ((iter == Base::s_registry.end()) || (static_cast<IdType>((*iter)->getId()) != id)) {
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
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        // Access initializer object to ensure it hasn't been erased by the optimizer
        static_cast<void>(Base::s_initializer);

        using RetType = 
            details::MessageInterfaceDispatchRetType<
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

template <typename TAllMessages, std::size_t TCount>
class DispatchMsgPolymorphicIsDirectSuitable
{
    using LastMsg = 
        typename std::tuple_element<
            std::tuple_size<TAllMessages>::value - 1,
            TAllMessages
        >::type;

    static_assert(comms::isMessageBase<LastMsg>(), "LastMsg must be message type");
    static_assert(messageHasStaticNumId<LastMsg>(), "LastMsg must define static numeric id");

    static const std::size_t MaxId = 
        static_cast<std::size_t>(LastMsg::doGetId());

    static const std::size_t MaxAllowedId = (TCount * 11) / 10;

public:
    static const bool Value = 
        (MaxId <= (TCount + 10U)) || (MaxId <= MaxAllowedId);
};

template <typename TAllMessages>
class DispatchMsgPolymorphicIsDirectSuitable<TAllMessages, 0U>
{
    static const bool Value = true;
};

template <typename TAllMessages, typename TMsgBase, typename THandler>
class DispatchMsgPolymorphicHelper
{
    struct DispatchInterfaceTag {};
    struct DirectTag {};
    struct StrongBinSearchTag {};
    struct WeakBinSearchTag {};

    struct IdInterfaceTag {};
    struct NoIdInterfaceTag {};

    static const std::size_t NumOfMessages = 
        std::tuple_size<TAllMessages>::value;

    static_assert(allMessagesAreWeakSorted<TAllMessages>(),
        "Message types must be sorted by their ID");

    using Tag = 
        typename std::conditional<
            TMsgBase::hasDispatch(),
            DispatchInterfaceTag,
            typename std::conditional<
                allMessagesAreStrongSorted<TAllMessages>(),
                typename std::conditional<
                    DispatchMsgPolymorphicIsDirectSuitable<TAllMessages, NumOfMessages>::Value,
                    DirectTag,
                    StrongBinSearchTag
                >::type,
                WeakBinSearchTag
            >::type
        >::type;
public:
    static auto dispatch(TMsgBase& msg, THandler& handler) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using IdRetrieveTag = 
            typename std::conditional<
                TMsgBase::hasGetId(),
                IdInterfaceTag,
                NoIdInterfaceTag
            >::type;

        return dispatchInternal( msg, handler, IdRetrieveTag());
    }

    template <typename TId>
    static auto dispatch(TId&& id, TMsgBase& msg, THandler& handler) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
        {
            return dispatchInternal(std::forward<TId>(id), msg, handler, Tag());
        }

    template <typename TId>
    static auto dispatch(TId&& id, std::size_t offset, TMsgBase& msg, THandler& handler) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return dispatchInternal(std::forward<TId>(id), offset, msg, handler, Tag());
    }

private:
    static auto dispatchInternal(TMsgBase& msg, THandler& handler, IdInterfaceTag) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        typename TMsgBase::MsgIdParamType id = msg.getId();
        return dispatch(id, msg, handler);
    }

    static auto dispatchInternal(TMsgBase& msg, THandler& handler, NoIdInterfaceTag) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_assert(TMsgBase::hasDispatch(), "The message must provide polymorphic dispatch");
        static_assert(std::is_base_of<typename TMsgBase::Handler, THandler>::value,
            "Incompatible handlers");

        using RetType = MessageInterfaceDispatchRetType<THandler>;
        return static_cast<RetType>(msg.dispatch(handler));
    }

    template <typename TId>
    static auto dispatchInternal(TId&& id, TMsgBase& msg, THandler& handler, DispatchInterfaceTag) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(id);
        return dispatchInternal(msg, handler, NoIdInterfaceTag());
    }

    template <typename TId>
    static auto dispatchInternal(TId&& id, TMsgBase& msg, THandler& handler, DirectTag) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return DispatchMsgDirectPolymorphicHelper<TAllMessages, TMsgBase, THandler>::
            dispatch(std::forward<TId>(id), msg, handler);
    }

    template <typename TId>
    static auto dispatchInternal(TId&& id, TMsgBase& msg, THandler& handler, StrongBinSearchTag) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return DispatchMsgBinSearchStrongPolymorphicHelper<TAllMessages, TMsgBase, THandler>::
            dispatch(std::forward<TId>(id), msg, handler);
    }

    template <typename TId>
    static auto dispatchInternal(TId&& id, TMsgBase& msg, THandler& handler, WeakBinSearchTag) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return dispatchInternal(std::forward<TId>(id), 0U, msg, handler, WeakBinSearchTag());
    }

    template <typename TId>
    static auto dispatchInternal(TId&& id, std::size_t offset, TMsgBase& msg, THandler& handler, DispatchInterfaceTag) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(id);
        static_assert(std::is_base_of<typename TMsgBase::Handler, THandler>::value,
            "Incompatible handlers");

        using RetType = MessageInterfaceDispatchRetType<THandler>;
        if (offset != 0U) {
            return static_cast<RetType>(handler.handle(msg));
        }

        return static_cast<RetType>(msg.dispatch(handler));
    }

    template <typename TId>
    static auto dispatchInternal(TId&& id, std::size_t offset, TMsgBase& msg, THandler& handler, DirectTag) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        if (offset != 0U) {
            using RetType = MessageInterfaceDispatchRetType<THandler>;
            return static_cast<RetType>(handler.handle(msg));
        }

        return dispatchInternal(std::forward<TId>(id), msg, handler, DirectTag());
    }

    template <typename TId>
    static auto dispatchInternal(TId&& id, std::size_t offset, TMsgBase& msg, THandler& handler, StrongBinSearchTag) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        if (offset != 0U) {
            using RetType = MessageInterfaceDispatchRetType<THandler>;
            return static_cast<RetType>(handler.handle(msg));
        }

        return dispatchInternal(std::forward<TId>(id), msg, handler, StrongBinSearchTag());
    }

    template <typename TId>
    static auto dispatchInternal(TId&& id, std::size_t offset, TMsgBase& msg, THandler& handler, WeakBinSearchTag) ->
        details::MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return DispatchMsgBinSearchWeakPolymorphicHelper<TAllMessages, TMsgBase, THandler>::
            dispatch(std::forward<TId>(id), offset, msg, handler);
    }
};

} // namespace details

} // namespace comms