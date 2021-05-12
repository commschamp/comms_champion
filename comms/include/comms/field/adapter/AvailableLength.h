//
// Copyright 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <algorithm>
#include <limits>

#include "comms/Assert.h"
#include "comms/util/SizeToType.h"
#include "comms/util/access.h"
#include "comms/util/type_traits.h"
#include "comms/ErrorStatus.h"
#include "comms/details/tag.h"
#include "comms/cast.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TBase>
class AvailableLength : public TBase
{
    using BaseImpl = TBase;
    using BaseSerialisedType = typename BaseImpl::SerialisedType;

public:

    using ValueType = typename BaseImpl::ValueType;
    using SerialisedType = typename BaseImpl::SerialisedType;

    using Endian = typename BaseImpl::Endian;

    AvailableLength() = default;

    explicit AvailableLength(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit AvailableLength(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    AvailableLength(const AvailableLength&) = default;
    AvailableLength(AvailableLength&&) = default;
    AvailableLength& operator=(const AvailableLength&) = default;
    AvailableLength& operator=(AvailableLength&&) = default;

    void setForcedLength(int len)
    {
        m_forcedLength = len;        
    }

    int getForcedLength() const
    {
        return m_forcedLength;
    }

    std::size_t length() const
    {
        if (m_forcedLength == 0) {
            return BaseImpl::length();
        }

        if (0 < m_forcedLength) {
            return std::min(BaseImpl::length(), static_cast<std::size_t>(m_forcedLength));
        }

        for (std::size_t len = 1U; len < sizeof(SerialisedType); ++len) {
            if (fitsLength(BaseImpl::value(), len)) {
                return len;
            }
        }

        return BaseImpl::length();
    }

    static constexpr std::size_t minLength()
    {
        return 1U;
    }

    static constexpr std::size_t maxLength()
    {
        return BaseImpl::maxLength();
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
        if (size == 0U) {
            return comms::ErrorStatus::NotEnoughData;
        }

        auto fromIter = iter;
        auto unsignedSerialized = util::readData<UnsignedSerialisedType>(iter, std::min(size, BaseImpl::maxLength()), Endian());
        auto len = static_cast<std::size_t>(std::distance(fromIter, iter));
        BaseImpl::value() = fromSerialised(signExtUnsignedSerialised(unsignedSerialized, len, HasSignTag()));
        return comms::ErrorStatus::Success;
    }

    static constexpr bool hasReadNoStatus()
    {
        return false;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    template <typename TIter>
    comms::ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (m_forcedLength == 0) {
            return BaseImpl::write(iter, size);
        }

        if (!BaseImpl::canWrite()) {
            return ErrorStatus::InvalidMsgData;
        }

        auto fieldLen = length();
        if (size < fieldLen) {
            return ErrorStatus::BufferOverflow;
        }

        auto serValue = toSerialised(BaseImpl::value());
        if (0 < m_forcedLength) {
            comms::util::writeData(serValue, fieldLen, iter, Endian());
            return comms::ErrorStatus::Success;
        }

        // variable length, based on value        
        std::size_t len = 1U;
        for (; len < sizeof(serValue); ++len) {
            if (!fitsLength(serValue, len)) {
                continue;
            }

            comms::util::writeData(serValue, len, iter, Endian());
            return comms::ErrorStatus::Success;
        }

        return BaseImpl::write(iter, size);
    }

    static constexpr bool hasWriteNoStatus()
    {
        return false;
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const = delete;

private:
    template <typename... TParams>
    using UnsignedTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using SignedTag = comms::details::tag::Tag2<>;

    using HasSignTag = 
        typename comms::util::LazyShallowConditional<
            std::is_signed<SerialisedType>::value
        >::template Type<
            SignedTag,
            UnsignedTag
        >;

    using UnsignedSerialisedType = typename std::make_unsigned<SerialisedType>::type;

    template <typename... TParams>
    static constexpr SerialisedType signExtUnsignedSerialised(
        UnsignedSerialisedType val,
        std::size_t,
        UnsignedTag<TParams...>)
    {
        return static_cast<SerialisedType>(val);
    }

    template <typename... TParams>
    static SerialisedType signExtUnsignedSerialised(
        UnsignedSerialisedType val,
        std::size_t bytesCount,
        SignedTag<TParams...>)
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


    template <typename... TParams>
    static SerialisedType getMinLimitedValue(std::size_t len, UnsignedTag<TParams...>)
    {
        static_cast<void>(len);
        return static_cast<SerialisedType>(0);      
    }

    template <typename... TParams>
    static SerialisedType getMinLimitedValue(std::size_t len, SignedTag<TParams...>)
    {
        COMMS_ASSERT(len < sizeof(UnsignedSerialisedType));
        auto mask = 
            ((static_cast<UnsignedSerialisedType>(1U) << ((len * BitsInByte) - 1)) - 1U);

        return static_cast<SerialisedType>(~static_cast<UnsignedSerialisedType>(mask));
    }

    template <typename... TParams>
    static SerialisedType getMaxLimitedValue(std::size_t len, UnsignedTag<TParams...>)
    {
        COMMS_ASSERT(len < sizeof(UnsignedSerialisedType));
        auto value = 
            ((static_cast<UnsignedSerialisedType>(1U) << (len * BitsInByte)) - 1U);
        return static_cast<SerialisedType>(value);
    }

    template <typename... TParams>
    static SerialisedType getMaxLimitedValue(std::size_t len, SignedTag<TParams...>)
    {
        COMMS_ASSERT(len < sizeof(UnsignedSerialisedType));
        auto value = 
            ((static_cast<UnsignedSerialisedType>(1U) << ((len * BitsInByte) - 1)) - 1U);
        return static_cast<SerialisedType>(value);
    }


    static bool fitsLength(SerialisedType val, std::size_t len)
    {
        if (sizeof(val) <= len) {
            return true;
        }

        auto minValue = getMinLimitedValue(len, HasSignTag());
        auto maxValue = getMaxLimitedValue(len, HasSignTag());
        return ((minValue <= val) && (val <= maxValue));
    }

    static const std::size_t BitsInByte = 
        std::numeric_limits<std::uint8_t>::digits; 


    int m_forcedLength = -1;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


