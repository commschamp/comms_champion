//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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


