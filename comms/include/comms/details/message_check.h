//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// Contains extra logic to help with dispatching message types and objects

#pragma once

#include <type_traits>

#include "comms/CompileControl.h"
#include "comms/Assert.h"
#include "comms/util/Tuple.h"
#include "comms/util/type_traits.h"

namespace comms
{

namespace details
{

template <typename TMessage>
using MessageStaticNumIdBoolType = 
    typename comms::util::LazyDeepConditional<
        comms::isMessageBase<TMessage>()
    >::template Type<
        comms::util::MessageCheckHasStaticId,
        comms::util::FalseType,
        TMessage
    >;

template <typename...>
struct MessageStaticNumIdCheckHelper
{
    template <typename TMessage>
    constexpr bool operator()(bool value) const
    {
        return value && MessageStaticNumIdBoolType<TMessage>::value;
    }
};    

template <typename... TMessages>
using AllMessagesHaveStaticNumIdBoolType = 
    std::integral_constant<
        bool, 
        comms::util::tupleTypeAccumulate<std::tuple<TMessages...> >(true, MessageStaticNumIdCheckHelper<>())
    >;

template <typename TAllMessages>
struct AllMessagesHaveStaticNumIdCheckHelper;

template <typename... TMessages>
struct AllMessagesHaveStaticNumIdCheckHelper<std::tuple<TMessages...> >
{
    static constexpr bool Value = AllMessagesHaveStaticNumIdBoolType<TMessages...>::value;
};

template <typename TAllMessages>
constexpr bool allMessagesHaveStaticNumId()
{
    return AllMessagesHaveStaticNumIdCheckHelper<TAllMessages>::Value;
}

template <typename TMessage>
constexpr bool messageHasStaticNumId()
{
    return MessageStaticNumIdBoolType<TMessage>::value;
}

template <bool THasElems>
class AllMessagesSortedCheckHelper;

template <typename...>
struct AllMessagesSortedCheckHelperInvoke
{
    template <typename TStrong, typename... TRest>
    using Type = 
        typename AllMessagesSortedCheckHelper<
            (1U < sizeof...(TRest))
        >::template Type<
            TStrong,
            TRest...
        >;
};

template <bool THasElems>
class AllMessagesSortedCheckHelper
{
public:    
    template <typename TStrong, typename... TRest>
    using Type = AllMessagesHaveStaticNumIdBoolType<TRest...>;
};

template <>
class AllMessagesSortedCheckHelper<true>
{
    template <typename TMsg1, typename TMsg2, typename... TRest>
    using StrongType = 
        typename comms::util::LazyDeepConditional<
            TMsg2::ImplOptions::MsgId <= TMsg1::ImplOptions::MsgId
        >::template Type<
            comms::util::FalseType,
            AllMessagesSortedCheckHelperInvoke,
            std::true_type, TMsg2, TRest...
        >;

    template <typename TMsg1, typename TMsg2, typename... TRest>
    using WeakType = 
        typename comms::util::LazyDeepConditional<
            TMsg2::ImplOptions::MsgId < TMsg1::ImplOptions::MsgId
        >::template Type<
            comms::util::FalseType,
            AllMessagesSortedCheckHelperInvoke,
            std::false_type, TMsg2, TRest...
        >;

    template <typename TStrong, typename TMsg1, typename TMsg2, typename... TRest>
    using InnerType = 
        typename comms::util::LazyShallowConditional<
            TStrong::value
        >::template Type<
            StrongType,
            WeakType,
            TMsg1, TMsg2, TRest...
        >;   

    template <typename TStrong, typename TMsg1, typename TMsg2, typename... TRest>
    using InnerFalse = std::false_type;

public:    
    template <typename TStrong, typename TMsg1, typename TMsg2, typename... TRest>
    using Type = 
        typename comms::util::LazyShallowConditional<
            MessageStaticNumIdBoolType<TMsg1>::value && MessageStaticNumIdBoolType<TMsg2>::value
        >::template Type<
            InnerType,
            InnerFalse,
            TStrong, TMsg1, TMsg2, TRest...
        >;
};

template <typename TStrong, typename TAllMessages>
struct AllMessagesSortedCheckHelperWrap;

template <typename TStrong, typename... TAllMessages>
struct AllMessagesSortedCheckHelperWrap<TStrong, std::tuple<TAllMessages...> >
{
    using ValueType = 
        typename AllMessagesSortedCheckHelper<(1U < sizeof...(TAllMessages))>::
            template Type<TStrong, TAllMessages...>;

    static constexpr bool Value = ValueType::value;
};

template <typename TAllMessages>
constexpr bool allMessagesAreStrongSorted()
{
    return AllMessagesSortedCheckHelperWrap<std::true_type, TAllMessages>::Value;
}

template <typename TAllMessages>
constexpr bool allMessagesAreWeakSorted()
{
    return AllMessagesSortedCheckHelperWrap<std::false_type, TAllMessages>::Value;
}

} // namespace details

} // namespace comms