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

#include "Message.h"
#include "MessageDisplayHandler.h"

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
    virtual QWidgetPtr createMsgWidgetImpl(const Message& msg) override;

    template <typename... TArgs>
    void displayField(const comms::field::BasicIntValue<TArgs...>& field)
    {
        static_cast<void>(field);
        assert(!"Must display field");
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

    using LayoutType = QVBoxLayout;
    QWidgetPtr m_widget;
    LayoutType* m_layout;
};

}  // namespace comms_champion


