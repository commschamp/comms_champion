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

#include <type_traits>

#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace basic
{

template <typename TFieldBase, typename T>
class IntValue : public TFieldBase
{
    static_assert(std::is_integral<T>::value, "T must be integral value");

    using BaseImpl = TFieldBase;
public:

    using ValueType = T;

    using SerialisedType = ValueType;
    using ScalingRatio = std::ratio<1, 1>;

    IntValue() = default;

    explicit IntValue(ValueType val)
      : value_(val)
    {
    }

    IntValue(const IntValue&) = default;
    IntValue(IntValue&&) = default;
    ~IntValue() noexcept = default;

    IntValue& operator=(const IntValue&) = default;
    IntValue& operator=(IntValue&&) = default;

    const ValueType& value() const
    {
        return value_;
    }

    ValueType& value()
    {
        return value_;
    }

    static constexpr std::size_t length()
    {
        return sizeof(SerialisedType);
    }

    static constexpr std::size_t minLength()
    {
        return length();
    }

    static constexpr std::size_t maxLength()
    {
        return length();
    }

    static constexpr SerialisedType toSerialised(ValueType val)
    {
        return static_cast<SerialisedType>(val);
    }

    static constexpr ValueType fromSerialised(SerialisedType val)
    {
        return static_cast<ValueType>(val);
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        if (size < length()) {
            return ErrorStatus::NotEnoughData;
        }

        readNoStatus(iter);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        auto serialisedValue =
            BaseImpl::template readData<SerialisedType>(iter);
        value_ = fromSerialised(serialisedValue);
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (size < length()) {
            return ErrorStatus::BufferOverflow;
        }

        writeNoStatus(iter);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        BaseImpl::writeData(toSerialised(value_), iter);
    }

private:
    ValueType value_ = static_cast<ValueType>(0);
};

}  // namespace basic

}  // namespace field

}  // namespace comms


