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

template <bool TAllSorted, bool TMoreThanOne, typename...>
class AllMessagesStrongSortedCheckHelper;

template <bool TMoreThanOne, typename... TParams>
class AllMessagesStrongSortedCheckHelper<false, TMoreThanOne, TParams...>
{
public:    
    template <typename...>
    using Type = std::false_type;
};

template <typename... TParams>
class AllMessagesStrongSortedCheckHelper<true, false, TParams...>
{
public:    
    template <typename...>
    using Type = std::true_type;
};

template <typename... TParams>
class AllMessagesStrongSortedCheckHelper<true, true, TParams...>
{
public:
    template <typename TMsg1, typename TMsg2, typename... TRest>
    using Type = 
        typename comms::util::Conditional<
            (TMsg1::ImplOptions::MsgId < TMsg2::ImplOptions::MsgId)
        >::template Type<
            typename AllMessagesStrongSortedCheckHelper<true, (0U < sizeof...(TRest))>::template Type<TMsg2, TRest...>,
            std::false_type
        >;
};

template <bool TAllSorted, bool TMoreThanOne, typename...>
class AllMessagesWeakSortedCheckHelper;

template <bool TMoreThanOne, typename... TParams    >
class AllMessagesWeakSortedCheckHelper<false, TMoreThanOne, TParams...>
{
public:    
    template <typename... TRest>
    using Type = std::false_type;
};

template <typename... TParams>
class AllMessagesWeakSortedCheckHelper<true, false, TParams...>
{
public:    
    template <typename... TRest>
    using Type = std::true_type;
};

template <typename... TParams>
class AllMessagesWeakSortedCheckHelper<true, true, TParams...>
{
public:
    template <typename TMsg1, typename TMsg2, typename... TRest>
    using Type = 
        typename comms::util::Conditional<
            (TMsg1::ImplOptions::MsgId <= TMsg2::ImplOptions::MsgId)
        >::template Type<
            typename AllMessagesWeakSortedCheckHelper<true, (0U < sizeof...(TRest))>::template Type<TMsg2, TRest...>,
            std::false_type
        >;
};

template <typename TAllMessages>
struct AllMessagesStrongSortedCheckHelperWrap;

template <typename... TAllMessages>
struct AllMessagesStrongSortedCheckHelperWrap<std::tuple<TAllMessages...> >
{
    static constexpr bool Value = 
        AllMessagesStrongSortedCheckHelper<
            (AllMessagesHaveStaticNumIdBoolType<TAllMessages...>::value),
            (1U < sizeof...(TAllMessages))
        >::template Type<TAllMessages...>::value;
};

template <typename TAllMessages>
struct AllMessagesWeakSortedCheckHelperWrap;

template <typename... TAllMessages>
struct AllMessagesWeakSortedCheckHelperWrap<std::tuple<TAllMessages...> >
{
    static constexpr bool Value = 
        AllMessagesWeakSortedCheckHelper<
            (AllMessagesHaveStaticNumIdBoolType<TAllMessages...>::value),
            (1U < sizeof...(TAllMessages))
        >::template Type<TAllMessages...>::value;
};

template <typename TAllMessages>
constexpr bool allMessagesAreStrongSorted()
{
    return AllMessagesStrongSortedCheckHelperWrap<TAllMessages>::Value;
}

template <typename TAllMessages>
constexpr bool allMessagesAreWeakSorted()
{
    return AllMessagesWeakSortedCheckHelperWrap<TAllMessages>::Value;
}

} // namespace details

} // namespace comms