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

#include "comms/comms.h"
#include "protocol/message/SerialInfo.h"
#include "CCDemoMessage.h"

namespace demo
{

namespace plugin
{

namespace message
{

class CCSerialInfo : public demo::message::SerialInfo<demo::plugin::CCDemoMessage>
{
    using Base = demo::message::SerialInfo<demo::plugin::CCDemoMessage>;
public:

    CCSerialInfo() = default;
    CCSerialInfo(const CCSerialInfo&) = default;
    virtual ~CCSerialInfo() = default;

    CCSerialInfo& operator=(const CCSerialInfo&) = default;

protected:
    virtual const char* nameImpl() const override;
    virtual void updateFieldPropertiesImpl(QWidget& fieldWidget, uint idx) const override;
    virtual const QVariantList& fieldsPropertiesImpl() const override;
    virtual void resetImpl() override;
    virtual void assignImpl(const comms_champion::Message& other) override;
};

}  // namespace message

}  // namespace plugin

}  // namespace demo


