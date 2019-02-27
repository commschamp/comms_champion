//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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

/// @file
/// Contains extra logic to help with dispatching message types and objects

#pragma once

#include <type_traits>

#include "comms/Assert.h"
#include "comms/util/Tuple.h"

namespace comms
{

namespace details
{

template <typename TMessage, bool TIsMessageBase>
struct MessageHasStaticNumId;

template <typename TMessage>
struct MessageHasStaticNumId<TMessage, true>
{
    static const bool Value = TMessage::ImplOptions::HasStaticMsgId;
};

template <typename TMessage>
struct MessageHasStaticNumId<TMessage, false>
{
    static const bool Value = false;
};

struct MessageStaticNumIdCheckHelper
{
    template <typename TMessage>
    constexpr bool operator()(bool value) const
    {
        return value && MessageHasStaticNumId<TMessage, comms::isMessageBase<TMessage>()>::Value;
    }
};

template <typename TAllMessages>
constexpr bool allMessagesHaveStaticNumId()
{
    return comms::util::tupleTypeAccumulate<TAllMessages>(true, MessageStaticNumIdCheckHelper());
}

template <typename TMessage>
constexpr bool messageHasStaticNumId()
{
    return MessageHasStaticNumId<TMessage, comms::isMessageBase<TMessage>()>::Value;
}

template <bool TAllStaticIds, bool TStrong, typename... TMessages>
struct AllMessagesSortedCheckHelper;

template <bool TStrong, typename... TMessages>
struct AllMessagesSortedCheckHelper<false, TStrong, TMessages...>
{
    static const bool Value = false;
};

template <
    bool TStrong,
    typename TMessage1,
    typename TMessage2,
    typename TMessage3,
    typename... TRest>
struct AllMessagesSortedCheckHelper<true, TStrong, TMessage1, TMessage2, TMessage3, TRest...>
{
    static const bool Value =
        AllMessagesSortedCheckHelper<true, TStrong, TMessage1, TMessage2>::Value &&
        AllMessagesSortedCheckHelper<true, TStrong, TMessage2, TMessage3, TRest...>::Value;
};

template <bool TStrong, typename TMessage1, typename TMessage2>
struct AllMessagesSortedCheckHelper<true, TStrong, TMessage1, TMessage2>
{
private:
    struct StrongTag {};
    struct WeakTag {};
    using Tag =
        typename std::conditional<
            TStrong,
            StrongTag,
            WeakTag
        >::type;

    template <typename T1, typename T2>
    static constexpr bool isLess(StrongTag)
    {
        return T1::ImplOptions::MsgId < T2::ImplOptions::MsgId;
    }

    template <typename T1, typename T2>
    static constexpr bool isLess(WeakTag)
    {
        return T1::ImplOptions::MsgId <= T2::ImplOptions::MsgId;
    }

    template <typename T1, typename T2>
    static constexpr bool isLess()
    {
        return isLess<T1, T2>(Tag());
    }

    static_assert(messageHasStaticNumId<TMessage1>(), "Message is expected to provide status numeric ID");
    static_assert(messageHasStaticNumId<TMessage2>(), "Message is expected to provide status numeric ID");

public:
    ~AllMessagesSortedCheckHelper() noexcept = default;
    static const bool Value = isLess<TMessage1, TMessage2>();
};

template <bool TStrong, typename TMessage1>
struct AllMessagesSortedCheckHelper<true, TStrong, TMessage1>
{
    static_assert(!comms::util::isTuple<TMessage1>(), "TMessage1 mustn't be tuple");
    static const bool Value = true;
};

template <bool TStrong>
struct AllMessagesSortedCheckHelper<true, TStrong>
{
    static const bool Value = true;
};

template <bool TStrong, typename... TMessages>
struct AllMessagesSortedCheckHelper<true, TStrong, std::tuple<TMessages...> >
{
    static const bool Value = AllMessagesSortedCheckHelper<true, TStrong, TMessages...>::Value;
};

template <typename TAllMessages>
constexpr bool allMessagesAreStrongSorted()
{
    return AllMessagesSortedCheckHelper<allMessagesHaveStaticNumId<TAllMessages>(), true, TAllMessages>::Value;
}

template <typename TAllMessages>
constexpr bool allMessagesAreWeakSorted()
{
    return AllMessagesSortedCheckHelper<allMessagesHaveStaticNumId<TAllMessages>(), false, TAllMessages>::Value;
}

} // namespace details

} // namespace comms