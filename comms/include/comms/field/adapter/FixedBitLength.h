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
#include <limits>

#include "comms/Assert.h"
#include "comms/util/SizeToType.h"
#include "comms/util/BitSizeToByteSize.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <std::size_t TLen, typename TBase>
class FixedBitLength : public TBase
{
    using BaseImpl = TBase;
    using BaseSerialisedType = typename BaseImpl::SerialisedType;

    static const std::size_t BitLength = TLen;
    static const std::size_t Length =
        comms::util::BitSizeToByteSize<BitLength>::Value;

    static_assert(0 < BitLength, "Bit length is expected to be greater than 0");
    static_assert(Length <= sizeof(BaseSerialisedType),
        "The provided length limit is too big");

public:

    using ValueType = typename BaseImpl::ValueType;

    using SerialisedType = typename std::conditional<
        (Length < sizeof(BaseSerialisedType)),
        typename comms::util::SizeToType<Length, std::is_signed<BaseSerialisedType>::value>::Type,
        BaseSerialisedType
    >::type;

    using Endian = typename BaseImpl::Endian;

    FixedBitLength() = default;

    explicit FixedBitLength(const ValueType& val)
      : BaseImpl(val)
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

    static constexpr SerialisedType toSerialised(ValueType val)
    {
        return adjustToSerialised(BaseImpl::toSerialised(val), HasSignTag());
    }

    static constexpr ValueType fromSerialised(SerialisedType val)
    {
        return BaseImpl::fromSerialised(adjustFromSerialised(val, HasSignTag()));
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t size)
    {
        if (size < length()) {
            return comms::ErrorStatus::NotEnoughData;
        }

        readNoStatus(iter);
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        auto serialisedValue =
            comms::util::readData<SerialisedType, Length>(iter, Endian());
        BaseImpl::value() = fromSerialised(serialisedValue);
    }

    template <typename TIter>
    comms::ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (size < length()) {
            return comms::ErrorStatus::BufferOverflow;
        }

        writeNoStatus(iter);
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        BaseImpl::template writeData<Length>(toSerialised(BaseImpl::value()), iter);
    }

private:

    struct UnsignedTag {};
    struct SignedTag {};
    struct NoSignExtTag {};
    struct MustSignExtTag {};


    using HasSignTag = typename std::conditional<
        std::is_signed<SerialisedType>::value,
        SignedTag,
        UnsignedTag
    >::type;

    using UnsignedSerialisedType = typename std::make_unsigned<SerialisedType>::type;

    using SignExtTag = typename std::conditional<
        BitLength < static_cast<std::size_t>(std::numeric_limits<UnsignedSerialisedType>::digits),
        MustSignExtTag,
        NoSignExtTag
    >::type;


    static SerialisedType adjustToSerialised(BaseSerialisedType val, UnsignedTag)
    {
        return static_cast<SerialisedType>(val & UnsignedValueMask);
    }

    static SerialisedType adjustToSerialised(BaseSerialisedType val, SignedTag)
    {
        auto valueTmp =
            static_cast<UnsignedSerialisedType>(val) & UnsignedValueMask;

        return signExtUnsignedSerialised(valueTmp);
    }

    static BaseSerialisedType adjustFromSerialised(SerialisedType val, UnsignedTag)
    {
        return static_cast<BaseSerialisedType>(val & UnsignedValueMask);
    }

    static BaseSerialisedType adjustFromSerialised(SerialisedType val, SignedTag)
    {
        auto valueTmp = static_cast<UnsignedSerialisedType>(val) & UnsignedValueMask;
        return
            static_cast<BaseSerialisedType>(
                signExtUnsignedSerialised(valueTmp));
    }

    static SerialisedType signExtUnsignedSerialised(UnsignedSerialisedType val)
    {
        return signExtUnsignedSerialisedInternal(val, SignExtTag());
    }

    static SerialisedType signExtUnsignedSerialisedInternal(UnsignedSerialisedType val, MustSignExtTag)
    {
        static_assert(
            BitLength < std::numeric_limits<UnsignedSerialisedType>::digits,
            "BitLength is expected to be less than number of bits in the value type");

        static const UnsignedSerialisedType SignExtMask =
            ~((static_cast<UnsignedSerialisedType>(1U) << BitLength) - 1);
        static const UnsignedSerialisedType SignMask =
            static_cast<UnsignedSerialisedType>(1U) << (BitLength - 1);

        if ((val & SignMask) != 0) {
            val |= SignExtMask;
        }
        return static_cast<SerialisedType>(val);
    }

    static SerialisedType signExtUnsignedSerialisedInternal(UnsignedSerialisedType val, NoSignExtTag)
    {
        return static_cast<SerialisedType>(val);
    }



private:
    static const UnsignedSerialisedType UnsignedValueMask =
        (static_cast<UnsignedSerialisedType>(1U) << BitLength) - 1;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


