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

#include <type_traits>

#include "details/AdapterBase.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <std::size_t TLen, typename TNext>
class FixedLength : public details::AdapterBaseT<TNext>
{
    typedef details::AdapterBaseT<TNext> Base;
    typedef typename Base::Next::SerialisedType NextSerialisedType;
public:

    typedef typename Base::ParamValueType ParamValueType;

    static_assert(TLen <= sizeof(NextSerialisedType),
        "The provided length limit is too big");

    typedef typename std::conditional<
        (TLen < sizeof(NextSerialisedType)),
        typename comms::util::SizeToType<TLen, std::is_signed<NextSerialisedType>::value>::Type,
        NextSerialisedType
    >::type SerialisedType;

    typedef typename Base::Endian Endian;

    static constexpr std::size_t length()
    {
        return Length;
    }

    static constexpr std::size_t minLength()
    {
        return length();
    }

    static constexpr std::size_t maxLength()
    {
        return length();
    }

    constexpr SerialisedType toSerialised(ParamValueType value)
    {
        return static_cast<SerialisedType>(Base::toSerialised(value));
    }

    static constexpr ParamValueType fromSerialised(SerialisedType value)
    {
        return Base::fromSerialised(static_cast<NextSerialisedType>(value));
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        if (size < length()) {
            return ErrorStatus::NotEnoughData;
        }

        auto serialisedValue =
            comms::util::readData<SerialisedType, Length>(iter, Endian());
        Base::setValue(fromSerialised(serialisedValue));
        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (size < length()) {
            return ErrorStatus::BufferOverflow;
        }

        comms::util::writeData<Length>(toSerialised(Base::getValue()), iter, Endian());
        return ErrorStatus::Success;
    }

private:
    static const std::size_t Length = TLen;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


