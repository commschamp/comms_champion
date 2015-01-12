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
#include "protocol/message/Status.h"
#include "CCDemoMessage.h"

namespace demo
{

namespace plugin
{

namespace message
{

class CCStatus : public demo::message::Status<demo::plugin::CCDemoMessage>
{
    using Base = demo::message::Status<demo::plugin::CCDemoMessage>;
public:

    CCStatus() = default;
    CCStatus(const CCStatus&) = default;
    virtual ~CCStatus() = default;

    CCStatus& operator=(const CCStatus&) = default;

protected:
    virtual const char* nameImpl() const override;
    virtual void updateFieldPropertiesImpl(QWidget& fieldWidget, uint idx) const override;
    virtual void resetImpl() override;
};

}  // namespace message

}  // namespace plugin

}  // namespace demo


