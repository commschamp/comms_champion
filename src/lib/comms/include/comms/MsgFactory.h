//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

#include <type_traits>
#include <algorithm>

#include "details/MsgFactoryBase.h"

namespace comms
{

template <typename TMsgBase, typename TAllMessages, typename... TOptions>
class MsgFactory : public details::MsgFactoryBase<TMsgBase, TAllMessages, TOptions...>
{
    typedef details::MsgFactoryBase<TMsgBase, TAllMessages, TOptions...> Base;
public:

    typedef TMsgBase Message;
    typedef typename Message::MsgIdParamType MsgIdParamType;
    typedef typename Base::MsgPtr MsgPtr;
    typedef typename Base::AllMessages AllMessages;

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

    MsgPtr createMsg(MsgIdParamType id, unsigned idx = 0) const
    {
        auto range =
            std::equal_range(
                registry_.begin(), registry_.end(), id,
                [](const CompWrapper& idWrapper1, const CompWrapper& idWrapper2) -> bool
                {
                    return idWrapper1.getId() < idWrapper2.getId();
                });

        auto dist = static_cast<unsigned>(std::distance(range.first, range.second));
        if (dist <= idx) {
            return MsgPtr();
        }

        auto iter = range.first + idx;
        GASSERT(*iter);
        return (*iter)->create(*this);
    }

    std::size_t msgCount(MsgIdParamType id) const
    {
        auto range =
            std::equal_range(
                registry_.begin(), registry_.end(), id,
                [](const CompWrapper& idWrapper1, const CompWrapper& idWrapper2) -> bool
                {
                    return idWrapper1.getId() < idWrapper2.getId();
                });

        return static_cast<std::size_t>(std::distance(range.first, range.second));
    }

private:
    class FactoryMethod
    {
    public:
        virtual ~FactoryMethod() {};

        virtual MsgIdParamType getId() const
        {
            return getIdImpl();
        }

        MsgPtr create(const MsgFactory& factory) const
        {
            return createImpl(factory);
        }

    protected:
        FactoryMethod() = default;

        virtual MsgIdParamType getIdImpl() const = 0;
        virtual MsgPtr createImpl(const MsgFactory& factory) const = 0;
    };

    template <typename TMessage>
    class NumIdFactoryMethod : public FactoryMethod
    {
    public:
        typedef TMessage Message;
        static const auto MsgId = Message::MsgId;
    protected:
        virtual MsgIdParamType getIdImpl() const
        {
            return static_cast<MsgIdParamType>(MsgId);
        }

        virtual MsgPtr createImpl(const MsgFactory& factory) const
        {
            return factory.template allocMsg<Message>();
        }
    };

    template <typename TMessage>
    friend class MsgFactory<TMsgBase, TAllMessages, TOptions...>::NumIdFactoryMethod;

    template <typename TMessage>
    class GenericFactoryMethod : public FactoryMethod
    {
    public:
        typedef TMessage Message;

        GenericFactoryMethod() : id_(Message().getId()) {}

    protected:

        virtual MsgIdParamType getIdImpl() const
        {
            return id_;
        }

        virtual MsgPtr createImpl(const MsgFactory& factory) const
        {
            return factory.template allocMsg<Message>();
        }

    private:
        typename Message::MsgIdType id_;
    };

    template <typename TMessage>
    friend class MsgFactory<TMsgBase, TAllMessages, TOptions...>::GenericFactoryMethod;

    static_assert(comms::util::IsTuple<AllMessages>::Value,
        "TAllMessages is expected to be a tuple.");

    static const std::size_t NumOfMessages =
        std::tuple_size<AllMessages>::value;

    typedef std::array<const FactoryMethod*, NumOfMessages> MethodsRegistry;

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
            typedef typename std::conditional<
                TMessage::ImplOptions::HasStaticMsgId,
                StaticNumericIdTag,
                OtherIdTag
            >::type Tag;

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
        MsgIdParamType m_id;
    };

    void initRegistry()
    {
        util::tupleForEachType<AllMessages>(MsgFactoryCreator(registry_));
    }

    MethodsRegistry registry_;
};


}  // namespace comms

