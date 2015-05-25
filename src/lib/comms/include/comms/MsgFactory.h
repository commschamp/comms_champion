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

namespace details
{

template <std::size_t TSize>
struct MsgFactoryCreateHelper
{
    template <typename TAllMessages,
              template <class> class TMethod,
              typename TMethodsRegistry>
    static void create(TMethodsRegistry& registry)
    {
        static const std::size_t Idx = TSize - 1;
        MsgFactoryCreateHelper<Idx>::template
                            create<TAllMessages, TMethod>(registry);

        typedef typename std::tuple_element<Idx, TAllMessages>::type Message;
        static TMethod<Message> method;
        registry[Idx] = &method;
    }
};

template <>
struct MsgFactoryCreateHelper<0>
{
    template <typename TAllMessages,
        template <class> class TMethod,
        typename TMethodsRegistry>
    static void create(TMethodsRegistry& registry)
    {
        static_cast<void>(registry);
        typedef typename std::decay<decltype(registry)>::type RegistryType;
        typedef typename RegistryType::value_type MethodPtr;
        GASSERT(
            std::is_sorted(registry.begin(), registry.end(),
                [](MethodPtr methodPtr1, MethodPtr methodPtr2) -> bool
                {
                    return methodPtr1->getId() < methodPtr2->getId();
                }));
    }
};

template <typename TOption>
struct MsgFactoryIsNumIdImplOpt
{
    static const bool Value = false;
};

template <long long int TId>
struct MsgFactoryIsNumIdImplOpt<comms::option::StaticNumIdImpl<TId> >
{
    static const bool Value = true;
};

template <typename TOptions>
struct MsgFactoryHasNumIdImplOpt;

template <typename TFirst, typename... TRest>
struct MsgFactoryHasNumIdImplOpt<std::tuple<TFirst, TRest...> >
{
    static const bool Value =
        MsgFactoryIsNumIdImplOpt<TFirst>::Value ||
        MsgFactoryHasNumIdImplOpt<std::tuple<TRest...> >::Value;
};

template <>
struct MsgFactoryHasNumIdImplOpt<std::tuple<> >
{
    static const bool Value = false;
};

template <typename TMessage>
struct MsgFactoryHasStaticId
{
    static const bool Value = MsgFactoryHasNumIdImplOpt<typename TMessage::AllOptions>::Value;
};


}  // namespace details

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
        initRegistryInternal(MethodTypeTag());
    }

    MsgPtr createMsg(MsgIdParamType id) const
    {
        auto iter =
            std::lower_bound(
                registry_.begin(), registry_.end(),
                [](FactoryMethod* method1, FactoryMethod* method2) -> bool
                {
                    GASSERT(method1 != nullptr);
                    GASSERT(method2 != nullptr);
                    return method1->getId() < method2->getId();
                });

        if ((iter == registry_.end()) ||
            ((*iter)->getId() != id)) {
            return MsgPtr();
        }

        return (*iter)->create(*this);
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

        MsgPtr create(MsgFactory& factory) const
        {
            return createImpl(factory);
        }

    protected:
        FactoryMethod() = default;

        virtual MsgIdParamType getIdImpl() const = 0;
        virtual MsgPtr createImpl(MsgFactory& factory) const = 0;
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

        virtual MsgPtr createImpl(MsgFactory& factory) const
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

        virtual MsgPtr createImpl(MsgFactory& factory) const
        {
            return factory.template allocMsg<Message>();
        }

    private:
        typename Message::MsgIdType id_;
    };

    template <typename TMessage>
    friend class MsgFactory<TMsgBase, TAllMessages, TOptions...>::GenericFactoryMethod;

    struct StaticNumericIdTag {};
    struct OtherIdTag {};

    typedef typename std::conditional<
        details::MsgFactoryHasStaticId<Message>::Value,
        StaticNumericIdTag,
        OtherIdTag
    >::type MethodTypeTag;

    static_assert(comms::util::IsTuple<AllMessages>::Value,
        "TAllMessages is expected to be a tuple.");

    static const std::size_t NumOfMessages =
        std::tuple_size<AllMessages>::value;

    typedef std::array<FactoryMethod*, NumOfMessages> MethodsRegistry;

    void initRegistryInternal(StaticNumericIdTag)
    {
        details::MsgFactoryCreateHelper<NumOfMessages>::template
                        create<AllMessages, NumIdFactoryMethod>(registry_);
    }

    void initRegistryInternal(OtherIdTag)
    {
        details::MsgFactoryCreateHelper<NumOfMessages>::template
                        create<AllMessages, GenericFactoryMethod>(registry_);
    }

    MethodsRegistry registry_;
};


}  // namespace comms

