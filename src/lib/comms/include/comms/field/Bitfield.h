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

#include "comms/util/Tuple.h"
#include "comms/util/SizeToType.h"
#include "comms/util/access.h"
#include "comms/Assert.h"
#include "comms/ErrorStatus.h"

#include "details/OptionsParser.h"

#include "IntValue.h"

namespace comms
{

namespace field
{

namespace details
{

template <std::size_t TRem, typename TMembers>
class BitfieldBitLengthCalcHelper
{
    static const auto Idx = std::tuple_size<TMembers>::value - TRem;
    typedef typename std::tuple_element<Idx, TMembers>::type FieldType;
    typedef typename FieldType::ParsedOptions FieldOptions;

    static_assert(FieldOptions::HasFixedBitLengthLimit,
        "Expects every bitfield member to define its length in bits. Use FixedBitLength option.");

    static const auto ThisFieldSize = FieldOptions::FixedBitLength;

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
struct BitfieldPosRetrieveHelper
{
    static_assert(TIdx < std::tuple_size<TMembers>::value, "Invalid tuple element");
    typedef typename std::tuple_element<TIdx - 1, TMembers>::type FieldType;
    typedef typename FieldType::ParsedOptions FieldOptions;

    static const auto PrevFieldSize = FieldOptions::FixedBitLength;

public:
    static const auto Value = BitfieldPosRetrieveHelper<TIdx - 1, TMembers>::Value + PrevFieldSize;
};

template <typename TMembers>
struct BitfieldPosRetrieveHelper<0, TMembers>
{
public:
    static const auto Value = 0;
};

template <std::size_t TIdx, typename TMembers>
constexpr std::size_t getMemberShiftPos()
{
    return BitfieldPosRetrieveHelper<TIdx, TMembers>::Value;
}


}  // namespace details

template <typename TFieldBase, typename TMembers, typename... TOptions>
class Bitfield : public TFieldBase
{
    typedef TFieldBase Base;

    static_assert(comms::util::IsTuple<TMembers>::Value, "TMembers is expected to be a tuple of BitfieldMember<...>");

    static_assert(
        1U < std::tuple_size<TMembers>::value,
        "Number of members is expected to be at least 2.");


    static const std::size_t TotalBits = details::calcBitLength<TMembers>();
    static_assert(
        (TotalBits % std::numeric_limits<std::uint8_t>::digits) == 0,
        "Wrong number of total bits");

    static const std::size_t Length = TotalBits / std::numeric_limits<std::uint8_t>::digits;
    static_assert(0U < Length, "Serialised length is expected to be greater than 0");
    typedef typename comms::util::SizeToType<Length, false>::Type SerialisedType;

    typedef
        IntValue<
            TFieldBase,
            SerialisedType,
            comms::option::FixedLength<Length>
        > FixedIntValueField;


    typedef
        IntValue<
            TFieldBase,
            SerialisedType
        > SimpleIntValueField;

    typedef typename std::conditional<
        ((Length % 0x1) == 0),
        SimpleIntValueField,
        FixedIntValueField
    >::type IntValueField;

public:
    typedef details::OptionsParser<TOptions...> ParsedOptions;
    typedef typename Base::Endian Endian;
    typedef TMembers Members;

    typedef SerialisedType ValueType;

    Members& members()
    {
        return members_;
    }

    const Members& members() const
    {
        return members_;
    }

    ValueType getValue() const
    {
        std::uint8_t buf[Length] = {0};
        auto* writeIter = &buf[0];
        auto es = write(writeIter, Length);
        static_cast<void>(es);
        GASSERT(es == comms::ErrorStatus::Success);
        const auto* readIter = &buf[0];
        return comms::util::readData<ValueType, Length>(readIter, Endian());
    }

    void setValue(ValueType value)
    {
        std::uint8_t buf[Length] = {0};
        auto* writeIter = &buf[0];
        comms::util::writeData<Length>(value, writeIter, Endian());
        const auto* readIter = &buf[0];
        auto es = read(readIter, Length);
        static_cast<void>(es);
        GASSERT(es == comms::ErrorStatus::Success);
    }

    template <std::size_t TIdx>
    static constexpr std::size_t memberBitLength()
    {
        static_assert(
            TIdx < std::tuple_size<Members>::value,
            "Index exceeds number of fields");

        typedef typename std::tuple_element<TIdx, Members>::type FieldType;
        typedef typename FieldType::ParsedOptions FieldOptions;
        return FieldOptions::FixedBitLength;
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

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        if (size < length()) {
            return ErrorStatus::NotEnoughData;
        }

        auto serValue = Base::template readData<SerialisedType, Length>(iter);
        ErrorStatus es = ErrorStatus::Success;
        comms::util::tupleForEachWithTemplateParamIdx(members_, ReadHelper(serValue, es));
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
        comms::util::tupleForEachWithTemplateParamIdx(members_, WriteHelper(serValue, es));
        if (es == ErrorStatus::Success) {
            comms::util::writeData<Length>(serValue, iter, Endian());
        }
        return es;
    }

    constexpr bool valid() const {
        return
            comms::util::tupleAccumulate(members_, true, ValidHelper()) &&
            extraValidation(ExtraValidationTag());
    }

private:

    struct NoExtraValidationTag {};
    struct ExtraValidationExists {};
    typedef typename std::conditional<
        ParsedOptions::HasCustomValidator,
        ExtraValidationExists,
        NoExtraValidationTag
    >::type ExtraValidationTag;

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
            typedef typename FieldType::ParsedOptions FieldOptions;
            static const auto Pos = details::getMemberShiftPos<TIdx, Members>();
            static const auto Mask =
                (static_cast<SerialisedType>(1) << FieldOptions::FixedBitLength) - 1;

            auto fieldSerValue =
                static_cast<SerialisedType>((value_ >> Pos) & Mask);

            static_assert(FieldType::minLength() == FieldType::maxLength(),
                "Bitfield doesn't support members with variable length");

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

            static_assert(FieldType::minLength() == FieldType::maxLength(),
                "Bitfield supports fixed length members only.");

            static const std::size_t MaxLength = FieldType::maxLength();
            std::uint8_t buf[MaxLength];
            auto* writeIter = &buf[0];
            es_ = field.write(writeIter, MaxLength);
            if (es_ != comms::ErrorStatus::Success) {
                return;
            }

            typedef typename FieldType::Endian FieldEndian;
            const auto* readIter = &buf[0];
            auto fieldSerValue = comms::util::readData<SerialisedType, MaxLength>(readIter, FieldEndian());

            typedef typename FieldType::ParsedOptions FieldOptions;
            static const auto Pos = details::getMemberShiftPos<TIdx, Members>();
            static const auto Mask =
                (static_cast<SerialisedType>(1) << FieldOptions::FixedBitLength) - 1;

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

    static constexpr bool extraValidation(NoExtraValidationTag) {
        return true;
    }

    constexpr bool extraValidation(ExtraValidationExists) const {
        typedef typename ParsedOptions::CustomValidator Validator;
        return Validator()(*this);
    }

    Members members_;
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


