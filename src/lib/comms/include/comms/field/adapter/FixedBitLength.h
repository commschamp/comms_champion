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
#include <limits>

#include "details/AdapterBase.h"
#include "comms/Assert.h"
#include "comms/util/SizeToType.h"
#include "comms/util/BitSizeToByteSize.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <std::size_t TLen, typename TNext>
class FixedBitLength : public details::AdapterBaseT<TNext>
{
    typedef details::AdapterBaseT<TNext> Base;
    typedef typename Base::Next::SerialisedType NextSerialisedType;

    static const std::size_t BitLength = TLen;
    static const std::size_t Length =
        comms::util::BitSizeToByteSize<BitLength>::Value;

    static_assert(0 < BitLength, "Bit length is expected to be greater than 0");
    static_assert(Length <= sizeof(NextSerialisedType),
        "The provided length limit is too big");

public:

    typedef typename Base::ValueType ValueType;

    typedef typename std::conditional<
        (Length < sizeof(NextSerialisedType)),
        typename comms::util::SizeToType<Length, std::is_signed<NextSerialisedType>::value>::Type,
        NextSerialisedType
    >::type SerialisedType;

    typedef typename Base::Endian Endian;

    FixedBitLength() = default;

    explicit FixedBitLength(const ValueType& value)
      : Base(fromSerialised(toSerialised(value)))
    {
    }

    FixedBitLength(const FixedBitLength&) = default;
    FixedBitLength(FixedBitLength&&) = default;
    FixedBitLength& operator=(const FixedBitLength&) = default;
    FixedBitLength& operator=(FixedBitLength&&) = default;

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

    static constexpr SerialisedType toSerialised(ValueType value)
    {
        return adjustToSerialised(Base::toSerialised(value), HasSignTag());
    }

    static constexpr ValueType fromSerialised(SerialisedType value)
    {
        return Base::fromSerialised(adjustFromSerialised(value, HasSignTag()));
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        if (size < length()) {
            return ErrorStatus::NotEnoughData;
        }

        auto serialisedValue =
            comms::util::readData<SerialisedType, Length>(iter, Endian());
        Base::value() = fromSerialised(serialisedValue);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (size < length()) {
            return ErrorStatus::BufferOverflow;
        }

        comms::util::writeData<Length>(toSerialised(Base::value()), iter, Endian());
        return ErrorStatus::Success;
    }

private:

    struct UnsignedTag {};
    struct SignedTag {};

    typedef typename std::conditional<
        std::is_signed<SerialisedType>::value,
        SignedTag,
        UnsignedTag
    >::type HasSignTag;

    typedef typename std::make_unsigned<SerialisedType>::type UnsignedSerialisedType;

    static SerialisedType adjustToSerialised(NextSerialisedType value, UnsignedTag)
    {
        return static_cast<SerialisedType>(value & UnsignedValueMask);
    }

    static SerialisedType adjustToSerialised(NextSerialisedType value, SignedTag)
    {
        auto valueTmp =
            static_cast<UnsignedSerialisedType>(value) & UnsignedValueMask;

        return signExtUnsignedSerialised(valueTmp);
    }

    static NextSerialisedType adjustFromSerialised(SerialisedType value, UnsignedTag)
    {
        return static_cast<NextSerialisedType>(value & UnsignedValueMask);
    }

    static NextSerialisedType adjustFromSerialised(SerialisedType value, SignedTag)
    {
        auto valueTmp = static_cast<UnsignedSerialisedType>(value) & UnsignedValueMask;
        return
            static_cast<NextSerialisedType>(
                signExtUnsignedSerialised(valueTmp));
    }

    static SerialisedType signExtUnsignedSerialised(UnsignedSerialisedType value)
    {
        static const UnsignedSerialisedType SignExtMask =
            ~((static_cast<UnsignedSerialisedType>(1U) << BitLength) - 1);
        static const auto SignMask =
            static_cast<UnsignedSerialisedType>(1U) << (BitLength - 1);

        if ((value & SignMask) != 0) {
            value |= SignExtMask;
        }
        return static_cast<SerialisedType>(value);
    }

private:
    static const auto UnsignedValueMask =
        (static_cast<UnsignedSerialisedType>(1U) << BitLength) - 1;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


