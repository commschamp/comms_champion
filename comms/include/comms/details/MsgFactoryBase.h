//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include <type_traits>
#include <memory>

#include "comms/Assert.h"
#include "comms/util/Tuple.h"
#include "comms/util/alloc.h"
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
struct AllMessagesRetrieveHelper;

template<>
struct AllMessagesRetrieveHelper<true>
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
struct AllMessagesRetrieveHelper<false>
{
    template <typename TAll, typename TOpt>
    using Type = TAll;
};

template <typename TAll, typename TOpt>
using AllMessagesBundle =
    typename AllMessagesRetrieveHelper<TOpt::HasInPlaceAllocation && TOpt::HasSupportGenericMessage>::template Type<TAll, TOpt>;

template <typename TMsgBase, typename TAllMessages, typename... TOptions>
class MsgFactoryBase
{
    static_assert(TMsgBase::InterfaceOptions::HasMsgIdType,
        "Usage of MsgFactoryBase requires Message interface to provide ID type. "
        "Use comms::option::MsgIdType option in message interface type definition.");
    using ParsedOptionsInternal = details::MsgFactoryOptionsParser<TOptions...>;

    using AllMessagesInternal = AllMessagesBundle<TAllMessages, ParsedOptionsInternal>;
    using Alloc =
        typename std::conditional<
            ParsedOptionsInternal::HasInPlaceAllocation,
            util::alloc::InPlaceSingle<TMsgBase, AllMessagesInternal>,
            util::alloc::DynMemory<TMsgBase>
        >::type;
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
        MsgPtr msg;
        CreateHandler handler(alloc_);
        using Tag = 
            typename std::conditional<
                ParsedOptions::HasForcedDispatch,
                ForcedTag,
                StandardTag
            >::type;
        bool result = dispatchMsgTypeInternal(id, idx, handler, Tag());
        do {
            if (!result) {
                reasonTmp = CreateFailureReason::InvalidId;
                break;
            }

            msg = handler.getMsg();
            if (!msg) {
                reasonTmp = CreateFailureReason::AllocFailure;
                break;
            }
        } while (false);
        
        if (reason != nullptr) {
            *reason = reasonTmp;
        }        

        return msg;
    }

    MsgPtr createGenericMsg(MsgIdParamType id) const
    {
        static_cast<void>(this);
        using Tag =
            typename std::conditional<
                ParsedOptions::HasSupportGenericMessage,
                AllocGenericTag,
                NoAllocTag
            >::type;

        return createGenericMsgInternal(id, Tag());
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

        static_assert(
            (!ParsedOptionsInternal::HasInPlaceAllocation) ||
                    comms::util::IsInTuple<TObj, AllMessagesInternal>::Value,
            "TObj must be in provided tuple of supported messages");

        return alloc_.template alloc<TObj>(std::forward<TArgs>(args)...);
    }

private:
    struct AllocGenericTag {};
    struct NoAllocTag {};

    struct ForcedTag {};
    struct StandardTag {};

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

    MsgPtr createGenericMsgInternal(MsgIdParamType id, AllocGenericTag) const
    {
        static_assert(std::is_base_of<Message, typename ParsedOptions::GenericMessage>::value,
            "The requested GenericMessage class must have the same interface class as all other messages");
        return allocMsg<typename ParsedOptions::GenericMessage>(id);
    }

    static MsgPtr createGenericMsgInternal(MsgIdParamType, NoAllocTag)
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
        return createMsgInternal(id, idx, handler, Tag());
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

    mutable Alloc alloc_;
};


}  // namespace details

}  // namespace comms


