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

template <typename TMsgBase, typename TAllMessages, typename... TOptions>
class MsgFactoryGeneric : public MsgFactoryBase<TMsgBase, TAllMessages, TOptions>
{
    using Base = MsgFactoryBase<TMsgBase, TAllMessages, TOptions>;

public:
    using AllMessages = Base::AllMessages;
    MsgFactory()
    {
        initRegistry();
        GASSERT(
            std::is_sorted(registry_.begin(), registry_.end(),
                [](const FactoryMethod* methodPtr1, const FactoryMethod* methodPtr2) -> bool
                {
                    GASSERT(methodPtr1 != nullptr);
                    GASSERT(methodPtr2 != nullptr);
                    return methodPtr1->getId() < methodPtr2->getId();
                }));
    }

private:
    using FactoryMethod = Base::FactoryMethod;

    static_assert(comms::util::IsTuple<AllMessages>::Value,
        "TAllMessages is expected to be a tuple.");

    static const std::size_t NumOfMessages =
        std::tuple_size<AllMessages>::value;

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


    class CompWrapper
    {
    public:

        CompWrapper(MsgIdParamType id)
          : m_id(id)
        {
        }

        CompWrapper(const FactoryMethod* method)
          : m_id(method->getId())
        {
        }


        MsgIdParamType getId() const
        {
            return m_id;
        }

    private:
        MsgIdType m_id;
    };

    void initRegistry()
    {
        util::tupleForEachType<AllMessages>(MsgFactoryCreator(registry_));
    }

    MethodsRegistry registry_;
};

} // namespace details

} // namespace comms
