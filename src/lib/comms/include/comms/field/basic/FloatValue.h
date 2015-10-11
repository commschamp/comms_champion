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
#include <ratio>

#include "comms/ErrorStatus.h"
#include "comms/field/category.h"
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

    typedef TFieldBase Base;
public:

    typedef comms::field::category::NumericValueField Category;

    typedef T ValueType;

    typedef typename comms::util::SizeToType<sizeof(ValueType), false>::Type SerialisedType;

    typedef std::ratio<1, 1> ScalingRatio;

    FloatValue() = default;

    explicit FloatValue(ValueType value)
      : value_(value)
    {
    }

    FloatValue(const FloatValue&) = default;
    FloatValue(FloatValue&&) = default;
    ~FloatValue() = default;

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

    static SerialisedType toSerialised(ValueType value)
    {
        CastUnion castUnion;
        castUnion.value_ = value;
        return castUnion.serValue_;
    }

    static ValueType fromSerialised(SerialisedType value)
    {
        CastUnion castUnion;
        castUnion.serValue_ = value;
        return castUnion.value_;
    }

    static constexpr bool valid()
    {
        return true;
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        if (size < length()) {
            return ErrorStatus::NotEnoughData;
        }

        auto serialisedValue =
            Base::template readData<SerialisedType>(iter);
        value_ = fromSerialised(serialisedValue);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (size < length()) {
            return ErrorStatus::BufferOverflow;
        }

        Base::writeData(toSerialised(value_), iter);
        return ErrorStatus::Success;
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


