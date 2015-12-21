//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include <cassert>
#include <type_traits>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QVBoxLayout>
CC_ENABLE_WARNINGS()

#include "comms/comms.h"

#include "comms_champion/Message.h"
#include "comms_champion/MessageDisplayHandler.h"
#include "comms_champion/DefaultMessageWidget.h"
#include "comms_champion/FieldWidgetCreator.h"

namespace comms_champion
{

class DefaultMessageDisplayHandler : public MessageDisplayHandler
{
public:

    template <typename TMessage>
    void handle(TMessage& msg)
    {
        auto& fields = msg.fields();
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


