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
#include "comms_champion/FieldWidgetCreator.h"

#include "comms_champion/field_wrapper/IntValueWrapper.h"
#include "comms_champion/field_wrapper/LongIntValueWrapper.h"
#include "comms_champion/field_wrapper/BitmaskValueWrapper.h"
#include "comms_champion/field_wrapper/EnumValueWrapper.h"
#include "comms_champion/field_wrapper/StringWrapper.h"
#include "comms_champion/field_wrapper/BitfieldWrapper.h"
#include "comms_champion/field_wrapper/OptionalWrapper.h"
#include "comms_champion/field_wrapper/UnknownValueWrapper.h"

namespace comms_champion
{

class DefaultMessageDisplayHandler : public MessageDisplayHandler
{
public:

    typedef FieldWidgetCreator::FieldWidgetPtr FieldWidgetPtr;

    template <typename TMessage>
    void handle(TMessage& msg)
    {
        auto& fields = msg.getFields();
        comms::util::tupleForEach(
            fields,
            FieldsDisplayDispatcher(
                [this](FieldWidgetPtr fieldWidget)
                {
                    m_widget->addFieldWidget(fieldWidget.release());
                }));
    }

protected:

    virtual MsgWidgetPtr createMsgWidgetImpl(Message& msg) override;

private:

    class FieldsDisplayDispatcher
    {
    public:
        typedef std::function <void (FieldWidgetPtr)> WidgetDispatchFunc;
        FieldsDisplayDispatcher(WidgetDispatchFunc&& dispatchOp)
          : m_dispatchOp(std::move(dispatchOp))
        {
        }

        template <typename TField>
        void operator()(TField&& field)
        {
            auto fieldWidget =
                FieldWidgetCreator::createWidget(std::forward<TField>(field));
            m_dispatchOp(std::move(fieldWidget));
        }

    private:
        WidgetDispatchFunc m_dispatchOp;
    };

    using DefaultMsgWidgetPtr = std::unique_ptr<DefaultMessageWidget>;
    DefaultMsgWidgetPtr m_widget;
};

}  // namespace comms_champion


