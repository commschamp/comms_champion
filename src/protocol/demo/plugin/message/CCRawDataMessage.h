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

#include <tuple>
#include "comms/comms.h"
#include "CCDemoMessage.h"

namespace demo
{

namespace plugin
{

namespace message
{

typedef std::tuple<
    comms::field::ArrayList<
        CCDemoMessage::Field,
        std::uint8_t
    >
> RawDataMessageFields;

class CCRawDataMessage : public
    comms::MessageBase<
        CCDemoMessage,
        comms::option::NoIdImpl,
        comms::option::FieldsImpl<RawDataMessageFields>,
        comms::option::DispatchImpl<CCRawDataMessage>
    >
{
public:
    CCRawDataMessage() = default;
    CCRawDataMessage(const CCRawDataMessage&) = default;
    virtual ~CCRawDataMessage() = default;

    CCRawDataMessage& operator=(const CCRawDataMessage&) = default;

protected:
    virtual const char* nameImpl() const override;
    virtual void updateFieldPropertiesImpl(QWidget& fieldWidget, uint idx) const override;
    virtual const QVariantList& fieldsPropertiesImpl() const override;
};

}  // namespace message

}  // namespace plugin

}  // namespace demo


