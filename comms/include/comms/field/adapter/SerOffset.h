//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "comms/ErrorStatus.h"
#include "comms/util/access.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <long long int TOffset, typename TBase>
class SerOffset : public TBase
{
    using BaseImpl = TBase;
    static const auto Offset = TOffset;
public:

    using ValueType = typename BaseImpl::ValueType;
    using SerialisedType = typename BaseImpl::SerialisedType;
    using Endian = typename BaseImpl::Endian;

    SerOffset() = default;

    explicit SerOffset(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit SerOffset(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    SerOffset(const SerOffset&) = default;
    SerOffset(SerOffset&&) = default;
    SerOffset& operator=(const SerOffset&) = default;
    SerOffset& operator=(SerOffset&&) = default;


    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        if (size < BaseImpl::length()) {
            return ErrorStatus::NotEnoughData;
        }

        readNoStatus(iter);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        auto serialisedValue =
            comms::util::readData<SerialisedType>(iter, Endian());
        BaseImpl::value() = fromSerialised(serialisedValue);
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (size < BaseImpl::length()) {
            return ErrorStatus::BufferOverflow;
        }

        writeNoStatus(iter);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        comms::util::writeData(toSerialised(BaseImpl::value()), iter, Endian());
    }

    static constexpr SerialisedType toSerialised(ValueType val)
    {
        return adjustToSerialised(BaseImpl::toSerialised(val));
    }

    static constexpr ValueType fromSerialised(SerialisedType val)
    {
        return BaseImpl::fromSerialised(adjustFromSerialised(val));
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

