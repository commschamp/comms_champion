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
#include <algorithm>
#include <limits>

#include "details/AdapterBase.h"
#include "comms/Assert.h"
#include "comms/util/SizeToType.h"
#include "comms/util/access.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <std::size_t TMinLen, std::size_t TMaxLen, typename TNext>
class VarLength : public details::AdapterBaseT<TNext>
{
    typedef details::AdapterBaseT<TNext> Base;
    typedef typename Base::Next::SerialisedType NextSerialisedType;

public:

    typedef typename Base::ParamValueType ParamValueType;

    static_assert(TMaxLen <= sizeof(NextSerialisedType),
        "The provided max length limit is too big");

    typedef typename std::conditional<
        (TMaxLen < sizeof(NextSerialisedType)),
        typename comms::util::SizeToType<TMaxLen, std::is_signed<NextSerialisedType>::value>::Type,
        NextSerialisedType
    >::type SerialisedType;

    typedef typename Base::Endian Endian;

    VarLength() = default;

    explicit VarLength(ParamValueType value)
      : Base(value)
    {
    }

    VarLength(const VarLength&) = default;
    VarLength(VarLength&&) = default;
    VarLength& operator=(const VarLength&) = default;
    VarLength& operator=(VarLength&&) = default;


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

    std::size_t length() const
    {
        auto serValue =
            adjustToUnsignedSerialisedVarLength(toSerialised(getValue()));
        std::size_t len = 0U;
        while (0 < serValue) {
            serValue >>= VarLengthShift;
            ++len;
        }

        GASSERT(len <= maxLength());
        return std::max(std::size_t(MinLength), len);
    }

    static constexpr std::size_t minLength()
    {
        return MinLength;
    }

    static constexpr std::size_t maxLength()
    {
        return MaxLength;
    }

    static constexpr SerialisedType toSerialised(ParamValueType value)
    {
        return signExtUnsignedSerialised(
            adjustToUnsignedSerialisedVarLength(Base::toSerialised(value)),
            HasSignTag());
    }

    static constexpr ParamValueType fromSerialised(SerialisedType value)
    {
        return Base::fromSerialised(
            static_cast<NextSerialisedType>(
                signExtUnsignedSerialised(
                    adjustToUnsignedSerialisedVarLength(value),
                    HasSignTag())));
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        UnsignedSerialisedType value = 0;
        std::size_t byteCount = 0;
        while (true) {
            if (size == 0) {
                return ErrorStatus::NotEnoughData;
            }

            auto byte = comms::util::readData<std::uint8_t>(iter, Endian());
            auto byteValue = byte & VarLengthValueBitsMask;
            addByteToSerialisedValue(
                byteValue, byteCount, value, typename Base::Endian());

            ++byteCount;

            if ((byte & VarLengthContinueBit) == 0) {
                break;
            }

            if (MaxLength <= byteCount) {
                return ErrorStatus::ProtocolError;
            }
            --size;
        }

        if (byteCount < minLength()) {
            return ErrorStatus::ProtocolError;
        }

        auto adjustedValue = signExtUnsignedSerialised(value, byteCount, HasSignTag());
        Base::setValue(Base::fromSerialised(adjustedValue));
        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        auto value = adjustToUnsignedSerialisedVarLength(Base::toSerialised(Base::getValue()));
        std::size_t byteCount = 0;
        while (true) {
            if (size == 0) {
                return ErrorStatus::BufferOverflow;
            }

            auto byte = removeByteFromSerialisedValue(value, byteCount, Endian());
            auto mustStop = (value == 0);
            if (!mustStop) {
                GASSERT((byte & VarLengthContinueBit) == 0);
                byte |= VarLengthContinueBit;
            }

            comms::util::writeData(byte, iter, Endian());
            ++byteCount;

            if (mustStop) {
                break;
            }

            GASSERT(byteCount < maxLength());
            --size;
        }

        if (byteCount < minLength()) {
            while ((byteCount + 1) < minLength()) {
                comms::util::writeData(VarLengthContinueBit, iter, Endian());
                ++byteCount;
            }
            comms::util::writeData(std::uint8_t(0), iter, Endian());
        }

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

    static UnsignedSerialisedType adjustToUnsignedSerialisedVarLength(SerialisedType value)
    {
        static_assert(MaxLength <= sizeof(UnsignedSerialisedType),
            "MaxLength is expected to be shorter than size of serialised type.");

        static const auto ZeroBitsCount =
            ((sizeof(UnsignedSerialisedType) - MaxLength) * std::numeric_limits<std::uint8_t>::digits) + MaxLength;

        static const auto TotalBits =
            sizeof(UnsignedSerialisedType) * std::numeric_limits<std::uint8_t>::digits;

        static const auto Mask =
            (static_cast<UnsignedSerialisedType>(1U) << (TotalBits - ZeroBitsCount)) - 1;

        return static_cast<UnsignedSerialisedType>(value) & Mask;
    }

    static void addByteToSerialisedValueBigEndian(
        std::uint8_t byte,
        UnsignedSerialisedType& value)
    {
        GASSERT((byte & VarLengthContinueBit) == 0);
        value <<= VarLengthShift;
        value |= byte;
    }

    static void addByteToSerialisedValueLittleEndian(
        std::uint8_t byte,
        std::size_t byteCount,
        UnsignedSerialisedType& value)
    {
        GASSERT((byte & VarLengthContinueBit) == 0);
        auto shift =
            byteCount * VarLengthShift;
        value = (static_cast<SerialisedType>(byte) << shift) | value;
    }

    static void addByteToSerialisedValue(
        std::uint8_t byte,
        std::size_t byteCount,
        UnsignedSerialisedType& value,
        comms::traits::endian::Big)
    {
        static_cast<void>(byteCount);
        addByteToSerialisedValueBigEndian(byte, value);
    }

    static void addByteToSerialisedValue(
        std::uint8_t byte,
        std::size_t byteCount,
        UnsignedSerialisedType& value,
        comms::traits::endian::Little)
    {
        addByteToSerialisedValueLittleEndian(byte, byteCount, value);
    }

    static std::uint8_t removeByteFromSerialisedValueBigEndian(
        UnsignedSerialisedType& value,
        std::size_t byteCount)
    {
        static const auto Mask = ~(static_cast<SerialisedType>(VarLengthValueBitsMask));

        auto valueTmp = value;
        std::size_t shift = 0;
        std::size_t count = 0;
        while ((valueTmp & Mask) != 0) {
            valueTmp >>= VarLengthShift;
            shift += VarLengthShift;
            ++count;
        }

        if ((byteCount + count + 1) < MinLength) {
            return 0;
        }

        auto clearMask = ~(static_cast<SerialisedType>(VarLengthValueBitsMask) << shift);
        value &= clearMask;
        return static_cast<std::uint8_t>(valueTmp);
    }

    static std::uint8_t removeByteFromSerialisedValueLittleEndian(
        UnsignedSerialisedType& value)
    {
        auto byte = static_cast<std::uint8_t>(value & VarLengthValueBitsMask);
        value >>= VarLengthShift;
        return byte;
    }

    static std::uint8_t removeByteFromSerialisedValue(
        UnsignedSerialisedType& value,
        std::size_t byteCount,
        comms::traits::endian::Big)
    {
        return removeByteFromSerialisedValueBigEndian(value, byteCount);
    }

    static std::uint8_t removeByteFromSerialisedValue(
        UnsignedSerialisedType& value,
        std::size_t byteCount,
        comms::traits::endian::Little)
    {
        static_cast<void>(byteCount);
        return removeByteFromSerialisedValueLittleEndian(value);
    }

    static constexpr SerialisedType signExtUnsignedSerialised(UnsignedSerialisedType value, UnsignedTag)
    {
        return static_cast<SerialisedType>(value);
    }

    static constexpr SerialisedType signExtUnsignedSerialised(
        UnsignedSerialisedType value,
        std::size_t,
        UnsignedTag)
    {
        return static_cast<SerialisedType>(value);
    }

    static SerialisedType signExtUnsignedSerialised(UnsignedSerialisedType value, SignedTag)
    {
        static const auto ZeroBitsCount =
            ((sizeof(UnsignedSerialisedType) - MaxLength) * std::numeric_limits<std::uint8_t>::digits) + MaxLength;

        static const auto TotalBits =
            sizeof(UnsignedSerialisedType) * std::numeric_limits<std::uint8_t>::digits;

        static const auto Mask =
            (static_cast<UnsignedSerialisedType>(1U) << (TotalBits - ZeroBitsCount)) - 1;

        static const UnsignedSerialisedType SignExtMask = ~Mask;

        static const auto SignMask = (Mask + 1) >> 1;

        if ((value & SignMask) != 0) {
            value |= SignExtMask;
        }
        return static_cast<SerialisedType>(value);
    }

    static SerialisedType signExtUnsignedSerialised(
        UnsignedSerialisedType value,
        std::size_t byteCount,
        SignedTag)
    {
        auto zeroBitsCount =
         ((sizeof(UnsignedSerialisedType) - byteCount) * std::numeric_limits<std::uint8_t>::digits) + MaxLength;

        static const auto TotalBits =
         sizeof(UnsignedSerialisedType) * std::numeric_limits<std::uint8_t>::digits;

        auto mask =
         (static_cast<UnsignedSerialisedType>(1U) << (TotalBits - zeroBitsCount)) - 1;

        UnsignedSerialisedType signExtMask = ~mask;

        auto signMask = (mask + 1) >> 1;

        if ((value & signMask) != 0) {
            value |= signExtMask;
        }
        return static_cast<SerialisedType>(value);
    }

    static const std::size_t MinLength = TMinLen;
    static const std::size_t MaxLength = TMaxLen;
    static const std::size_t MaxBitLength =
        MaxLength * std::numeric_limits<std::uint8_t>::digits;
    static const std::size_t VarLengthShift = 7;
    static const std::uint8_t VarLengthValueBitsMask =
        (static_cast<std::uint8_t>(1U) << VarLengthShift) - 1;
    static const std::uint8_t VarLengthContinueBit =
        static_cast<std::uint8_t>(~(VarLengthValueBitsMask));

    static_assert(0 < MinLength, "MinLength is expected to be greater than 0");
    static_assert(MinLength <= MaxLength,
        "MinLength is expected to be no greater than MaxLength");
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


