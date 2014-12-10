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

#include <memory>
#include <cstdint>
#include <cstddef>
#include <list>
#include <string>

#include "Message.h"
#include "ErrorStatus.h"
#include "MessageInfo.h"

namespace comms_champion
{

class Protocol
{
public:
    typedef const std::uint8_t* ReadIterType;
    typedef std::list<MessageInfoPtr> MessagesList;

    virtual ~Protocol() {}

    const std::string& name() const
    {
        return nameImpl();
    }

    MessagesList read(
        ReadIterType iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        return readImpl(iter, size, missingSize);
    }

protected:
    virtual const std::string& nameImpl() const = 0;

    virtual MessagesList readImpl(
        ReadIterType iter,
        std::size_t size,
        std::size_t* missingSize) = 0;
};

}  // namespace comms_champion


