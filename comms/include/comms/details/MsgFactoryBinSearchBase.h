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

#include "MsgFactoryBase.h"

namespace comms
{

namespace details
{

struct MsgFactoryStaticNumIdCheckHelper
{
    template <typename TMessage>
    constexpr bool operator()(bool value)
    {
        return value && TMessage::ImplOptions::HasStaticMsgId;
    }
};

template <typename TAllMessages>
constexpr bool msgFactoryAllHaveStaticNumId()
{
    return comms::util::tupleTypeAccumulate<TAllMessages>(true, MsgFactoryStaticNumIdCheckHelper());
}

template <bool TStrong, typename... TMessages>
struct MsgFactoryBinSearchSortedCheckHelper;

template <
    bool TStrong,
    typename TMessage1,
    typename TMessage2,
    typename TMessage3,
    typename... TRest>
struct MsgFactoryBinSearchSortedCheckHelper<TStrong, TMessage1, TMessage2, TMessage3, TRest...>
{
    static const bool Value =
        MsgFactoryBinSearchSortedCheckHelper<TStrong, TMessage1, TMessage2>::Value &&
        MsgFactoryBinSearchSortedCheckHelper<TStrong, TMessage2, TMessage3, TRest...>::Value;
};

template <bool TStrong, typename TMessage1, typename TMessage2>
struct MsgFactoryBinSearchSortedCheckHelper<TStrong, TMessage1, TMessage2>
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

    static_assert(TMessage1::ImplOptions::HasStaticMsgId, "Message is expected to provide status numeric ID");
    static_assert(TMessage2::ImplOptions::HasStaticMsgId, "Message is expected to provide status numeric ID");

public:
    ~MsgFactoryBinSearchSortedCheckHelper() = default;
    static const bool Value = isLess<TMessage1, TMessage2>();
};

template <bool TStrong, typename TMessage1>
struct MsgFactoryBinSearchSortedCheckHelper<TStrong, TMessage1>
{
    static_assert(!comms::util::isTuple<TMessage1>(), "TMessage1 mustn't be tuple");
    static const bool Value = true;
};

template <bool TStrong>
struct MsgFactoryBinSearchSortedCheckHelper<TStrong>
{
    static const bool Value = true;
};

template <bool TStrong, typename... TMessages>
struct MsgFactoryBinSearchSortedCheckHelper<TStrong, std::tuple<TMessages...> >
{
    static const bool Value = MsgFactoryBinSearchSortedCheckHelper<TStrong, TMessages...>::Value;
};

template <typename TAllMessages>
constexpr bool msgFactoryAreAllStrongSorted()
{
    return MsgFactoryBinSearchSortedCheckHelper<true, TAllMessages>::Value;
}

template <typename TAllMessages>
constexpr bool msgFactoryAreAllWeakSorted()
{
    return MsgFactoryBinSearchSortedCheckHelper<false, TAllMessages>::Value;
}

template <typename TMsgBase, typename TAllMessages, typename... TOptions>
class MsgFactoryBinSearchBase : public MsgFactoryBase<TMsgBase, TAllMessages, TOptions...>
{
    using Base = MsgFactoryBase<TMsgBase, TAllMessages, TOptions...>;

public:
    using AllMessages = typename Base::AllMessages;
    using MsgPtr = typename Base::MsgPtr;
    using MsgIdParamType = typename Base::MsgIdParamType;
    using MsgIdType = typename Base::MsgIdType;

    MsgFactoryBinSearchBase()
    {
        initRegistry();
        checkSorted(SortedCheckTag());
    }

protected:

    static_assert(comms::util::IsTuple<AllMessages>::Value,
        "TAllMessages is expected to be a tuple.");

    static const std::size_t NumOfMessages =
        std::tuple_size<AllMessages>::value;

    using FactoryMethod = typename Base::FactoryMethod;
    using MethodsRegistry = std::array<const FactoryMethod*, NumOfMessages>;

    MethodsRegistry& registry()
    {
        return registry_;
    }

    const MethodsRegistry& registry() const
    {
        return registry_;
    }

private:

    struct CompileTimeSorted {};
    struct RunTimeSorted {};

    using SortedCheckTag =
        typename std::conditional<
            msgFactoryAllHaveStaticNumId<AllMessages>(),
            CompileTimeSorted,
            RunTimeSorted
        >::type;

    template <typename TMessage>
    using NumIdFactoryMethod = typename Base::template NumIdFactoryMethod<TMessage>;

    template <typename TMessage>
    using GenericFactoryMethod = typename Base::template GenericFactoryMethod<TMessage>;

    class MsgFactoryCreator
    {
    public:
        MsgFactoryCreator(MethodsRegistry& registry)
          : registry_(registry)
        {
        }

        template <typename TMessage>
        void operator()()
        {
            using Tag = typename std::conditional<
                TMessage::ImplOptions::HasStaticMsgId,
                StaticNumericIdTag,
                OtherIdTag
            >::type;

            registry_[idx_] = createFactory<TMessage>(Tag());
            ++idx_;
        }

    private:
        struct StaticNumericIdTag {};
        struct OtherIdTag {};

        template <typename TMessage>
        static const FactoryMethod* createFactory(StaticNumericIdTag)
        {
            static const NumIdFactoryMethod<TMessage> Factory;
            return &Factory;
        }

        template <typename TMessage>
        const FactoryMethod* createFactory(OtherIdTag)
        {
            static const GenericFactoryMethod<TMessage> Factory;
            return &Factory;
        }

        MethodsRegistry& registry_;
        unsigned idx_ = 0;
    };

    void initRegistry()
    {
        util::tupleForEachType<AllMessages>(MsgFactoryCreator(registry_));
    }

    void checkSorted(CompileTimeSorted)
    {
        static_assert(msgFactoryAreAllWeakSorted<AllMessages>(),
                "The messages in AllMessages tuple are expected to be sorted");
    }

    void checkSorted(RunTimeSorted)
    {
        GASSERT(
            std::is_sorted(registry_.begin(), registry_.end(),
                [](const FactoryMethod* methodPtr1, const FactoryMethod* methodPtr2) -> bool
                {
                    GASSERT(methodPtr1 != nullptr);
                    GASSERT(methodPtr2 != nullptr);
                    return methodPtr1->getId() < methodPtr2->getId();
                }));
    }

    MethodsRegistry registry_;
};

} // namespace details

} // namespace comms
