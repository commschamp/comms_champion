//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include <cassert>
#include <type_traits>
#include <QtWidgets/QVBoxLayout>

#include "comms/comms.h"

#include "comms_champion/Message.h"
#include "comms_champion/MessageDisplayHandler.h"
#include "comms_champion/DefaultMessageWidget.h"

#include "comms_champion/field_wrapper/BasicIntValueWrapper.h"
#include "comms_champion/field_wrapper/BitmaskValueWrapper.h"
#include "comms_champion/field_wrapper/BasicEnumValueWrapper.h"
#include "comms_champion/field_wrapper/StringWrapper.h"
#include "comms_champion/field_wrapper/BitfieldWrapper.h"
#include "comms_champion/field_wrapper/UnknownValueWrapper.h"

namespace comms_champion
{

namespace details
{

struct BasicIntValueTag {};
struct BitmaskValueTag {};
struct BasicEnumValueTag {};
struct StringTag {};
struct BitfieldTag {};
struct UnknownValueTag {};

template <typename TField>
struct TagOf
{
    static_assert(!comms::field::isBasicIntValue<TField>(),
        "BasicIntValue is perceived as unknown type");
    static_assert(!comms::field::isBitmaskValue<TField>(),
        "BitmaskValue is perceived as unknown type");
    static_assert(!comms::field::isString<TField>(),
        "String is perceived as unknown type");
    typedef UnknownValueTag Type;
};

template <typename... TArgs>
struct TagOf<comms::field::BasicIntValue<TArgs...> >
{
    static_assert(
        comms::field::isBasicIntValue<comms::field::BasicIntValue<TArgs...> >(),
        "isBasicIntValue is supposed to return true");

    typedef BasicIntValueTag Type;
};

template <typename... TArgs>
struct TagOf<comms::field::BitmaskValue<TArgs...> >
{
    static_assert(
        comms::field::isBitmaskValue<comms::field::BitmaskValue<TArgs...> >(),
        "isBitmaskValue is supposed to return true");

    typedef BitmaskValueTag Type;
};

template <typename... TArgs>
struct TagOf<comms::field::BasicEnumValue<TArgs...> >
{
    static_assert(
        comms::field::isBasicEnumValue<comms::field::BasicEnumValue<TArgs...> >(),
        "isBasicEnumValue is supposed to return true");

    typedef BasicEnumValueTag Type;
};

template <typename... TArgs>
struct TagOf<comms::field::String<TArgs...> >
{
    static_assert(
        comms::field::isString<comms::field::String<TArgs...> >(),
        "isString is supposed to return true");

    typedef StringTag Type;
};

template <typename... TArgs>
struct TagOf<comms::field::Bitfield<TArgs...> >
{
    static_assert(
        comms::field::isBitfield<comms::field::Bitfield<TArgs...> >(),
        "isBitfield is supposed to return true");

    typedef BitfieldTag Type;
};

template <typename TField>
using TagOfT = typename TagOf<TField>::Type;

}  // namespace details

class DefaultMessageDisplayHandler : public MessageDisplayHandler
{
public:

    template <typename TMessage>
    void handle(TMessage& msg)
    {
        auto& fields = msg.getFields();
        comms::util::tupleForEach(fields, makeFieldsDisplayDispatcher(*this));
    }

protected:

    using FieldWidgetPtr = std::unique_ptr<FieldWidget>;

    virtual MsgWidgetPtr createMsgWidgetImpl(Message& msg) override;

private:

    using BasicIntValueTag = details::BasicIntValueTag;
    using BitmaskValueTag = details::BitmaskValueTag;
    using BasicEnumValueTag = details::BasicEnumValueTag;
    using StringTag = details::StringTag;
    using BitfieldTag = details::BitfieldTag;
    using UnknownValueTag = details::UnknownValueTag;

    template <typename THandler>
    class FieldsDisplayDispatcher
    {
    public:
        FieldsDisplayDispatcher(THandler& handler)
          : m_handler(handler)
        {
        }

        template <typename TField>
        void operator()(TField&& field)
        {
            using FieldType = typename std::decay<TField>::type;
            using Tag = details::TagOfT<FieldType>;

            auto fieldWidget =
                m_handler.createFieldWidget(std::forward<TField>(field), Tag());
            m_handler.m_widget->addFieldWidget(fieldWidget.release());
        }

    private:
        THandler& m_handler;
    };

    template <typename THandler>
    friend class FieldsDisplayDispatcher;

    template <typename THandler>
    FieldsDisplayDispatcher<THandler> makeFieldsDisplayDispatcher(THandler& handler)
    {
        return FieldsDisplayDispatcher<THandler>(handler);
    }

    template <typename TField>
    FieldWidgetPtr createFieldWidget(TField& field, BasicIntValueTag)
    {
        return createBasicIntValueFieldWidget(
            field_wrapper::makeBasicIntValueWrapper(field));
    }

    template <typename TField>
    FieldWidgetPtr createFieldWidget(TField& field, BitmaskValueTag)
    {
        return createBitmaskValueFieldWidget(
            field_wrapper::makeBitmaskValueWrapper(field));
    }

    template <typename TField>
    FieldWidgetPtr createFieldWidget(TField& field, BasicEnumValueTag)
    {
        return createBasicEnumValueFieldWidget(
            field_wrapper::makeBasicEnumValueWrapper(field));
    }

    template <typename TField>
    FieldWidgetPtr createFieldWidget(TField& field, StringTag)
    {
        return createStringFieldWidget(
            field_wrapper::makeStringWrapper(field));
    }

    template <typename TField>
    FieldWidgetPtr createFieldWidget(TField& field, BitfieldTag)
    {
        return createBitfieldFieldWidget(
            field_wrapper::makeBitfieldWrapper(field));
    }

    template <typename TField>
    FieldWidgetPtr createFieldWidget(TField& field, UnknownValueTag)
    {
        return createUnknownValueFieldWidget(
            field_wrapper::makeUnknownValueWrapper(field));
    }

    FieldWidgetPtr createBasicIntValueFieldWidget(
        field_wrapper::BasicIntValueWrapperPtr&& fieldWrapper);

    FieldWidgetPtr createBitmaskValueFieldWidget(
        field_wrapper::BitmaskValueWrapperPtr&& fieldWrapper);

    FieldWidgetPtr createBasicEnumValueFieldWidget(
        field_wrapper::BasicEnumValueWrapperPtr&& fieldWrapper);

    FieldWidgetPtr createStringFieldWidget(
        field_wrapper::StringWrapperPtr&& fieldWrapper);

    FieldWidgetPtr createBitfieldFieldWidget(
        field_wrapper::BitfieldWrapperPtr&& fieldWrapper);

    FieldWidgetPtr createUnknownValueFieldWidget(
        field_wrapper::UnknownValueWrapperPtr&& fieldWrapper);


    static void updateFieldIdxProperty(FieldWidget& field, std::size_t idx);

    using DefaultMsgWidgetPtr = std::unique_ptr<DefaultMessageWidget>;
    DefaultMsgWidgetPtr m_widget;
};

}  // namespace comms_champion


