//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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
#include <tuple>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

#include "comms/comms.h"
#include "property/field.h"
#include "ProtocolMessageBase.h"

namespace comms_champion
{

namespace details
{

template <typename TMsgBase>
class ExtraInfoMessageImpl : public
    comms::MessageBase<
        TMsgBase,
        comms::option::NoIdImpl,
        comms::option::FieldsImpl<std::tuple<comms::field::String<typename TMsgBase::Field> > >,
        comms::option::DispatchImpl<ExtraInfoMessageImpl<TMsgBase> >
    >
{

};

}  // namespace details

template <typename TMsgBase>
class ExtraInfoMessage : public
    ProtocolMessageBase<
        details::ExtraInfoMessageImpl<TMsgBase>,
        ExtraInfoMessage<TMsgBase>
    >
{
public:
    virtual ~ExtraInfoMessage() = default;

protected:
    virtual const char*
    nameImpl() const override
    {
        static const char* Str = "Generic Extra Info Message";
        return Str;
    }

    virtual const QVariantList& fieldsPropertiesImpl() const override
    {
        static const QVariantList Props = createFieldsProperties();
        return Props;
    }

    virtual QString idAsStringImpl() const override
    {
        assert(!"Mustn't be called");
        return QString();
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

private:

    static QVariantList createFieldsProperties()
    {
        QVariantList props;
        props.append(
            property::field::String().serialisedHidden().asMap());
        return props;
    }
};

}  // namespace comms_champion



