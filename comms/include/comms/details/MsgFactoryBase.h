//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <memory>

#include "comms/Assert.h"
#include "comms/util/Tuple.h"
#include "comms/util/alloc.h"
#include "comms/util/type_traits.h"
#include "comms/MessageBase.h"
#include "comms/details/message_check.h"
#include "comms/traits.h"
#include "comms/dispatch.h"
#include "comms/details/message_check.h"

namespace comms
{

namespace details
{

template <typename TAllMessages>
constexpr bool msgFactoryAllHaveStaticNumId()
{
    return allMessagesHaveStaticNumId<TAllMessages>();
}

template <typename TMessage>
constexpr bool msgFactoryMessageHasStaticNumId()
{
    return messageHasStaticNumId<TMessage>();
}

template<bool TMustCat>
struct MsgFactoryAllMessagesRetrieveHelper;

template<>
struct MsgFactoryAllMessagesRetrieveHelper<true>
{
    template <typename TAll, typename TOpt>
    using Type =
        typename std::decay<
            decltype(
                std::tuple_cat(
                    std::declval<TAll>(),
                    std::declval<std::tuple<typename TOpt::GenericMessage> >()
                )
            )
        >::type;
};

template<>
struct MsgFactoryAllMessagesRetrieveHelper<false>
{
    template <typename TAll, typename TOpt>
    using Type = TAll;
};

template <typename TAll, typename TOpt>
using AllMessagesBundle =
    typename MsgFactoryAllMessagesRetrieveHelper<TOpt::HasInPlaceAllocation && TOpt::HasSupportGenericMessage>::template Type<TAll, TOpt>;

template <bool THasGenericMessage>
struct MsgFactorGenericMsgRetrieveHelper;

template <>
struct MsgFactorGenericMsgRetrieveHelper<true>
{
    template <typename TOpt>
    using Type = typename TOpt::GenericMessage;
};

template <>
struct MsgFactorGenericMsgRetrieveHelper<false>
{
    template <typename TOpt>
    using Type = void;
};

template <typename TOpt>
using MsgFactoryGenericMsgType =
    typename MsgFactorGenericMsgRetrieveHelper<TOpt::HasSupportGenericMessage>::
        template Type<TOpt>;

template <typename TMsgBase, typename TAllMessages, typename... TOptions>
class MsgFactoryBase
{
    static_assert(TMsgBase::InterfaceOptions::HasMsgIdType,
        "Usage of MsgFactoryBase requires Message interface to provide ID type. "
        "Use comms::option::def::MsgIdType option in message interface type definition.");
    using ParsedOptionsInternal = details::MsgFactoryOptionsParser<TOptions...>;

    static const bool InterfaceHasVirtualDestructor =
        std::has_virtual_destructor<TMsgBase>::value;

    using AllMessagesInternal = AllMessagesBundle<TAllMessages, ParsedOptionsInternal>;
    using GenericMessageInternal = MsgFactoryGenericMsgType<ParsedOptionsInternal>;

    using Alloc =
        typename comms::util::Conditional<
            ParsedOptionsInternal::HasInPlaceAllocation
        >::template Type<
            typename comms::util::Conditional<
                InterfaceHasVirtualDestructor
            >::template Type<
                util::alloc::InPlaceSingle<TMsgBase, AllMessagesInternal>,
                util::alloc::InPlaceSingleNoVirtualDestructor<
                    TMsgBase,
                    AllMessagesInternal,
                    TAllMessages,
                    typename
                    TMsgBase::MsgIdType,
                    GenericMessageInternal
                >
            >,
            typename comms::util::Conditional<
                InterfaceHasVirtualDestructor
            >::template Type<
                util::alloc::DynMemory<TMsgBase>,
                util::alloc::DynMemoryNoVirtualDestructor<
                    TMsgBase,
                    TAllMessages,
                    typename TMsgBase::MsgIdType,
                    GenericMessageInternal
                >
            >
        >;
public:
    using ParsedOptions = ParsedOptionsInternal;
    using Message = TMsgBase;
    using MsgIdParamType = typename Message::MsgIdParamType;
    using MsgIdType = typename Message::MsgIdType;
    using MsgPtr = typename Alloc::Ptr;
    using AllMessages = TAllMessages;

    enum class CreateFailureReason
    {
        None,
        InvalidId,
        AllocFailure,
        NumOfValues
    };

    MsgPtr createMsg(MsgIdParamType id, unsigned idx, CreateFailureReason* reason) const
    {
        CreateFailureReason reasonTmp = CreateFailureReason::None;
        bool result = false;
        MsgPtr msg = createMsgInternal(id, idx, result, DestructorTag());
        do {
            if (msg) {
                COMMS_ASSERT(result);
                break;
            }

            if (!result) {
                reasonTmp = CreateFailureReason::InvalidId;
                break;
            }

            reasonTmp = CreateFailureReason::AllocFailure;
        } while (false);
        
        if (reason != nullptr) {
            *reason = reasonTmp;
        }        

        return msg;
    }

    MsgPtr createGenericMsg(MsgIdParamType id, unsigned idx) const
    {
        static_cast<void>(this);
        using Tag =
            typename comms::util::Conditional<
                ParsedOptions::HasSupportGenericMessage
            >::template Type<
                AllocGenericTag,
                NoAllocTag
            >;

        return createGenericMsgInternal(id, idx, Tag(), DestructorTag());
    }

    bool canAllocate() const
    {
        return alloc_.canAllocate();
    }

    std::size_t msgCount(MsgIdParamType id) const
    {
        return comms::dispatchMsgTypeCountStaticBinSearch<AllMessages>(id);
    }

    static constexpr bool hasUniqueIds()
    {
        return comms::details::allMessagesAreStrongSorted<AllMessages>();
    }

    static constexpr bool isDispatchPolymorphic()
    {
        return isDispatchPolymorphicInternal(DispatchTag());
    }

    static constexpr bool isDispatchStaticBinSearch()
    {
        return isDispatchStaticBinSearchInternal(DispatchTag());
    }

    static constexpr bool isDispatchLinearSwitch()
    {
        return isDispatchLinearSwitchInternal(DispatchTag());
    }

protected:
    MsgFactoryBase() = default;
    MsgFactoryBase(const MsgFactoryBase&) = default;
    MsgFactoryBase(MsgFactoryBase&&) = default;
    MsgFactoryBase& operator=(const MsgFactoryBase&) = default;
    MsgFactoryBase& operator=(MsgFactoryBase&&) = default;

    template <typename TObj, typename... TArgs>
    MsgPtr allocMsg(TArgs&&... args) const
    {
        static_assert(std::is_base_of<Message, TObj>::value,
            "TObj is not a proper message type");

        static_assert(std::has_virtual_destructor<TObj>::value,
            "This function is expected to be called for message objects with virtual destructor");
        static_assert(
            (!ParsedOptionsInternal::HasInPlaceAllocation) ||
                    comms::util::IsInTuple<TObj, AllMessagesInternal>::Value,
            "TObj must be in provided tuple of supported messages");

        return alloc_.template alloc<TObj>(std::forward<TArgs>(args)...);
    }

    template <typename TObj, typename... TArgs>
    MsgPtr allocMsg(MsgIdParamType id, unsigned idx, TArgs&&... args) const
    {
        static_assert(std::is_base_of<Message, TObj>::value,
            "TObj is not a proper message type");

        static_assert(!std::has_virtual_destructor<TObj>::value,
            "This function is expected to be called for message objects without virtual destructor");

        static_assert(
            (!ParsedOptionsInternal::HasInPlaceAllocation) ||
                    comms::util::IsInTuple<TObj, AllMessagesInternal>::Value,
            "TObj must be in provided tuple of supported messages");

        return alloc_.template alloc<TObj>(id, idx, std::forward<TArgs>(args)...);
    }

private:
    struct AllocGenericTag {};
    struct NoAllocTag {};

    struct ForcedTag {};
    struct StandardTag {};

    struct VirtualDestructorTag {};
    struct NonVirtualDestructorTag {};

    using DispatchTag = 
        typename comms::util::Conditional<
            ParsedOptions::HasForcedDispatch
        >::template Type<
            ForcedTag,
            StandardTag
        >;

    using DestructorTag =
        typename comms::util::Conditional<
            InterfaceHasVirtualDestructor
        >::template Type<
            VirtualDestructorTag,
            NonVirtualDestructorTag
        >;

    class CreateHandler
    {
    public:
        explicit CreateHandler(Alloc& a) : a_(a) {}

        MsgPtr getMsg()
        {
            return std::move(msg_);
        }

        template <typename T>
        void handle()
        {
            msg_ = a_.template alloc<T>();
        }

    private:
        Alloc& a_;
        MsgPtr msg_;
    };

    class NonVirtualDestructorCreateHandler
    {
    public:
        explicit NonVirtualDestructorCreateHandler(MsgIdParamType id, unsigned idx, Alloc& a) :
            id_(id),
            idx_(idx),
            a_(a)
        {
        }

        MsgPtr getMsg()
        {
            return std::move(msg_);
        }

        template <typename T>
        void handle()
        {
            msg_ = a_.template alloc<T>(id_, idx_);
        }

    private:
        MsgIdType id_;
        unsigned idx_ = 0U;
        Alloc& a_;
        MsgPtr msg_;
    };

    MsgPtr createGenericMsgInternal(MsgIdParamType id, unsigned idx, AllocGenericTag, VirtualDestructorTag) const
    {
        static_cast<void>(idx);
        static_assert(std::is_base_of<Message, typename ParsedOptions::GenericMessage>::value,
            "The requested GenericMessage class must have the same interface class as all other messages");
        return allocMsg<typename ParsedOptions::GenericMessage>(id);
    }

    MsgPtr createGenericMsgInternal(MsgIdParamType id, unsigned idx, AllocGenericTag, NonVirtualDestructorTag) const
    {
        static_assert(std::is_base_of<Message, typename ParsedOptions::GenericMessage>::value,
            "The requested GenericMessage class must have the same interface class as all other messages");
        return allocMsg<typename ParsedOptions::GenericMessage>(id, idx, id);
    }

    template <typename TDestructorTag>
    static MsgPtr createGenericMsgInternal(MsgIdParamType, NoAllocTag, TDestructorTag)
    {
        return MsgPtr();
    }

    template <typename THandler>
    static bool dispatchMsgTypeInternal(MsgIdParamType id, unsigned idx, THandler& handler, StandardTag)
    {
        return comms::dispatchMsgType<AllMessages>(id, idx, handler);
    }

    template <typename THandler>
    static bool dispatchMsgTypeInternal(MsgIdParamType id, unsigned idx, THandler& handler, ForcedTag)
    {
        using Tag = typename ParsedOptions::ForcedDispatch;
        return dispatchMsgTypeInternal(id, idx, handler, Tag());
    }

    template <typename THandler>
    static bool dispatchMsgTypeInternal(MsgIdParamType id, unsigned idx, THandler& handler, comms::traits::dispatch::Polymorphic)
    {
        return comms::dispatchMsgTypePolymorphic<AllMessages>(id, idx, handler);    
    }

    template <typename THandler>
    static bool dispatchMsgTypeInternal(MsgIdParamType id, unsigned idx, THandler& handler, comms::traits::dispatch::StaticBinSearch)
    {
        return comms::dispatchMsgTypeStaticBinSearch<AllMessages>(id, idx, handler);    
    }

    template <typename THandler>
    static bool dispatchMsgTypeInternal(MsgIdParamType id, unsigned idx, THandler& handler, comms::traits::dispatch::LinearSwitch)
    {
        return comms::dispatchMsgTypeStaticBinSearch<AllMessages>(id, idx, handler);    
    }

    static constexpr bool isDispatchPolymorphicInternal(ForcedTag)
    {
        return std::is_same<comms::traits::dispatch::Polymorphic, typename ParsedOptions::ForcedDispatch>::value; 
    }

    static constexpr bool isDispatchPolymorphicInternal(StandardTag)
    {
        return dispatchMsgTypeIsPolymorphic<AllMessages>(); 
    }

    static constexpr bool isDispatchStaticBinSearchInternal(ForcedTag)
    {
        return std::is_same<comms::traits::dispatch::StaticBinSearch, typename ParsedOptions::ForcedDispatch>::value; 
    }

    static constexpr bool isDispatchStaticBinSearchInternal(StandardTag)
    {
        return dispatchMsgTypeIsStaticBinSearch<AllMessages>(); 
    }

    static constexpr bool isDispatchLinearSwitchInternal(ForcedTag)
    {
        return std::is_same<comms::traits::dispatch::LinearSwitch, typename ParsedOptions::ForcedDispatch>::value; 
    }

    static constexpr bool isDispatchLinearSwitchInternal(StandardTag)
    {
        return false;
    }

    MsgPtr createMsgInternal(MsgIdParamType id, unsigned idx, bool& success, VirtualDestructorTag) const
    {
        CreateHandler handler(alloc_);
        success = dispatchMsgTypeInternal(id, idx, handler, DispatchTag());
        return handler.getMsg();
    }

    MsgPtr createMsgInternal(MsgIdParamType id, unsigned idx, bool& success, NonVirtualDestructorTag) const
    {
        NonVirtualDestructorCreateHandler handler(id, idx, alloc_);
        success = dispatchMsgTypeInternal(id, idx, handler, DispatchTag());
        return handler.getMsg();
    }


    mutable Alloc alloc_;
};


}  // namespace details

}  // namespace comms


