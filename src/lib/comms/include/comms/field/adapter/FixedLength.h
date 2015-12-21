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

    typedef typename Base::ValueType ValueType;

    static_assert(TLen <= sizeof(NextSerialisedType),
        "The provided length limit is too big");

    typedef typename std::conditional<
        (TLen < sizeof(NextSerialisedType)),
        typename comms::util::SizeToType<TLen, std::is_signed<NextSerialisedType>::value>::Type,
        NextSerialisedType
    >::type SerialisedType;

    typedef typename Base::Endian Endian;

    FixedLength() = default;

    explicit FixedLength(const ValueType& val)
      : Base(fromSerialised(toSerialised(val)))
    {
    }

    FixedLength(const FixedLength&) = default;
    FixedLength(FixedLength&&) = default;
    FixedLength& operator=(const FixedLength&) = default;
    FixedLength& operator=(FixedLength&&) = default;

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

    static constexpr SerialisedType toSerialised(ValueType val)
    {
        return adjustToSerialised(Base::toSerialised(val), ConversionTag());
    }

    static constexpr ValueType fromSerialised(SerialisedType val)
    {
        return Base::fromSerialised(adjustFromSerialised(val, ConversionTag()));
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

    struct JustCastTag {};
    struct SignExtendTag {};
    struct UnsignedTag {};
    struct SignedTag {};

    typedef typename std::conditional<
        (TLen < sizeof(SerialisedType)),
        SignExtendTag,
        JustCastTag
    >::type ConversionTag;


    typedef typename std::conditional<
        std::is_signed<SerialisedType>::value,
        SignedTag,
        UnsignedTag
    >::type HasSignTag;

    typedef typename std::make_unsigned<SerialisedType>::type UnsignedSerialisedType;

    static constexpr SerialisedType adjustToSerialised(NextSerialisedType val, JustCastTag)
    {
        return static_cast<SerialisedType>(val);
    }

    static SerialisedType adjustToSerialised(NextSerialisedType val, SignExtendTag)
    {
        auto valueTmp =
            static_cast<UnsignedSerialisedType>(val) & UnsignedValueMask;

        return signExtUnsignedSerialised(valueTmp, HasSignTag());
    }

    static constexpr NextSerialisedType adjustFromSerialised(SerialisedType val, JustCastTag)
    {
        return static_cast<NextSerialisedType>(val);
    }

    static NextSerialisedType adjustFromSerialised(SerialisedType val, SignExtendTag)
    {
        auto valueTmp = static_cast<UnsignedSerialisedType>(val) & UnsignedValueMask;
        return static_cast<NextSerialisedType>(signExtUnsignedSerialised(valueTmp, HasSignTag()));
    }

    static constexpr SerialisedType signExtUnsignedSerialised(UnsignedSerialisedType val, UnsignedTag)
    {
        return static_cast<SerialisedType>(val);
    }

    static SerialisedType signExtUnsignedSerialised(UnsignedSerialisedType val, SignedTag)
    {
        static const UnsignedSerialisedType SignExtMask =
            ~((static_cast<UnsignedSerialisedType>(1U) << BitLength) - 1);
        static const auto SignMask =
            static_cast<UnsignedSerialisedType>(1U) << (BitLength - 1);

        if ((val & SignMask) != 0) {
            val |= SignExtMask;
        }
        return static_cast<SerialisedType>(val);
    }

    static const std::size_t Length = TLen;
    static const std::size_t BitLength =
        Length * std::numeric_limits<std::uint8_t>::digits;

    static const auto UnsignedValueMask =
        (static_cast<UnsignedSerialisedType>(1U) << BitLength) - 1;

    static_assert(0 < Length, "Length is expected to be greater than 0");
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


