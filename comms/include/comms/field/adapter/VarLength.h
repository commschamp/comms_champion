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
#include <algorithm>
#include <limits>

#include "comms/Assert.h"
#include "comms/util/SizeToType.h"
#include "comms/util/access.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <std::size_t TMinLen, std::size_t TMaxLen, typename TBase>
class VarLength : public TBase
{
    using BaseImpl = TBase;
    using BaseSerialisedType = typename BaseImpl::SerialisedType;

public:

    using ValueType = typename BaseImpl::ValueType;

    static_assert(1U <= TMinLen, "Minimal length must be at least 1");
    static_assert(TMinLen < TMaxLen, "Maximal length must be greater than minimal");
    static_assert(TMaxLen <= sizeof(std::uint64_t), "Currently variable length greater than 8 bytes is not supported");

    using SerialisedType = 
        typename comms::util::SizeToType<TMaxLen, std::is_signed<BaseSerialisedType>::value>::Type;

    using Endian = typename BaseImpl::Endian;

    VarLength() = default;

    explicit VarLength(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit VarLength(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    VarLength(const VarLength&) = default;
    VarLength(VarLength&&) = default;
    VarLength& operator=(const VarLength&) = default;
    VarLength& operator=(VarLength&&) = default;


    std::size_t length() const
    {
        return lengthInternal(HasSignTag());
    }

    static constexpr std::size_t minLength()
    {
        return MinLength;
    }

    static constexpr std::size_t maxLength()
    {
        return MaxLength;
    }

    static constexpr SerialisedType toSerialised(ValueType val)
    {
        return static_cast<SerialisedType>(BaseImpl::toSerialised(val));
    }

    static constexpr ValueType fromSerialised(SerialisedType val)
    {
        return BaseImpl::fromSerialised(static_cast<BaseSerialisedType>(val));
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t size)
    {
        UnsignedSerialisedType val = 0;
        std::size_t bytesCount = 0;
        while (true) {
            if (size == 0) {
                return comms::ErrorStatus::NotEnoughData;
            }

            GASSERT(bytesCount < MaxLength);
            auto byte = comms::util::readData<std::uint8_t>(iter, Endian());
            auto byteValue = byte & VarLengthValueBitsMask;
            addByteToSerialisedValue(
                byteValue, bytesCount, val, typename BaseImpl::Endian());

            ++bytesCount;

            if ((byte & VarLengthContinueBit) == 0) {
                break;
            }

            if (MaxLength <= bytesCount) {
                return ErrorStatus::ProtocolError;
            }

            --size;
        }

        if (bytesCount < minLength()) {
            return ErrorStatus::ProtocolError;
        }

        auto adjustedValue = signExtUnsignedSerialised(val, bytesCount, HasSignTag());
        BaseImpl::value() = BaseImpl::fromSerialised(adjustedValue);
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

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
        return writeNoStatusInternal(toSerialised(BaseImpl::value()), iter, HasSignTag(), Endian());
    }

private:

    struct UnsignedTag {};
    struct SignedTag {};

    using HasSignTag = typename std::conditional<
        std::is_signed<SerialisedType>::value,
        SignedTag,
        UnsignedTag
    >::type;

    using UnsignedSerialisedType = typename std::make_unsigned<SerialisedType>::type;

    std::size_t lengthInternal(UnsignedTag) const
    {
        auto serValue = 
            static_cast<UnsignedSerialisedType>(toSerialised(BaseImpl::value()));
        std::size_t len = 0U;
        while (0 < serValue) {
            serValue >>= VarLengthShift;
            ++len;
        }

        COMMS_ASSERT(len <= maxLength());
        return std::max(std::size_t(minLength()), std::min(len, maxLength()));
    }

    std::size_t lengthInternal(SignedTag) const
    {
        auto serValue = toSerialised(BaseImpl::value());
        if (0 <= serValue) {
            // positive
            return lengthSignedPositiveInternal();
        }

        return lengthSignedNegativeInternal();
    }

    std::size_t lengthSignedNegativeInternal() const
    {
        auto serValue = toSerialised(BaseImpl::value());
        std::size_t len = 0U;
        std::uint8_t lastByte = 0U;
        while (serValue != static_cast<decltype(serValue)>(-1)) {
            auto unsignedSerValue = static_cast<UnsignedSerialisedType>(serValue);
            lastByte = static_cast<decltype(lastByte)>(unsignedSerValue & VarLengthValueBitsMask);
            unsignedSerValue >>= VarLengthShift;
            ++len;

            unsignedSerValue |= SignExtMask;
            serValue = static_cast<decltype(serValue)>(unsignedSerValue);
        }

        if ((lastByte & 0x40) == 0U) {
            // Sign hasn't been captured, add one more byte
            ++len;
        }

        COMMS_ASSERT(len <= maxLength());
        return std::max(std::size_t(minLength()), std::min(len, maxLength()));
    }

    std::size_t lengthSignedPositiveInternal() const
    {
        auto serValue = toSerialised(BaseImpl::value());
        std::size_t len = 0U;
        std::uint8_t lastByte = 0U;
        while (serValue != static_cast<decltype(serValue)>(0)) {
            auto unsignedSerValue = static_cast<UnsignedSerialisedType>(serValue);
            lastByte = static_cast<decltype(lastByte)>(unsignedSerValue & VarLengthValueBitsMask);
            unsignedSerValue >>= VarLengthShift;
            ++len;

            serValue = static_cast<decltype(serValue)>(unsignedSerValue);
        }

        if ((lastByte & 0x40) != 0U) {
            // Last data byte is 1, not be mistaken sign
            ++len;
        }

        COMMS_ASSERT(len <= maxLength());
        return std::max(std::size_t(minLength()), std::min(len, maxLength()));
    }


    template <typename TIter>
    static void writeNoStatusInternal(
        SerialisedType val, 
        TIter& iter, 
        UnsignedTag, 
        traits::endian::Little) 
    {
        auto unsignedVal = 
            static_cast<UnsignedSerialisedType>(val);
        UnsignedSerialisedType unsignedValToWrite = 0U;
        std::size_t bytesCount = 0;  

        auto isLastByte = 
            [&unsignedVal, &bytesCount]() -> bool
            {
                return 
                    ((unsignedVal == 0) && (MinLength <= bytesCount)) || 
                     (MaxLength <= bytesCount);
            };       

        while (!isLastByte()) {
            auto byte = static_cast<std::uint8_t>(unsignedVal & VarLengthValueBitsMask);
            unsignedVal >>= VarLengthShift;
            ++bytesCount;

            if (!isLastByte()) {
                byte |= VarLengthContinueBit;
            }

            unsignedValToWrite |= 
                (static_cast<UnsignedSerialisedType>(byte) << ((bytesCount - 1) * BitsInByte));
        }           

        auto len = std::max(minLength(), std::min(bytesCount, maxLength()));
        comms::util::writeData(unsignedValToWrite, len, iter, Endian());
    }

    template <typename TIter>
    static void writeNoStatusInternal(
        SerialisedType val, 
        TIter& iter, 
        UnsignedTag, 
        traits::endian::Big) 
    {
        auto unsignedVal = 
            static_cast<UnsignedSerialisedType>(val);
        UnsignedSerialisedType unsignedValToWrite = 0U;
        std::size_t bytesCount = 0;  

        auto isLastByte = 
            [&unsignedVal, &bytesCount]() -> bool
            {
                return 
                    ((unsignedVal == 0) && (MinLength <= bytesCount)) || 
                     (MaxLength <= bytesCount);
            };       

        while (!isLastByte()) {
            auto byte = static_cast<std::uint8_t>(unsignedVal & VarLengthValueBitsMask);
            unsignedVal >>= VarLengthShift;
            
            if (0 < bytesCount) {
                byte |= VarLengthContinueBit;
            }

            unsignedValToWrite |= 
                (static_cast<UnsignedSerialisedType>(byte) << (bytesCount * BitsInByte));

            ++bytesCount;
        }           

        auto len = std::max(minLength(), std::min(bytesCount, maxLength()));
        comms::util::writeData(unsignedValToWrite, len, iter, Endian());
    }    

    template <typename TIter, typename TEndian>
    static void writeNoStatusInternal(
        SerialisedType val, 
        TIter& iter, 
        SignedTag, 
        TEndian endian) 
    {
        if (static_cast<SerialisedType>(0) <= val) {
            return writePositiveNoStatusInternal(val, iter, endian);
        }

        return writeNegativeNoStatusInternal(val, iter, endian);
    }

    template <typename TIter>
    static void writeNegativeNoStatusInternal(
        SerialisedType val, 
        TIter& iter, 
        traits::endian::Little) 
    {
        UnsignedSerialisedType unsignedValToWrite = 0U;
        std::size_t bytesCount = 0;  

        auto isLastByte = 
            [&val, &bytesCount]() -> bool
            {
                return 
                    ((val == static_cast<SerialisedType>(-1)) && (MinLength <= bytesCount)) || 
                    (MaxLength <= bytesCount);
            };       

        while (!isLastByte()) {
            auto unsignedVal = static_cast<UnsignedSerialisedType>(val);
            auto byte = static_cast<std::uint8_t>(unsignedVal & VarLengthValueBitsMask);
            unsignedVal >>= VarLengthShift;
            ++bytesCount;
            unsignedVal |= SignExtMask;
            val = static_cast<decltype(val)>(unsignedVal);

            if (!isLastByte()) {
                byte |= VarLengthContinueBit;
            }
            else if (((byte & 0x40) == 0U) && (bytesCount < MaxLength)) {
                // Sign is not captured
                byte |= VarLengthContinueBit;
                unsignedValToWrite |= 
                    (static_cast<UnsignedSerialisedType>(byte) << ((bytesCount - 1) * BitsInByte));

                ++bytesCount;
                byte = VarLengthValueBitsMask;
            }

            unsignedValToWrite |= 
                (static_cast<UnsignedSerialisedType>(byte) << ((bytesCount - 1) * BitsInByte));
        }           

        auto len = std::max(minLength(), std::min(bytesCount, maxLength()));
        comms::util::writeData(unsignedValToWrite, len, iter, Endian());
    }

    template <typename TIter>
    static void writePositiveNoStatusInternal(
        SerialisedType val, 
        TIter& iter, 
        traits::endian::Little) 
    {
        UnsignedSerialisedType unsignedValToWrite = 0U;
        std::size_t bytesCount = 0;  

        auto isLastByte = 
            [&val, &bytesCount]() -> bool
            {
                return 
                    ((val == static_cast<SerialisedType>(0)) && (MinLength <= bytesCount)) || 
                    (MaxLength <= bytesCount);
            };       

        while (!isLastByte()) {
            auto unsignedVal = static_cast<UnsignedSerialisedType>(val);
            auto byte = static_cast<std::uint8_t>(unsignedVal & VarLengthValueBitsMask);
            unsignedVal >>= VarLengthShift;
            ++bytesCount;
            val = static_cast<decltype(val)>(unsignedVal);

            if (!isLastByte()) {
                byte |= VarLengthContinueBit;
            }
            else if (((byte & 0x40) != 0U) && (bytesCount < MaxLength)) {
                // data MSB is 1, may be confused with sign
                byte |= VarLengthContinueBit;
                unsignedValToWrite |= 
                    (static_cast<UnsignedSerialisedType>(byte) << ((bytesCount - 1) * BitsInByte));

                ++bytesCount;
                byte = 0U;
            }

            unsignedValToWrite |= 
                (static_cast<UnsignedSerialisedType>(byte) << ((bytesCount - 1) * BitsInByte));
        }           

        auto len = std::max(minLength(), std::min(bytesCount, maxLength()));
        comms::util::writeData(unsignedValToWrite, len, iter, Endian());
    }    

    template <typename TIter>
    static void writeNegativeNoStatusInternal(
        SerialisedType val, 
        TIter& iter, 
        traits::endian::Big)
    {
        UnsignedSerialisedType unsignedValToWrite = 0U;
        std::size_t bytesCount = 0;  

        auto isLastByte = 
            [&val, &bytesCount]() -> bool
            {
                return 
                    ((val == static_cast<SerialisedType>(-1)) && (MinLength <= bytesCount)) || 
                    (MaxLength <= bytesCount);
            };       

        while (!isLastByte()) {
            auto unsignedVal = static_cast<UnsignedSerialisedType>(val);
            auto byte = static_cast<std::uint8_t>(unsignedVal & VarLengthValueBitsMask);
            unsignedVal >>= VarLengthShift;
            unsignedVal |= SignExtMask;
            val = static_cast<decltype(val)>(unsignedVal);

            if (0U < bytesCount) {
                byte |= VarLengthContinueBit;
            }

            unsignedValToWrite |= 
                (static_cast<UnsignedSerialisedType>(byte) << (bytesCount * BitsInByte));
            ++bytesCount;
            

            if (isLastByte() && ((byte & 0x40) == 0U) && (bytesCount < MaxLength)) {
                // Sign is not captured
                byte = 0xff;
                unsignedValToWrite |= 
                    (static_cast<UnsignedSerialisedType>(byte) << (bytesCount * BitsInByte));
                ++bytesCount;
            }
        }           

        auto len = std::max(minLength(), std::min(bytesCount, maxLength()));
        comms::util::writeData(unsignedValToWrite, len, iter, Endian());
    }    

    template <typename TIter>
    static void writePositiveNoStatusInternal(
        SerialisedType val, 
        TIter& iter, 
        traits::endian::Big)
    {
        UnsignedSerialisedType unsignedValToWrite = 0U;
        std::size_t bytesCount = 0;  

        auto isLastByte = 
            [&val, &bytesCount]() -> bool
            {
                return 
                    ((val == static_cast<SerialisedType>(0)) && (MinLength <= bytesCount)) || 
                    (MaxLength <= bytesCount);
            };       

        while (!isLastByte()) {
            auto unsignedVal = static_cast<UnsignedSerialisedType>(val);
            auto byte = static_cast<std::uint8_t>(unsignedVal & VarLengthValueBitsMask);
            unsignedVal >>= VarLengthShift;
            val = static_cast<decltype(val)>(unsignedVal);

            if (0U < bytesCount) {
                byte |= VarLengthContinueBit;
            }
            unsignedValToWrite |= 
                (static_cast<UnsignedSerialisedType>(byte) << (bytesCount * BitsInByte));
            ++bytesCount;

            if (isLastByte() && ((byte & 0x40) != 0U) && (bytesCount < MaxLength)) {
                // MSB data bit may be confusted with sign, add one more byte
                byte = VarLengthContinueBit;

                unsignedValToWrite |= 
                    (static_cast<UnsignedSerialisedType>(byte) << (bytesCount * BitsInByte));
                ++bytesCount;
                break;
            }
        }           

        auto len = std::max(minLength(), std::min(bytesCount, maxLength()));
        comms::util::writeData(unsignedValToWrite, len, iter, Endian());
    }    


    static constexpr SerialisedType signExtUnsignedSerialised(
        UnsignedSerialisedType val,
        std::size_t,
        UnsignedTag)
    {
        return static_cast<SerialisedType>(val);
    }

    static SerialisedType signExtUnsignedSerialised(
        UnsignedSerialisedType val,
        std::size_t bytesCount,
        SignedTag)
    {
        UnsignedSerialisedType signBitMask = 
            static_cast<UnsignedSerialisedType>(1U) << ((bytesCount * BitsInByte) - (bytesCount + 1));

        if ((val & signBitMask) == 0U) {
            return static_cast<SerialisedType>(val);
        }

        UnsignedSerialisedType signExtMask = 
            static_cast<UnsignedSerialisedType>(~(signBitMask - 1));

        val |= signExtMask;
        return static_cast<SerialisedType>(val);
    }


    static void addByteToSerialisedValue(
        std::uint8_t byte,
        std::size_t byteCount,
        UnsignedSerialisedType& val,
        comms::traits::endian::Big)
    {
        static_cast<void>(byteCount);
        COMMS_ASSERT((byte & VarLengthContinueBit) == 0);
        val <<= VarLengthShift;
        val |= static_cast<UnsignedSerialisedType>(byte);
    }

    static void addByteToSerialisedValue(
        std::uint8_t byte,
        std::size_t byteCount,
        UnsignedSerialisedType& val,
        comms::traits::endian::Little)
    {
        COMMS_ASSERT((byte & VarLengthContinueBit) == 0);
        auto shift =
            byteCount * VarLengthShift;
        val = (static_cast<UnsignedSerialisedType>(byte) << shift) | val;
    }


    static const std::size_t MinLength = TMinLen;
    static const std::size_t MaxLength = TMaxLen;
    static const std::size_t VarLengthShift = 7;
    static const std::uint8_t VarLengthValueBitsMask =
        (static_cast<std::uint8_t>(1U) << VarLengthShift) - 1;
    static const std::uint8_t VarLengthContinueBit =
        static_cast<std::uint8_t>(~(VarLengthValueBitsMask));
    static const std::size_t BitsInByte = 
        std::numeric_limits<std::uint8_t>::digits; 
    static const std::size_t SerLengthInBits = 
        BitsInByte * sizeof(SerialisedType);    
    static const UnsignedSerialisedType SignExtMask = 
        static_cast<UnsignedSerialisedType>(
            static_cast<UnsignedSerialisedType>(~0) << (SerLengthInBits - VarLengthShift));

    static_assert(0 < MinLength, "MinLength is expected to be greater than 0");
    static_assert(MinLength <= MaxLength,
        "MinLength is expected to be no greater than MaxLength");
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


