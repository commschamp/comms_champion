//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

#include "details/AdapterBase.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <long long int TOffset, typename TNext>
class SerOffset : public details::AdapterBaseT<TNext>
{
    typedef details::AdapterBaseT<TNext> Base;
    static const auto Offset = TOffset;
public:

    typedef typename Base::ValueType ValueType;
    typedef typename Base::ParamValueType ParamValueType;
    typedef typename Base::SerialisedType SerialisedType;
    typedef typename Base::Endian Endian;

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        if (size < Base::length()) {
            return ErrorStatus::NotEnoughData;
        }

        auto serialisedValue =
            comms::util::readData<SerialisedType>(iter, Endian());
        Base::setValue(fromSerialised(serialisedValue));
        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (size < Base::length()) {
            return ErrorStatus::BufferOverflow;
        }

        comms::util::writeData(toSerialised(Base::getValue()), iter, Endian());
        return ErrorStatus::Success;
    }

    static constexpr SerialisedType toSerialised(ParamValueType value)
    {
        return static_cast<SerialisedType>(Offset + value);
    }

    static constexpr ParamValueType fromSerialised(SerialisedType value)
    {
        return static_cast<ValueType>((-Offset) + value);
    }

private:
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

