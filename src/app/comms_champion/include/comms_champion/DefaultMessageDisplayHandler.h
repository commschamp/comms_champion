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

namespace comms_champion
{

class DefaultMessageDisplayHandler : public MessageDisplayHandler
{
public:
    struct BasicIntValueTag {};
    struct UnknownValueTag {};

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
            using Tag =
                typename std::conditional<
                    comms::field::isBasicIntValue<FieldType>(),
                    BasicIntValueTag,
                    UnknownValueTag
                >::type;
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
    FieldWidgetPtr createFieldWidget(TField& field, UnknownValueTag)
    {
        assert(!"Displaying unknown field is not implemented yet");
    }

    FieldWidgetPtr createBasicIntValueFieldWidget(
        field_wrapper::BasicIntValueWrapperPtr&& fieldWrapper);

    static void updateFieldIdxProperty(FieldWidget& field, std::size_t idx);

    using DefaultMsgWidgetPtr = std::unique_ptr<DefaultMessageWidget>;
    DefaultMsgWidgetPtr m_widget;
};

}  // namespace comms_champion


