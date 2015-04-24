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

#include "details/BitfieldBase.h"
#include "comms/util/Tuple.h"
#include "comms/util/SizeToType.h"
#include "comms/util/access.h"
#include "comms/Assert.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

template <typename TField, std::size_t TSize>
struct BitfieldMember
{
    typedef TField Field;
    static const std::size_t Size = TSize;
};

namespace details
{

template <typename TMember>
struct IsBitfieldMember
{
    static const bool Value = false;
};

template <typename TField, std::size_t TSize>
struct IsBitfieldMember<comms::field::BitfieldMember<TField, TSize> >
{
    static const bool Value = true;
};

template <std::size_t TRem, typename TMembers>
class BitfieldMembersToFieldsHelper
{
    static const auto Idx = std::tuple_size<TMembers>::value - TRem;
    typedef typename std::tuple_element<Idx, TMembers>::type WrapperType;
    static_assert(IsBitfieldMember<WrapperType>::Value, "Expects BitfieldMember to specify members.");

    typedef typename std::decay<WrapperType>::type DecayedWrapperType;
    typedef typename DecayedWrapperType::Field FieldType;
    typedef std::tuple<FieldType> ThisFieldTuple;

    typedef typename BitfieldMembersToFieldsHelper<TRem - 1, TMembers>::Type OtherFieldsTuple;

    typedef decltype(std::tuple_cat(std::declval<ThisFieldTuple>(), std::declval<OtherFieldsTuple>())) TupleCatResult;

public:
    typedef typename std::decay<TupleCatResult>::type Type;

    static_assert(comms::util::IsTuple<Type>::Value, "Type is expected to be tuple");
};

template <typename TMembers>
class BitfieldMembersToFieldsHelper<0, TMembers>
{
public:
    typedef std::tuple<> Type;

    static_assert(comms::util::IsTuple<Type>::Value, "Type is expected to be tuple");
};

template <typename TMembers>
using MembersToFieldsT = typename BitfieldMembersToFieldsHelper<std::tuple_size<TMembers>::value, TMembers>::Type;

template <std::size_t TRem, typename TMembers>
class BitfieldBitLengthCalcHelper
{
    static const auto Idx = std::tuple_size<TMembers>::value - TRem;
    typedef typename std::tuple_element<Idx, TMembers>::type WrapperType;
    static_assert(IsBitfieldMember<WrapperType>::Value, "Expects BitfieldMember to specify members.");

    typedef typename std::decay<WrapperType>::type DecayedWrapperType;
    static const auto ThisFieldSize = DecayedWrapperType::Size;

public:
    static const auto Value = BitfieldBitLengthCalcHelper<TRem - 1, TMembers>::Value + ThisFieldSize;
};

template <typename TMembers>
class BitfieldBitLengthCalcHelper<0, TMembers>
{
public:
    static const std::size_t Value = 0;
};

template <typename TMembers>
constexpr std::size_t calcBitLength()
{
    return BitfieldBitLengthCalcHelper<std::tuple_size<TMembers>::value, TMembers>::Value;
}

template <std::size_t TIdx, typename TMembers>
struct BitfieldSizeRetrieveHelper;

template <std::size_t TIdx, typename TFirst, typename... TRest>
struct BitfieldSizeRetrieveHelper<TIdx, std::tuple<TFirst, TRest...> >
{
    static const auto Value = BitfieldSizeRetrieveHelper<TIdx - 1, std::tuple<TRest...> >::Value;
};

template <typename TFirst, typename... TRest>
struct BitfieldSizeRetrieveHelper<0, std::tuple<TFirst, TRest...> >
{
    static_assert(IsBitfieldMember<TFirst>::Value, "Expects BitfieldMember to specify members.");
    static const auto Value = TFirst::Size;
};

template <std::size_t TIdx, typename TMembers>
constexpr std::size_t getMemberBitLength()
{
    return BitfieldSizeRetrieveHelper<TIdx, TMembers>::Value;
}

template <std::size_t TIdx, typename TMembers, bool TFromMsb>
struct BitfieldPosRetrieveHelper;

template <std::size_t TIdx, typename TMembers>
struct BitfieldPosRetrieveHelper<TIdx, TMembers, false>
{
    static_assert(TIdx < std::tuple_size<TMembers>::value, "Invalid tuple element");
    typedef typename std::tuple_element<TIdx - 1, TMembers>::type WrapperType;
    static_assert(IsBitfieldMember<WrapperType>::Value, "Expects BitfieldMember to specify members.");

    typedef typename std::decay<WrapperType>::type DecayedWrapperType;
    static const auto PrevFieldSize = DecayedWrapperType::Size;

public:
    static const auto Value = BitfieldPosRetrieveHelper<TIdx - 1, TMembers, false>::Value + PrevFieldSize;
};

template <typename TMembers>
struct BitfieldPosRetrieveHelper<0, TMembers, false>
{
public:
    static const auto Value = 0;
};

template <std::size_t TIdx, typename TMembers>
struct BitfieldPosRetrieveHelper<TIdx, TMembers, true>
{
    static const auto Value = BitfieldBitLengthCalcHelper<(std::tuple_size<TMembers>::value - TIdx) - 1, TMembers>::Value;
};

template <std::size_t TIdx, typename TMembers, bool TFromMsb>
constexpr std::size_t getMemberShiftPos()
{
    return BitfieldPosRetrieveHelper<TIdx, TMembers, TFromMsb>::Value;
}


}  // namespace details

template <typename TField, typename TMembers, typename... TOptions>
class Bitfield:
    public details::BitfieldBase<TField, TOptions...>
{
    typedef details::BitfieldBase<TField, TOptions...> Base;

    static_assert(comms::util::IsTuple<TMembers>::Value, "TMembers is expected to be a tuple of BitfieldMember<...>");

    static_assert(
        1U < std::tuple_size<TMembers>::value,
        "Number of members is expected to be at least 2.");


    static const std::size_t TotalBits = details::calcBitLength<TMembers>();
    static_assert(
        (TotalBits % std::numeric_limits<std::uint8_t>::digits) == 0,
        "Wrong number of total bits");

    static const std::size_t SerialisedLen = TotalBits / std::numeric_limits<std::uint8_t>::digits;
    static_assert(0U < SerialisedLen, "Serialised length is expected to be greater than 0");

public:
    typedef typename Base::Endian Endian;
    typedef details::MembersToFieldsT<TMembers> Fields;

    typedef typename comms::util::SizeToType<SerialisedLen, false>::Type SerialisedType;
    typedef SerialisedType ValueType;

    Fields& fields()
    {
        return fields_;
    }

    const Fields& fields() const
    {
        return fields_;
    }

    ValueType getValue() const
    {
        std::uint8_t buf[SerialisedLen] = {0};
        auto* writeIter = &buf[0];
        auto es = write(writeIter, SerialisedLen);
        static_cast<void>(es);
        GASSERT(es == comms::ErrorStatus::Success);
        const auto* readIter = &buf[0];
        return comms::util::readData<ValueType, SerialisedLen>(readIter, Endian());
    }

    void setValue(ValueType value)
    {
        std::uint8_t buf[SerialisedLen] = {0};
        auto* writeIter = &buf[0];
        comms::util::writeData<SerialisedLen>(value, writeIter, Endian());
        const auto* readIter = &buf[0];
        auto es = read(readIter, SerialisedLen);
        static_cast<void>(es);
        GASSERT(es == comms::ErrorStatus::Success);
    }

    template <std::size_t TIdx>
    static constexpr std::size_t fieldBitLength()
    {
        static_assert(
            TIdx < std::tuple_size<Members>::value,
            "Index exceeds number of fields");

        return details::getMemberBitLength<TIdx, Members>();
    }

    static constexpr std::size_t length()
    {
        return SerialisedLen;
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        if (size < length()) {
            return ErrorStatus::NotEnoughData;
        }

        auto serValue = Base::template readData<SerialisedType, SerialisedLen>(iter);
        ErrorStatus es = ErrorStatus::Success;
        comms::util::tupleForEachWithTemplateParamIdx(fields_, ReadHelper(serValue, es));
        return es;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (size < length()) {
            return ErrorStatus::BufferOverflow;
        }

        SerialisedType serValue = 0;
        ErrorStatus es = ErrorStatus::Success;
        comms::util::tupleForEachWithTemplateParamIdx(fields_, WriteHelper(serValue, es));
        if (es == ErrorStatus::Success) {
            comms::util::writeData<SerialisedLen>(serValue, iter, Endian());
        }
        return es;
    }

    constexpr bool valid() const {
        return comms::util::tupleAccumulate(fields_, true, ValidHelper());
    }

    static constexpr bool hasFixedLength()
    {
        return true;
    }


private:

    typedef TMembers Members;

    static const bool IndexingFromMsb = Base::IndexingFromMsb;

    class ReadHelper
    {
    public:
        ReadHelper(SerialisedType value, ErrorStatus& es)
          : value_(value),
            es_(es) {}

        template <std::size_t TIdx, typename TFieldParam>
        void operator()(TFieldParam&& field)
        {
            if (es_ != comms::ErrorStatus::Success) {
                return;
            }

            typedef typename std::decay<decltype(field)>::type FieldType;
            typedef typename FieldType::SerialisedType FieldSerialisedType;
            static const auto Pos = details::getMemberShiftPos<TIdx, Members, IndexingFromMsb>();
            static const auto Mask =
                (static_cast<SerialisedType>(1) << details::getMemberBitLength<TIdx, Members>()) - 1;

            auto fieldSerValue =
                static_cast<FieldSerialisedType>((value_ >> Pos) & Mask);
            if (std::is_signed<FieldSerialisedType>::value) {
                static const auto SignBitMask = (Mask + 1) >> 1;
                if ((fieldSerValue & SignBitMask) != 0) {
                    typedef typename std::make_unsigned<FieldSerialisedType>::type UnsignedFieldSerialisedType;
                    auto SignExtMask =
                        static_cast<UnsignedFieldSerialisedType>(~Mask);
                    fieldSerValue =
                        static_cast<FieldSerialisedType>(
                            static_cast<UnsignedFieldSerialisedType>(fieldSerValue) | SignExtMask);
                }
            }

            static_assert(FieldType::hasFixedLength(), "Bitfield supports fixed length members only.");

            static const std::size_t MaxLength = FieldType::maxLength();
            std::uint8_t buf[MaxLength];
            auto* writeIter = &buf[0];
            typedef typename FieldType::Endian FieldEndian;
            comms::util::writeData<MaxLength>(fieldSerValue, writeIter, FieldEndian());

            const auto* readIter = &buf[0];
            es_ = field.read(readIter, MaxLength);
        }

    private:
        SerialisedType value_;
        ErrorStatus& es_;
    };

    class WriteHelper
    {
    public:
        WriteHelper(SerialisedType& value, ErrorStatus& es)
          : value_(value),
            es_(es) {}

        template <std::size_t TIdx, typename TFieldParam>
        void operator()(TFieldParam&& field)
        {
            if (es_ != comms::ErrorStatus::Success) {
                return;
            }

            typedef typename std::decay<decltype(field)>::type FieldType;
            typedef typename FieldType::SerialisedType FieldSerialisedType;

            static_assert(FieldType::hasFixedLength(), "Bitfield supports fixed length members only.");

            static const std::size_t MaxLength = FieldType::maxLength();
            std::uint8_t buf[MaxLength];
            auto* writeIter = &buf[0];
            es_ = field.write(writeIter, MaxLength);
            if (es_ != comms::ErrorStatus::Success) {
                return;
            }

            typedef typename FieldType::Endian FieldEndian;
            const auto* readIter = &buf[0];
            auto fieldSerValue = comms::util::readData<FieldSerialisedType, MaxLength>(readIter, FieldEndian());

            static const auto Pos = details::getMemberShiftPos<TIdx, Members, IndexingFromMsb>();
            static const auto Mask =
                (static_cast<SerialisedType>(1) << details::getMemberBitLength<TIdx, Members>()) - 1;

            static const auto ClearMask = ~(Mask << Pos);

            auto valueMask =
                (static_cast<SerialisedType>(fieldSerValue) & Mask) << Pos;

            value_ &= ClearMask;
            value_ |= valueMask;
        }


    private:
        SerialisedType& value_;
        ErrorStatus& es_;
    };

    struct ValidHelper
    {
        template <typename TFieldParam>
        bool operator()(bool soFar, TFieldParam&& field)
        {
            return soFar && field.valid();
        }
    };

    static_assert(
        comms::util::IsTuple<Fields>::Value,
        "Fields must be tuple.");

    static_assert(
        std::tuple_size<Fields>::value == std::tuple_size<Members>::value,
        "Fields must be tuple.");

    Fields fields_;
};

/// @brief Equality comparison operator.
/// @related Bitfield
template <typename... TArgs>
bool operator==(
    const Bitfield<TArgs...>& field1,
    const Bitfield<TArgs...>& field2)
{
    return field1.getValue() == field2.getValue();
}

/// @brief Non-equality comparison operator.
/// @related Bitfield
template <typename... TArgs>
bool operator!=(
    const Bitfield<TArgs...>& field1,
    const Bitfield<TArgs...>& field2)
{
    return field1.getValue() != field2.getValue();
}

/// @brief Equivalence comparison operator.
/// @related Bitfield
template <typename... TArgs>
bool operator<(
    const Bitfield<TArgs...>& field1,
    const Bitfield<TArgs...>& field2)
{
    return field1.getValue() < field2.getValue();
}

namespace details
{

template <typename T>
struct IsBitfield
{
    static const bool Value = false;
};

template <typename... TArgs>
struct IsBitfield<comms::field::Bitfield<TArgs...> >
{
    static const bool Value = true;
};

}  // namespace details

template <typename T>
constexpr bool isBitfield()
{
    return details::IsBitfield<T>::Value;
}


}  // namespace field

}  // namespace comms


