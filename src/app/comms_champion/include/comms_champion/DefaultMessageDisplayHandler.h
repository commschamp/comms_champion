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
    template <typename TMessage>
    void handle(TMessage& msg)
    {
        auto& fields = msg.getFields();
        comms::util::tupleForEach(fields, makeFieldsDisplayDispatcher(*this));
    }

protected:

    using FieldWidgetPtr = std::unique_ptr<FieldWidget>;

    virtual MsgWidgetPtr createMsgWidgetImpl(Message& msg) override;

    template <typename TField>
    void displayField(TField& field)
    {
        auto fieldWidget = createFieldWidget<TField>(field);
        m_widget->addFieldWidget(fieldWidget.release());
    }

    template <typename TField, typename... TArgs>
    FieldWidgetPtr createFieldWidget(
        comms::field::BasicIntValue<TArgs...>& field)
    {
        auto& castedField = static_cast<TField&>(field);
        return createBasicIntValueFieldWidget(
            field_wrapper::makeBasicIntValueWrapper(castedField));
    }

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
            m_handler.displayField(std::forward<TField>(field));
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

    FieldWidgetPtr createBasicIntValueFieldWidget(
        field_wrapper::BasicIntValueWrapperPtr&& fieldWrapper);

    static void updateFieldIdxProperty(FieldWidget& field, std::size_t idx);

    using DefaultMsgWidgetPtr = std::unique_ptr<DefaultMessageWidget>;
    DefaultMsgWidgetPtr m_widget;
};

}  // namespace comms_champion


