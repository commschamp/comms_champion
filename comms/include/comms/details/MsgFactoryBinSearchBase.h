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

template <typename TAllMessages>
constexpr bool msgFactoryAreAllStrongSorted()
{
    return allMessagesAreStrongSorted<TAllMessages>();
}

template <typename TAllMessages>
constexpr bool msgFactoryAreAllWeakSorted()
{
    return allMessagesAreWeakSorted<TAllMessages>();
}

template <typename TMsgBase, typename TAllMessages, typename... TOptions>
class MsgFactoryBinSearchBase : public MsgFactoryBase<TMsgBase, TAllMessages, TOptions...>
{
    using BaseImpl = MsgFactoryBase<TMsgBase, TAllMessages, TOptions...>;

public:
    using AllMessages = typename BaseImpl::AllMessages;
    using MsgPtr = typename BaseImpl::MsgPtr;
    using MsgIdParamType = typename BaseImpl::MsgIdParamType;
    using MsgIdType = typename BaseImpl::MsgIdType;

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

    using FactoryMethod = typename BaseImpl::FactoryMethod;
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
    using NumIdFactoryMethod = typename BaseImpl::template NumIdFactoryMethod<TMessage>;

    template <typename TMessage>
    using GenericFactoryMethod = typename BaseImpl::template GenericFactoryMethod<TMessage>;

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
                details::msgFactoryMessageHasStaticNumId<TMessage>(),
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
        COMMS_ASSERT(
            std::is_sorted(registry_.begin(), registry_.end(),
                [](const FactoryMethod* methodPtr1, const FactoryMethod* methodPtr2) -> bool
                {
                    COMMS_ASSERT(methodPtr1 != nullptr);
                    COMMS_ASSERT(methodPtr2 != nullptr);
                    return methodPtr1->getId() < methodPtr2->getId();
                }));
    }

    MethodsRegistry registry_;
};

} // namespace details

} // namespace comms
