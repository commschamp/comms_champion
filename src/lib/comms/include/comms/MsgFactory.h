//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

/// @brief Message factory class.
/// @details It is responsible to create message objects given the ID of the
///     message. This class @b DOESN'T use dynamic memory allocation to store its
///     internal data structures, hence can be used in any bare-metal and other
///     embedded environment.
/// @tparam TMsgBase Common base class for all the messages, smart pointer to
///     this type is returned when allocation of specify message is requested.
/// @tparam TAllMessages All custom message types, that this factory is capable
///     of creating, bundled in std::tuple<>. The message types must be sorted
///     based on their IDs. Different variants of the same message (reporting
///     same ID, but implemented as different classes) are also supported. However
///     they must follow one another in this std::tuple, i.e. be sorted.
/// @tparam TOptions Zero or more options. The supported options are:
///     @li comms::option::InPlaceAllocation - Option to specify that custom
///         message objects are @b NOT allocated using dynamic memory, instead
///         an uninitialised area of memory in private members is used to contain
///         any type of custom message (provided with TAllMessages template parameter) and
///         placement "new" operator is used to initialise requested message in
///         this area.
///         The allocated message objects are returned from createMsg() function
///         wrapped in the smart pointer (variant of std::unique_ptr). If
///         comms::option::InPlaceAllocation option is used, then the smart pointer
///         definition contains custom deleter, which will explicitly invoke
///         destructor of the message when the smart pointer is out of scope. It
///         means that it is @b NOT possible to create new message with this factory
///         if previously allocated one wasn't destructed yet.
///         If comms::option::InPlaceAllocation option is NOT used, than the
///         requested message objects are allocated using dynamic memory and
///         returned wrapped in std::unique_ptr without custom deleter.
/// @pre TMsgBase is a base class for all the messages in TAllMessages.
/// @pre Message type is TAllMessages must be sorted based on their IDs.
/// @pre If comms::option::InPlaceAllocation option is provided, only one custom
///     message can be allocated. The next one can be allocated only after previous
///     message has been destructed.
template <typename TMsgBase, typename TAllMessages, typename... TOptions>
class MsgFactory : public details::MsgFactoryBase<TMsgBase, TAllMessages, TOptions...>
{
    typedef details::MsgFactoryBase<TMsgBase, TAllMessages, TOptions...> Base;

    static_assert(TMsgBase::InterfaceOptions::HasMsgIdType,
        "Usage of MsgFactory requires Message interface to provide ID type. "
        "Use comms::option::MsgIdType option in message interface type definition.");
public:

    /// @brief Type of the common base class of all the messages.
    typedef TMsgBase Message;

    /// @brief Type of the message ID when passed as a parameter.
    typedef typename Message::MsgIdParamType MsgIdParamType;

    /// @brief Type of the message ID.
    typedef typename Message::MsgIdType MsgIdType;

    /// @brief Smart pointer to @ref Message which holds allocated message object.
    /// @details It is a variant of std::unique_ptr, based on whether
    ///     comms::option::InPlaceAllocation option was used.
    typedef typename Base::MsgPtr MsgPtr;

    /// @brief All messages provided as template parameter to this class.
    typedef typename Base::AllMessages AllMessages;

    /// @brief Constructor.
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

    /// @brief Create message object given the ID of the message.
    /// @param id ID of the message.
    /// @param idx Relative index of the message with the same ID. In case
    ///     protocol implementation contains multiple distinct message types
    ///     that report same ID value, it must be possible to choose the
    ///     relative index of such message from the first message type reporting
    ///     the same ID. This parameter provides such an ability. However,
    ///     most protocols will implement single message class for single ID.
    ///     For such implementations, use default value of this parameter.
    /// @return Smart pointer (variant of std::unique_ptr) to @ref Message type,
    ///     which is a common base class of all the messages (provided as
    ///     first template parameter to this class). If comms::option::InPlaceAllocation
    ///     option was used and previously allocated message wasn't de-allocated
    ///     yet, the empty (null) pointer will be returned.
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

    /// @brief Get number of message types from @ref AllMessages, that have the specified ID.
    /// @param id ID of the message.
    /// @return Number of message classes that report same ID.
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
    friend class NumIdFactoryMethod;

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
    friend class GenericFactoryMethod;

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
        MsgIdType m_id;
    };

    void initRegistry()
    {
        util::tupleForEachType<AllMessages>(MsgFactoryCreator(registry_));
    }

    MethodsRegistry registry_;
};


}  // namespace comms

