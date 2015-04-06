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

#include <vector>
#include <iterator>

#include "comms_champion/MessageBase.h"
#include "protocol/DemoMessage.h"

namespace demo
{

namespace plugin
{

typedef std::tuple<
    comms::option::MsgIdType<demo::message::MsgId>,
    comms::option::BigEndian,
    comms::option::ReadIterator<const std::uint8_t*>,
    comms::option::WriteIterator<std::back_insert_iterator<std::vector<std::uint8_t> > >
>CCDemoDefaultOptions;

class CCDemoMessage : public comms_champion::MessageBase<CCDemoDefaultOptions>
{
public:
    CCDemoMessage() = default;
    CCDemoMessage(const CCDemoMessage&) = default;
    virtual ~CCDemoMessage() = default;

    CCDemoMessage& operator=(const CCDemoMessage&) = default;

protected:
    virtual QString idAsStringImpl() const override;
    virtual void resetImpl() override;
    virtual void assignImpl(const comms_champion::Message& other) override;
};

}  // namespace plugin

}  // namespace demo

