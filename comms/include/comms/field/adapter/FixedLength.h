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
#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <std::size_t TLen, bool TSignExtend, typename TBase>
class FixedLength : public TBase
{
    using BaseImpl = TBase;
    using BaseSerialisedType = typename BaseImpl::SerialisedType;
public:

    using ValueType = typename BaseImpl::ValueType;

    static_assert(TLen <= sizeof(BaseSerialisedType),
        "The provided length limit is too big");

    using SerialisedType = typename std::conditional<
        (TLen < sizeof(BaseSerialisedType)),
        typename comms::util::SizeToType<TLen, std::is_signed<BaseSerialisedType>::value>::Type,
        BaseSerialisedType
    >::type;

    using Endian = typename BaseImpl::Endian;

    FixedLength() = default;

    explicit FixedLength(const ValueType& val)
      : BaseImpl(val)
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
        return adjustToSerialised(BaseImpl::toSerialised(val), ConversionTag());
    }

    static constexpr ValueType fromSerialised(SerialisedType val)
    {
        return BaseImpl::fromSerialised(adjustFromSerialised(val, ConversionTag()));
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t size)
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
            comms::util::readData<SerialisedType, Length>(iter, Endian());
        BaseImpl::value() = fromSerialised(serialisedValue);
    }

    template <typename TIter>
    comms::ErrorStatus write(TIter& iter, std::size_t size) const
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
        BaseImpl::template writeData<Length>(toSerialised(BaseImpl::value()), iter);
    }

private:

    struct JustCastTag {};
    struct SignExtendTag {};
    struct UnsignedTag {};
    struct SignedTag {};

    using ConversionTag = typename std::conditional<
        (TLen < sizeof(SerialisedType)),
        SignExtendTag,
        JustCastTag
    >::type;


    using HasSignTag = typename std::conditional<
        std::is_signed<SerialisedType>::value && TSignExtend,
        SignedTag,
        UnsignedTag
    >::type;

    using UnsignedSerialisedType = typename std::make_unsigned<SerialisedType>::type;

    static constexpr SerialisedType adjustToSerialised(BaseSerialisedType val, JustCastTag)
    {
        return static_cast<SerialisedType>(val);
    }

    static SerialisedType adjustToSerialised(BaseSerialisedType val, SignExtendTag)
    {
        auto valueTmp =
            static_cast<UnsignedSerialisedType>(val) & UnsignedValueMask;

        return signExtUnsignedSerialised(valueTmp, HasSignTag());
    }

    static constexpr BaseSerialisedType adjustFromSerialised(SerialisedType val, JustCastTag)
    {
        return static_cast<BaseSerialisedType>(val);
    }

    static BaseSerialisedType adjustFromSerialised(SerialisedType val, SignExtendTag)
    {
        auto valueTmp = static_cast<UnsignedSerialisedType>(val) & UnsignedValueMask;
        return static_cast<BaseSerialisedType>(signExtUnsignedSerialised(valueTmp, HasSignTag()));
    }

    static constexpr SerialisedType signExtUnsignedSerialised(UnsignedSerialisedType val, UnsignedTag)
    {
        return static_cast<SerialisedType>(val);
    }

    static SerialisedType signExtUnsignedSerialised(UnsignedSerialisedType val, SignedTag)
    {
        static const UnsignedSerialisedType SignExtMask =
            ~((static_cast<UnsignedSerialisedType>(1U) << BitLength) - 1);
        static const UnsignedSerialisedType SignMask =
            static_cast<UnsignedSerialisedType>(1U) << (BitLength - 1);

        if ((val & SignMask) != 0) {
            val |= SignExtMask;
        }
        return static_cast<SerialisedType>(val);
    }

    static const std::size_t Length = TLen;
    static const std::size_t BitsInByte = std::numeric_limits<std::uint8_t>::digits;
    static const std::size_t BitLength = Length * BitsInByte;

    static const UnsignedSerialisedType UnsignedValueMask =
        static_cast<UnsignedSerialisedType>(
            (static_cast<std::uintmax_t>(1U) << BitLength) - 1);

    static_assert(0 < Length, "Length is expected to be greater than 0");
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


