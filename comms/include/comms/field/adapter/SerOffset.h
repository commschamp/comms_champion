//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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
    using Base = details::AdapterBaseT<TNext>;
    static const auto Offset = TOffset;
public:

    using ValueType = typename Base::ValueType;
    using SerialisedType = typename Base::SerialisedType;
    using Endian = typename Base::Endian;

    SerOffset() = default;

    explicit SerOffset(const ValueType& val)
      : Base(val)
    {
    }

    explicit SerOffset(ValueType&& val)
      : Base(std::move(val))
    {
    }

    SerOffset(const SerOffset&) = default;
    SerOffset(SerOffset&&) = default;
    SerOffset& operator=(const SerOffset&) = default;
    SerOffset& operator=(SerOffset&&) = default;


    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        if (size < Base::length()) {
            return ErrorStatus::NotEnoughData;
        }

        auto serialisedValue =
            comms::util::readData<SerialisedType>(iter, Endian());
        Base::value() = fromSerialised(serialisedValue);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (size < Base::length()) {
            return ErrorStatus::BufferOverflow;
        }

        comms::util::writeData(toSerialised(Base::value()), iter, Endian());
        return ErrorStatus::Success;
    }

    static constexpr SerialisedType toSerialised(ValueType val)
    {
        return adjustToSerialised(Base::toSerialised(val));
    }

    static constexpr ValueType fromSerialised(SerialisedType val)
    {
        return Base::fromSerialised(adjustFromSerialised(val));
    }

private:
    static SerialisedType adjustToSerialised(SerialisedType val)
    {
        return static_cast<SerialisedType>(Offset + val);
    }

    static SerialisedType adjustFromSerialised(SerialisedType val)
    {
        return static_cast<SerialisedType>((-Offset) + val);
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

