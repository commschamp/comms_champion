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
#include "comms/details/tag.h"

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

template <typename TMsgBase, typename TAllMessages, typename... TOptions>
class MsgFactoryBase
{
    static_assert(TMsgBase::InterfaceOptions::HasMsgIdType,
        "Usage of MsgFactoryBase requires Message interface to provide ID type. "
        "Use comms::option::def::MsgIdType option in message interface type definition.");
    using ParsedOptionsInternal = details::MsgFactoryOptionsParser<TOptions...>;

    static const bool InterfaceHasVirtualDestructor =
        std::has_virtual_destructor<TMsgBase>::value;

    using AllMessagesInternal = 
        typename ParsedOptionsInternal::template AllMessages<TAllMessages>;
    using GenericMessageInternal = typename ParsedOptionsInternal::GenericMessage; 

    template <typename...>
    struct InPlaceAllocDeepCondWrap
    {
        template <
            typename TInterface,
            typename TAllocMessages,
            typename TOrigMessages,
            typename TId,
            typename TDefaultType,
            typename...>        
        using Type = 
            typename comms::util::LazyDeepConditional<
                InterfaceHasVirtualDestructor
            >::template Type<
                comms::util::alloc::details::InPlaceSingleDeepCondWrap,
                comms::util::alloc::details::InPlaceSingleNoVirtualDestructorDeepCondWrap,
                TInterface,
                TAllocMessages,
                TOrigMessages,
                TId,
                TDefaultType
            >;
    };

    template <typename...>
    struct DynMemoryAllocDeepCondWrap
    {
        template <
            typename TInterface,
            typename TAllocMessages,
            typename TOrigMessages,
            typename TId,
            typename TDefaultType,
            typename...>        
        using Type = 
            typename comms::util::LazyDeepConditional<
                InterfaceHasVirtualDestructor
            >::template Type<
                comms::util::alloc::details::DynMemoryDeepCondWrap,
                comms::util::alloc::details::DynMemoryNoVirtualDestructorDeepCondWrap,
                TInterface,
                TOrigMessages,
                TId,
                TDefaultType
            >;
    };    

    using Alloc =
        typename comms::util::LazyDeepConditional<
            ParsedOptionsInternal::HasInPlaceAllocation
        >::template Type<
            InPlaceAllocDeepCondWrap,
            DynMemoryAllocDeepCondWrap,
            TMsgBase,
            AllMessagesInternal,
            TAllMessages,
            typename TMsgBase::MsgIdType,
            GenericMessageInternal
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
        MsgPtr msg = createMsgInternal(id, idx, result, DestructorTag<>());
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
            typename comms::util::LazyShallowConditional<
                ParsedOptions::HasSupportGenericMessage
            >::template Type<
                AllocGenericTag,
                NoAllocTag
            >;

        return createGenericMsgInternal(id, idx, Tag(), DestructorTag<>());
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
        return isDispatchPolymorphicInternal(DispatchTag<>());
    }

    static constexpr bool isDispatchStaticBinSearch()
    {
        return isDispatchStaticBinSearchInternal(DispatchTag<>());
    }

    static constexpr bool isDispatchLinearSwitch()
    {
        return isDispatchLinearSwitchInternal(DispatchTag<>());
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
    template <typename... TParams>
    using AllocGenericTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using NoAllocTag = comms::details::tag::Tag2<>;    

    template <typename... TParams>
    using ForcedTag = comms::details::tag::Tag3<>;    

    template <typename... TParams>
    using StandardTag = comms::details::tag::Tag4<>; 

    template <typename... TParams>
    using VirtualDestructorTag = comms::details::tag::Tag5<>; 

    template <typename... TParams>
    using NonVirtualDestructorTag = comms::details::tag::Tag6<>;              

    template <typename...>
    using DispatchTag = 
        typename comms::util::LazyShallowConditional<
            ParsedOptions::HasForcedDispatch
        >::template Type<
            ForcedTag,
            StandardTag
        >;

    template <typename...>
    using DestructorTag =
        typename comms::util::LazyShallowConditional<
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

    template <typename... TParams>
    MsgPtr createGenericMsgInternal(MsgIdParamType id, unsigned idx, AllocGenericTag<TParams...>, VirtualDestructorTag<TParams...>) const
    {
        static_cast<void>(idx);
        static_assert(std::is_base_of<Message, typename ParsedOptions::GenericMessage>::value,
            "The requested GenericMessage class must have the same interface class as all other messages");
        return allocMsg<typename ParsedOptions::GenericMessage>(id);
    }

    template <typename... TParams>
    MsgPtr createGenericMsgInternal(MsgIdParamType id, unsigned idx, AllocGenericTag<TParams...>, NonVirtualDestructorTag<TParams...>) const
    {
        static_assert(std::is_base_of<Message, typename ParsedOptions::GenericMessage>::value,
            "The requested GenericMessage class must have the same interface class as all other messages");
        return allocMsg<typename ParsedOptions::GenericMessage>(id, idx, id);
    }

    template <typename TDestructorTag, typename... TParams>
    static MsgPtr createGenericMsgInternal(MsgIdParamType, NoAllocTag<TParams...>, TDestructorTag)
    {
        return MsgPtr();
    }

    template <typename THandler, typename... TParams>
    static bool dispatchMsgTypeInternal(MsgIdParamType id, unsigned idx, THandler& handler, StandardTag<TParams...>)
    {
        return comms::dispatchMsgType<AllMessages>(id, idx, handler);
    }

    template <typename THandler, typename... TParams>
    static bool dispatchMsgTypeInternal(MsgIdParamType id, unsigned idx, THandler& handler, ForcedTag<TParams...>)
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

    template <typename... TParams>
    static constexpr bool isDispatchPolymorphicInternal(ForcedTag<TParams...>)
    {
        return std::is_same<comms::traits::dispatch::Polymorphic, typename ParsedOptions::ForcedDispatch>::value; 
    }

    template <typename... TParams>
    static constexpr bool isDispatchPolymorphicInternal(StandardTag<TParams...>)
    {
        return dispatchMsgTypeIsPolymorphic<AllMessages>(); 
    }

    template <typename... TParams>
    static constexpr bool isDispatchStaticBinSearchInternal(ForcedTag<TParams...>)
    {
        return std::is_same<comms::traits::dispatch::StaticBinSearch, typename ParsedOptions::ForcedDispatch>::value; 
    }

    template <typename... TParams>
    static constexpr bool isDispatchStaticBinSearchInternal(StandardTag<TParams...>)
    {
        return dispatchMsgTypeIsStaticBinSearch<AllMessages>(); 
    }

    template <typename... TParams>
    static constexpr bool isDispatchLinearSwitchInternal(ForcedTag<TParams...>)
    {
        return std::is_same<comms::traits::dispatch::LinearSwitch, typename ParsedOptions::ForcedDispatch>::value; 
    }

    template <typename... TParams>
    static constexpr bool isDispatchLinearSwitchInternal(StandardTag<TParams...>)
    {
        return false;
    }

    template <typename... TParams>
    MsgPtr createMsgInternal(MsgIdParamType id, unsigned idx, bool& success, VirtualDestructorTag<TParams...>) const
    {
        CreateHandler handler(alloc_);
        success = dispatchMsgTypeInternal(id, idx, handler, DispatchTag<>());
        return handler.getMsg();
    }

    template <typename... TParams>
    MsgPtr createMsgInternal(MsgIdParamType id, unsigned idx, bool& success, NonVirtualDestructorTag<TParams...>) const
    {
        NonVirtualDestructorCreateHandler handler(id, idx, alloc_);
        success = dispatchMsgTypeInternal(id, idx, handler, DispatchTag<>());
        return handler.getMsg();
    }

    mutable Alloc alloc_;
};


}  // namespace details

}  // namespace comms


