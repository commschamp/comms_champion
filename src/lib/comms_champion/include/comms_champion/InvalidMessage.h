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
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include <cassert>
#include <tuple>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

#include "comms/comms.h"
#include "property/message.h"
#include "ProtocolMessageBase.h"

namespace comms_champion
{

namespace details
{

template <typename TMsgBase>
class InvalidMessageImpl : public
    comms::MessageBase<
        TMsgBase,
        comms::option::NoIdImpl,
        comms::option::FieldsImpl<std::tuple<> >,
        comms::option::MsgType<InvalidMessageImpl<TMsgBase> >,
        comms::option::DispatchImpl
    >
{

};

}  // namespace details

template <typename TMessage>
class InvalidMessage : public
    ProtocolMessageBase<
        details::InvalidMessageImpl<TMessage>,
        InvalidMessage<TMessage>
    >
{
public:
    virtual ~InvalidMessage() = default;

protected:
    virtual const char*
    nameImpl() const override
    {
        if (property::message::TransportMsg().getFrom(*this)) {
            static const char* InvalidMsgStr = "???";
            return InvalidMsgStr;
        }

        static const char* GarbageStr = "-#-";
        return GarbageStr;
    }

    virtual const QVariantList& fieldsPropertiesImpl() const override
    {
        static const QVariantList Props;
        return Props;
    }

    virtual QString idAsStringImpl() const override
    {
        return QString();
    }

    virtual bool isValidImpl() const override
    {
        return false;
    }

    virtual void resetImpl() override
    {
        assert(!"Mustn't be called");
    }

    virtual bool assignImpl(const comms_champion::Message& other) override
    {
        static_cast<void>(other);
        assert(!"Mustn't be called");
        return false;
    }

};

}  // namespace comms_champion


