//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
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

#include <functional>

#include "Api.h"
#include "Message.h"
#include "field_wrapper/FieldWrapper.h"
#include "details/FieldWrapperCreator.h"

namespace comms_champion
{

class CC_API MessageHandler
{
public:
    typedef field_wrapper::FieldWrapperPtr FieldWrapperPtr;

    virtual ~MessageHandler();

    template <typename TMessage>
    void handle(TMessage& msg)
    {
        beginMsgHandlingImpl(msg);
        auto& fields = msg.fields();
        comms::util::tupleForEach(
            fields,
            FieldsWrapperCreateHelper(
                [this](FieldWrapperPtr wrapper)
                {
                    addFieldImpl(std::move(wrapper));
                }));
        endMsgHandlingImpl();
    }

protected:
    virtual void beginMsgHandlingImpl(Message& msg);
    virtual void addFieldImpl(FieldWrapperPtr wrapper);
    virtual void endMsgHandlingImpl();

private:
    class FieldsWrapperCreateHelper
    {
    public:
        typedef std::function <void (FieldWrapperPtr)> WrapperDispatchFunc;
        FieldsWrapperCreateHelper(WrapperDispatchFunc&& dispatchOp)
          : m_dispatchOp(std::move(dispatchOp))
        {
        }

        template <typename TField>
        void operator()(TField&& field)
        {
            auto wraper =
                details::FieldWrapperCreator::createWrapper(
                    std::forward<TField>(field));
            m_dispatchOp(std::move(wraper));
        }

    private:
        WrapperDispatchFunc m_dispatchOp;
    };
};

}  // namespace comms_champion


