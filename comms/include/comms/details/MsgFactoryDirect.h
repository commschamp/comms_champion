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
using MsgFactoryLastMessageType =
    typename std::tuple_element<std::tuple_size<TAllMessages>::value - 1, TAllMessages>::type;

template <std::size_t TSize, typename TAllMessages>
struct MsgFactoryDirectNumOfRegElements
{
    using LastMessage =
        typename std::tuple_element<TSize - 1, TAllMessages>::type;

    static const std::size_t Value =
            static_cast<std::size_t>(LastMessage::ImplOptions::MsgId) + 1U;
};

template <typename TAllMessages>
struct MsgFactoryDirectNumOfRegElements<0U, TAllMessages>
{
    static const size_t Value = 0U;
};

template <typename TAllMessages>
constexpr std::size_t msgFactoryDirectNumOfRegElements()
{
    return MsgFactoryDirectNumOfRegElements<std::tuple_size<TAllMessages>::value, TAllMessages>::Value;
}


template <typename TMsgBase, typename TAllMessages, typename... TOptions>
class MsgFactoryDirect : public MsgFactoryBase<TMsgBase, TAllMessages, TOptions...>
{
    using BaseImpl = MsgFactoryBase<TMsgBase, TAllMessages, TOptions...>;

public:
    using AllMessages = typename BaseImpl::AllMessages;
    using MsgPtr = typename BaseImpl::MsgPtr;
    using MsgIdParamType = typename BaseImpl::MsgIdParamType;
    using MsgIdType = typename BaseImpl::MsgIdType;

    MsgFactoryDirect()
    {
        initRegistry();
    }

    MsgPtr createMsg(MsgIdParamType id, unsigned idx = 0) const
    {
        if (0 < idx) {
            return MsgPtr();
        }

        auto method = getMethod(id);
        if (method == nullptr) {
            return MsgPtr();
        }

        return method->create(*this);
    }

    std::size_t msgCount(MsgIdParamType id) const
    {
        auto method = getMethod(id);
        if (method == nullptr) {
            return 0U;
        }
        return 1U;
    }

    static constexpr bool hasUniqueIds()
    {
        return true;
    }

private:

    static_assert(comms::util::IsTuple<AllMessages>::Value,
        "TAllMessages is expected to be a tuple.");

    static const std::size_t NumOfMessages =
            msgFactoryDirectNumOfRegElements<TAllMessages>();

    template <typename TMessage>
    using NumIdFactoryMethod = typename BaseImpl::template NumIdFactoryMethod<TMessage>;

    using FactoryMethod = typename BaseImpl::FactoryMethod;
    using MethodsRegistry = std::array<const FactoryMethod*, NumOfMessages>;

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
            static const std::size_t Idx =
                    static_cast<std::size_t>(TMessage::ImplOptions::MsgId);

            static_assert(Idx < NumOfMessages, "Invalid message id");

            static const NumIdFactoryMethod<TMessage> Factory;
            COMMS_ASSERT(registry_[Idx] == nullptr);
            registry_[Idx] = &Factory;
        }

    private:
        MethodsRegistry& registry_;
    };

    void initRegistry()
    {
        std::fill(registry_.begin(), registry_.end(), nullptr);
        util::tupleForEachType<AllMessages>(MsgFactoryCreator(registry_));
    }

    const FactoryMethod* getMethod(MsgIdParamType id) const
    {
        auto elemIdx = static_cast<std::size_t>(id);
        if (registry_.size() <= elemIdx){
            return nullptr;
        }

        return registry_[elemIdx];
    }

    MethodsRegistry registry_;
};

} // namespace details

} // namespace comms
