//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <limits>

#include "comms/Assert.h"
#include "comms/util/SizeToType.h"
#include "comms/util/type_traits.h"
#include "comms/ErrorStatus.h"
#include "comms/details/tag.h"

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

    using SerialisedType = 
        typename comms::util::Conditional<
            (TLen < sizeof(BaseSerialisedType))
        >::template Type<
            typename comms::util::SizeToType<TLen, std::is_signed<BaseSerialisedType>::value>::Type,
            BaseSerialisedType
        >;

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
        return adjustToSerialised(BaseImpl::toSerialised(val), ConversionTag<>());
    }

    static constexpr ValueType fromSerialised(SerialisedType val)
    {
        return BaseImpl::fromSerialised(adjustFromSerialised(val, ConversionTag<>()));
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

    template <typename... TParams>
    using JustCastTag = comms::details::tag::Tag1<TParams...>;

    template <typename... TParams>
    using SignExtendTag = comms::details::tag::Tag2<TParams...>;    

    template <typename... TParams>
    using UnsignedTag = comms::details::tag::Tag3<TParams...>;  

    template <typename... TParams>
    using SignedTag = comms::details::tag::Tag4<TParams...>;     

    template <typename...>
    using ConversionTag = 
        typename comms::util::LazyShallowConditional<
            (TLen < sizeof(SerialisedType))
        >::template Type<
            SignExtendTag,
            JustCastTag
        >;

    template <typename...>
    using HasSignTag = 
        typename comms::util::LazyShallowConditional<
            std::is_signed<SerialisedType>::value && TSignExtend
        >::template Type<
            SignedTag,
            UnsignedTag
        >;

    using UnsignedSerialisedType = typename std::make_unsigned<SerialisedType>::type;

    template <typename... TParams>
    static constexpr SerialisedType adjustToSerialised(BaseSerialisedType val, JustCastTag<TParams...>)
    {
        return static_cast<SerialisedType>(val);
    }

    template <typename... TParams>
    static SerialisedType adjustToSerialised(BaseSerialisedType val, SignExtendTag<TParams...>)
    {
        auto valueTmp =
            static_cast<UnsignedSerialisedType>(val) & UnsignedValueMask;

        return signExtUnsignedSerialised(valueTmp, HasSignTag<>());
    }

    template <typename... TParams>
    static constexpr BaseSerialisedType adjustFromSerialised(SerialisedType val, JustCastTag<TParams...>)
    {
        return static_cast<BaseSerialisedType>(val);
    }

    template <typename... TParams>
    static BaseSerialisedType adjustFromSerialised(SerialisedType val, SignExtendTag<TParams...>)
    {
        auto valueTmp = static_cast<UnsignedSerialisedType>(val) & UnsignedValueMask;
        return static_cast<BaseSerialisedType>(signExtUnsignedSerialised(valueTmp, HasSignTag<>()));
    }

    template <typename... TParams>
    static constexpr SerialisedType signExtUnsignedSerialised(UnsignedSerialisedType val, UnsignedTag<TParams...>)
    {
        return static_cast<SerialisedType>(val);
    }

    template <typename... TParams>
    static SerialisedType signExtUnsignedSerialised(UnsignedSerialisedType val, SignedTag<TParams...>)
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


