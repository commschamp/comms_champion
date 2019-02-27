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
#include <limits>

#include "comms/Message.h"
#include "comms/MessageBase.h"
#include "comms/util/Tuple.h"
#include "comms/details/message_check.h"
#include "comms/CompileControl.h"

namespace comms
{

namespace details
{

template <typename TMessage>
class DispatchMsgIdRetrieveHelper
{
public:
    static_assert(TMessage::hasMsgIdType(), "Message interface class must define its id type");
    using MsgIdParamType = typename TMessage::MsgIdParamType;
    using MsgIdType = typename TMessage::MsgIdType;

    static MsgIdParamType doGetId()
    {
        using Tag = 
            typename std::conditional<
                messageHasStaticNumId<TMessage>(),
                HasStaticIdTag,
                typename std::conditional<
                    comms::isMessage<TMessage>() && TMessage::hasGetId(),
                    HasDynamicIdTag,
                    NoIdTag
                >::type
            >::type;

        static_assert(!std::is_same<Tag, NoIdTag>::value, "Must be able to retrieve ID");
        return doGetIdInternal(Tag());
    }

private:
    struct HasStaticIdTag {};
    struct HasDynamicIdTag {};
    struct NoIdTag {};
    
    static MsgIdParamType doGetIdInternal(HasStaticIdTag)
    {
        static_assert(comms::isMessageBase<TMessage>(), "Must be actual message");
        static_assert(messageHasStaticNumId<TMessage>(), "Message must define static numeric ID");

        return TMessage::doGetId();
    }      

    static MsgIdParamType doGetIdInternal(HasDynamicIdTag)
    {
        static_assert(comms::isMessage<TMessage>(), "Must be actual message");
        static_assert(TMessage::hasGetId(), "Message interface must be able to return id polymorphically");

        static const MsgIdType Id = TMessage().getId();
        return Id;
    }        
};

template <typename TMessage>
auto dispatchMsgGetMsgId() -> decltype(DispatchMsgIdRetrieveHelper<TMessage>::doGetId())
{
    return DispatchMsgIdRetrieveHelper<TMessage>::doGetId();
}

} // namespace details

} // namespace comms