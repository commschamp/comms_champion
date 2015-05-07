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

    typedef typename Base::ParamValueType ParamValueType;

    typedef typename std::conditional<
        (Length < sizeof(NextSerialisedType)),
        typename comms::util::SizeToType<Length, std::is_signed<NextSerialisedType>::value>::Type,
        NextSerialisedType
    >::type SerialisedType;

    typedef typename Base::Endian Endian;

    FixedBitLength() = default;

    explicit FixedBitLength(ParamValueType value)
      : Base(fromSerialised(toSerialised(value)))
    {
    }

    FixedBitLength(const FixedBitLength&) = default;
    FixedBitLength(FixedBitLength&&) = default;
    FixedBitLength& operator=(const FixedBitLength&) = default;
    FixedBitLength& operator=(FixedBitLength&&) = default;

    ParamValueType getValue() const
    {
        auto value = Base::getValue();
        GASSERT(value == fromSerialised(toSerialised(value)));
        return value;
    }

    void setValue(ParamValueType value)
    {
        Base::setValue(fromSerialised(toSerialised(value)));
    }

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

    static constexpr SerialisedType toSerialised(ParamValueType value)
    {
        return toSerialisedInternal(value, HasSignTag());
    }

    static constexpr ParamValueType fromSerialised(SerialisedType value)
    {
        return fromSerialisedInternal(value, HasSignTag());
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        if (size < length()) {
            return ErrorStatus::NotEnoughData;
        }

        auto unsignedSerialisedValue =
            comms::util::readData<UnsignedSerialisedType, Length>(iter, Endian());
        auto serialisedValue = signExtUnsignedSerialised(unsignedSerialisedValue, HasSignTag());
        Base::setValue(Base::fromSerialised(serialisedValue));
        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (size < length()) {
            return ErrorStatus::BufferOverflow;
        }

        comms::util::writeData<Length>(Base::toSerialised(Base::getValue()), iter, Endian());
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


    static SerialisedType toSerialisedInternal(ParamValueType value, UnsignedTag)
    {
        static const auto Mask =
            (static_cast<UnsignedSerialisedType>(1U) << BitLength) - 1;

        return static_cast<SerialisedType>(Base::toSerialised(value) & Mask);
    }

    static SerialisedType toSerialisedInternal(ParamValueType value, SignedTag)
    {
        auto serValue =
            static_cast<UnsignedSerialisedType>(toSerialisedInternal(value, UnsignedTag()));

        return signExtUnsignedSerialised(serValue, SignedTag());
    }

    static ParamValueType fromSerialisedInternal(SerialisedType value, UnsignedTag)
    {
        static const auto Mask =
            (static_cast<UnsignedSerialisedType>(1U) << BitLength) - 1;

        return Base::fromSerialised(static_cast<NextSerialisedType>(value & Mask));
    }

    static ParamValueType fromSerialisedInternal(SerialisedType value, SignedTag)
    {
        auto valueTmp = fromSerialisedInternal(
            static_cast<UnsignedSerialisedType>(value),
            UnsignedTag());
        return fromSerialisedInternal(
            signExtUnsignedSerialised(valueTmp, SignedTag()),
            UnsignedTag());
    }

    static constexpr SerialisedType signExtUnsignedSerialised(UnsignedSerialisedType value, UnsignedTag)
    {
        return static_cast<SerialisedType>(value);
    }

    static SerialisedType signExtUnsignedSerialised(UnsignedSerialisedType value, SignedTag)
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
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


