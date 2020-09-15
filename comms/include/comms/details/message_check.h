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

#if COMMS_IS_MSVC_2017_OR_BELOW    

template <typename... TMessages>
using AllMessagesHaveStaticNumIdBoolType = 
    std::integral_constant<
        bool, 
        comms::util::tupleTypeAccumulate<std::tuple<TMessages...> >(true, MessageStaticNumIdCheckHelper<>())
    >;

#else // #if COMMS_IS_MSVC_2017_OR_BELOW    

template <typename... TMessages>
using AllMessagesHaveStaticNumIdBoolType = 
    typename comms::util::Accumulate<>::template Type<
        comms::util::MessageCheckHasStaticId,
        comms::util::LogicalAndBinaryOp,
        std::true_type,
        TMessages...
    >;

#endif // #if COMMS_IS_MSVC_2017_OR_BELOW    

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

// template <bool TAllStaticIds, bool TStrong, typename... TMessages>
// struct AllMessagesSortedCheckHelper;

// template <bool TStrong, typename... TMessages>
// struct AllMessagesSortedCheckHelper<false, TStrong, TMessages...>
// {
//     static const bool Value = false;
// };

// template <
//     bool TStrong,
//     typename TMessage1,
//     typename TMessage2,
//     typename TMessage3,
//     typename... TRest>
// struct AllMessagesSortedCheckHelper<true, TStrong, TMessage1, TMessage2, TMessage3, TRest...>
// {
//     static const bool Value =
//         AllMessagesSortedCheckHelper<true, TStrong, TMessage1, TMessage2>::Value &&
//         AllMessagesSortedCheckHelper<true, TStrong, TMessage2, TMessage3, TRest...>::Value;
// };

// template <bool TStrong, typename TMessage1, typename TMessage2>
// struct AllMessagesSortedCheckHelper<true, TStrong, TMessage1, TMessage2>
// {
// private:
//     struct StrongTag {};
//     struct WeakTag {};
//     using Tag =
//         typename comms::util::Conditional<
//             TStrong
//         >::template Type<
//             StrongTag,
//             WeakTag
//         >;

//     template <typename T1, typename T2>
//     static constexpr bool isLess(StrongTag)
//     {
//         return T1::ImplOptions::MsgId < T2::ImplOptions::MsgId;
//     }

//     template <typename T1, typename T2>
//     static constexpr bool isLess(WeakTag)
//     {
//         return T1::ImplOptions::MsgId <= T2::ImplOptions::MsgId;
//     }

//     template <typename T1, typename T2>
//     static constexpr bool isLess()
//     {
//         return isLess<T1, T2>(Tag());
//     }

//     static_assert(messageHasStaticNumId<TMessage1>(), "Message is expected to provide status numeric ID");
//     static_assert(messageHasStaticNumId<TMessage2>(), "Message is expected to provide status numeric ID");

// public:
//     ~AllMessagesSortedCheckHelper() noexcept = default;
//     static const bool Value = isLess<TMessage1, TMessage2>();
// };

// template <bool TStrong, typename TMessage1>
// struct AllMessagesSortedCheckHelper<true, TStrong, TMessage1>
// {
//     static_assert(!comms::util::isTuple<TMessage1>(), "TMessage1 mustn't be tuple");
//     static const bool Value = true;
// };

// template <bool TStrong>
// struct AllMessagesSortedCheckHelper<true, TStrong>
// {
//     static const bool Value = true;
// };

// template <bool TStrong, typename... TMessages>
// struct AllMessagesSortedCheckHelper<true, TStrong, std::tuple<TMessages...> >
// {
//     static const bool Value = AllMessagesSortedCheckHelper<true, TStrong, TMessages...>::Value;
// };

template <typename TAllMessages>
constexpr bool allMessagesAreStrongSorted()
{
    return AllMessagesSortedCheckHelperWrap<std::true_type, TAllMessages>::Value;
    //return AllMessagesSortedCheckHelper<allMessagesHaveStaticNumId<TAllMessages>(), true, TAllMessages>::Value;
}

template <typename TAllMessages>
constexpr bool allMessagesAreWeakSorted()
{
    return AllMessagesSortedCheckHelperWrap<std::false_type, TAllMessages>::Value;
    //return AllMessagesSortedCheckHelper<allMessagesHaveStaticNumId<TAllMessages>(), false, TAllMessages>::Value;
}

} // namespace details

} // namespace comms