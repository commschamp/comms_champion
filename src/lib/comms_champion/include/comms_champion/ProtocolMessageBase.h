//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

#include <type_traits>
#include <string>
#include <cassert>

namespace comms_champion
{

template <typename TMsgBase, typename TActualMsg>
class ProtocolMessageBase : public TMsgBase
{
    typedef TMsgBase Base;
    typedef TActualMsg ActualMsg;

public:
    ProtocolMessageBase() = default;
    ProtocolMessageBase(const ProtocolMessageBase&) = default;
    ProtocolMessageBase(ProtocolMessageBase&&) = default;
    virtual ~ProtocolMessageBase() = default;

    ProtocolMessageBase& operator=(const ProtocolMessageBase&) = default;
    ProtocolMessageBase& operator=(ProtocolMessageBase&&) = default;

protected:

    virtual void resetImpl() override
    {
        auto& actObj = static_cast<ActualMsg&>(*this);
        actObj = ActualMsg();
    }

    virtual bool assignImpl(const comms_champion::Message& other) override
    {
        auto* castedOther = dynamic_cast<const ActualMsg*>(&other);
        if (castedOther == nullptr) {
            return false;
        }

        assert(other.idAsString() == Base::idAsString());
        auto& actObj = static_cast<ActualMsg&>(*this);
        actObj = *castedOther;
        return true;
    }
};

}  // namespace comms_champion


