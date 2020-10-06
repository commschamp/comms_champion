//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "comms/Message.h"
#include "comms/details/detect.h"
#include "comms/details/tag.h"
#include "comms/util/type_traits.h"

namespace comms
{

namespace details
{

template <typename...>
class WriteIteratorHelper
{
    template <typename... TParams>
    using HasWriteIterTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using MsgPointerTag = comms::details::tag::Tag2<>;    

    template <typename... TParams>
    using CastTag = comms::details::tag::Tag3<>;         

    template <typename TMsg>
    using MsgObjTag = 
        typename comms::util::LazyShallowConditional<
            TMsg::hasWrite()
        >::template Type<
            HasWriteIterTag,
            CastTag
        >;

    template <typename TMsg>
    using MsgPtrOrCastTag = 
        typename comms::util::LazyShallowConditional<
            hasElementType<TMsg>()
        >::template Type<
            MsgPointerTag,
            CastTag
        >;    

    template <typename TMsg>
    using Tag = 
        typename comms::util::LazyShallowConditional<
            comms::isMessage<TMsg>()
        >::template Type<
            MsgObjTag,
            MsgPtrOrCastTag,
            TMsg
        >;

    template <typename TMsg, typename TIter, typename... TParams>
    static auto getInternal(TIter&& iter, HasWriteIterTag<TParams...>) -> typename TMsg::WriteIterator
    {
        static_assert(std::is_convertible<typename std::decay<decltype(iter)>::type, typename TMsg::WriteIterator>::value,
            "Provided iterator is not convertible to read iterator type used by message interface");
        return typename TMsg::WriteIterator(std::forward<TIter>(iter));
    }    

    template <typename TMsg, typename TIter, typename... TParams>
    static auto getInternal(TIter&& iter, CastTag<TParams...>) -> decltype(std::forward<TIter>(iter))
    {
        return std::forward<TIter>(iter);
    }     

    template <typename TMsg, typename TIter, typename... TParams>
    static auto getInternal(TIter&& iter, MsgPointerTag<TParams...>) -> 
        decltype(getInternal<typename TMsg::element_type>(std::forward<TIter>(iter), Tag<typename TMsg::element_type>()))
    {
        return getInternal<typename TMsg::element_type>(std::forward<TIter>(iter), Tag<typename TMsg::element_type>());
    }  

public:
    template <typename TMsg, typename TIter>
    static auto get(TIter&& iter) -> decltype(getInternal<TMsg>(std::forward<TIter>(iter), Tag<TMsg>()))
    {
        return getInternal<TMsg>(std::forward<TIter>(iter), Tag<TMsg>());
    }
};

} // namespace details

} // namespace comms
