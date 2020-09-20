//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <limits>

#include "comms/Message.h"
#include "comms/MessageBase.h"
#include "comms/util/Tuple.h"
#include "comms/util/type_traits.h"
#include "comms/details/message_check.h"
#include "comms/CompileControl.h"
#include "comms/details/tag.h"

namespace comms
{

namespace details
{

template <typename...>
class DispatchMsgIdRetrieveHelper
{
public:

    template <typename TMessage>
    static typename TMessage::MsgIdParamType doGetId()
    {
        static_assert(TMessage::hasMsgIdType(), "Message interface class must define its id type");
        static_assert(!std::is_same<Tag<TMessage>, NoIdTag<> >::value, "Must be able to retrieve ID");

        return doGetIdInternal<TMessage>(Tag<TMessage>());
    }

private:
    template <typename... TParams>
    using HasStaticIdTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using HasDynamicIdTag = comms::details::tag::Tag2<>;

    template <typename... TParams>
    using NoIdTag = comms::details::tag::Tag3<>;

    template <typename TMessage, typename...>
    using DynamicCheckTag = 
        typename comms::util::LazyShallowConditional<
            comms::isMessage<TMessage>() && TMessage::hasGetId()
        >::template Type<
            HasDynamicIdTag,
            NoIdTag
        >;

    template <typename TMessage>
    using Tag = 
        typename comms::util::LazyShallowConditional<
            messageHasStaticNumId<TMessage>()
        >::template Type<
            HasStaticIdTag,
            DynamicCheckTag,
            TMessage
        >; 

    template <typename TMessage, typename... TParams>
    static typename TMessage::MsgIdParamType doGetIdInternal(HasStaticIdTag<TParams...>)
    {
        static_assert(comms::isMessageBase<TMessage>(), "Must be actual message");
        static_assert(messageHasStaticNumId<TMessage>(), "Message must define static numeric ID");

        return TMessage::doGetId();
    }      

    template <typename TMessage, typename... TParams>
    static typename TMessage::MsgIdParamType doGetIdInternal(HasDynamicIdTag<TParams...>)
    {
        static_assert(comms::isMessage<TMessage>(), "Must be actual message");
        static_assert(TMessage::hasGetId(), "Message interface must be able to return id polymorphically");

        static const typename TMessage::MsgIdType Id = TMessage().getId();
        return Id;
    }        
};

template <typename TMessage>
auto dispatchMsgGetMsgId() -> decltype(DispatchMsgIdRetrieveHelper<>::template doGetId<TMessage>())
{
    return DispatchMsgIdRetrieveHelper<>::template doGetId<TMessage>();
}

} // namespace details

} // namespace comms