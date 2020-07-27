//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <ratio>

#include "comms/ErrorStatus.h"
#include "comms/util/SizeToType.h"

namespace comms
{

namespace field
{

namespace basic
{

template <typename TFieldBase, typename T>
class FloatValue : public TFieldBase
{
    static_assert(std::is_floating_point<T>::value, "T must be floating point value");

    using BaseImpl = TFieldBase;
public:

    using ValueType = T;

    using SerialisedType = typename comms::util::SizeToType<sizeof(ValueType), false>::Type;

    using ScalingRatio = std::ratio<1, 1>;

    FloatValue() = default;

    explicit FloatValue(ValueType val)
      : value_(val)
    {
    }

    FloatValue(const FloatValue&) = default;
    FloatValue(FloatValue&&) = default;
    ~FloatValue() noexcept = default;

    FloatValue& operator=(const FloatValue&) = default;
    FloatValue& operator=(FloatValue&&) = default;

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

    static SerialisedType toSerialised(ValueType val)
    {
        CastUnion castUnion;
        castUnion.value_ = val;
        return castUnion.serValue_;
    }

    static ValueType fromSerialised(SerialisedType val)
    {
        CastUnion castUnion;
        castUnion.serValue_ = val;
        return castUnion.value_;
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
    union CastUnion
    {
        ValueType value_;
        SerialisedType serValue_;
    };

    ValueType value_ = static_cast<ValueType>(0.0);
};

}  // namespace basic

}  // namespace field

}  // namespace comms


