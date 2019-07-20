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

#include "comms/util/Tuple.h"
#include "comms/util/SizeToType.h"
#include "comms/util/access.h"
#include "comms/Assert.h"
#include "comms/ErrorStatus.h"

#include "comms/field/IntValue.h"
#include "CommonFuncs.h"

namespace comms
{

namespace field
{

namespace basic
{

namespace details
{

template <typename TField, bool THasFixedBitLength>
struct BitfieldMemberLengthRetrieveHelper;

template <typename TField>
struct BitfieldMemberLengthRetrieveHelper<TField, true>
{
    static const std::size_t Value = TField::ParsedOptions::FixedBitLength;
};

template <typename TField>
struct BitfieldMemberLengthRetrieveHelper<TField, false>
{
    static const std::size_t Value =
            std::numeric_limits<typename TField::ValueType>::digits;
};

template <typename TField>
struct BitfieldMemberLengthRetriever
{
    static const std::size_t Value =
        BitfieldMemberLengthRetrieveHelper<TField, TField::ParsedOptions::HasFixedBitLengthLimit>::Value;
};

template <std::size_t TRem, typename TMembers>
class BitfieldBitLengthCalcHelper
{
    static const std::size_t Idx = std::tuple_size<TMembers>::value - TRem;
    using FieldType = typename std::tuple_element<Idx, TMembers>::type;

public:
    static const std::size_t Value =
        BitfieldBitLengthCalcHelper<TRem - 1, TMembers>::Value +
        BitfieldMemberLengthRetriever<FieldType>::Value;
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
    using FieldType = typename std::tuple_element<TIdx - 1, TMembers>::type;

    static const std::size_t PrevFieldSize = BitfieldMemberLengthRetriever<FieldType>::Value;

public:
    static const std::size_t Value = BitfieldPosRetrieveHelper<TIdx - 1, TMembers>::Value + PrevFieldSize;
};

template <typename TMembers>
struct BitfieldPosRetrieveHelper<0, TMembers>
{
public:
    static const std::size_t Value = 0;
};

template <std::size_t TIdx, typename TMembers>
constexpr std::size_t getMemberShiftPos()
{
    return BitfieldPosRetrieveHelper<TIdx, TMembers>::Value;
}


}  // namespace details

template <typename TFieldBase, typename TMembers>
class Bitfield : public TFieldBase
{
    using BaseImpl = TFieldBase;

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
    using SerialisedType = typename comms::util::SizeToType<Length, false>::Type;

    using FixedIntValueField =
        comms::field::IntValue<
            TFieldBase,
            SerialisedType,
            comms::option::def::FixedLength<Length>
        >;


    using SimpleIntValueField =
        comms::field::IntValue<
            TFieldBase,
            SerialisedType
        >;

    using IntValueField = typename std::conditional<
        ((Length & (Length - 1)) == 0),
        SimpleIntValueField,
        FixedIntValueField
    >::type;

public:
    using Endian = typename BaseImpl::Endian;
    using VersionType = typename BaseImpl::VersionType;
    using ValueType = TMembers;

    Bitfield() = default;
    explicit Bitfield(const ValueType& val)
      : members_(val)
    {
    }

    explicit Bitfield(ValueType&& val)
      : members_(std::move(val))
    {
    }

    const ValueType& value() const
    {
        return members_;
    }

    ValueType& value()
    {
        return members_;
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

        auto serValue = BaseImpl::template readData<SerialisedType, Length>(iter);
        ErrorStatus es = ErrorStatus::Success;
        comms::util::tupleForEachWithTemplateParamIdx(members_, ReadHelper(serValue, es));
        return es;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        auto serValue = BaseImpl::template readData<SerialisedType, Length>(iter);
        comms::util::tupleForEachWithTemplateParamIdx(members_, ReadNoStatusHelper(serValue));
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

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        SerialisedType serValue = 0;
        comms::util::tupleForEachWithTemplateParamIdx(members_, WriteNoStatusHelper(serValue));
        comms::util::writeData<Length>(serValue, iter, Endian());
    }


    constexpr bool valid() const
    {
        return comms::util::tupleAccumulate(members_, true, ValidHelper());
    }

    bool refresh()
    {
        return comms::util::tupleAccumulate(members_, false, RefreshHelper());
    }

    template <std::size_t TIdx>
    static constexpr std::size_t memberBitLength()
    {
        static_assert(
            TIdx < std::tuple_size<ValueType>::value,
            "Index exceeds number of fields");

        using FieldType = typename std::tuple_element<TIdx, ValueType>::type;
        return details::BitfieldMemberLengthRetriever<FieldType>::Value;
    }

    static constexpr bool isVersionDependent()
    {
        return CommonFuncs::areMembersVersionDependent<ValueType>();
    }

    static constexpr bool hasNonDefaultRefresh()
    {
        return CommonFuncs::doMembersMembersHaveNonDefaultRefresh<ValueType>();
    }

    bool setVersion(VersionType version)
    {
        return CommonFuncs::setVersionForMembers(value(), version);
    }

private:

    class ReadHelper
    {
    public:
        ReadHelper(SerialisedType val, ErrorStatus& es)
          : value_(val),
            es_(es) {}

        template <std::size_t TIdx, typename TFieldParam>
        void operator()(TFieldParam&& field)
        {
            if (es_ != comms::ErrorStatus::Success) {
                return;
            }

            using FieldType = typename std::decay<decltype(field)>::type;
            static const auto Pos = details::getMemberShiftPos<TIdx, ValueType>();
            static const auto Mask =
                (static_cast<SerialisedType>(1) << details::BitfieldMemberLengthRetriever<FieldType>::Value) - 1;

            auto fieldSerValue =
                static_cast<SerialisedType>((value_ >> Pos) & Mask);

            static_assert(FieldType::minLength() == FieldType::maxLength(),
                "Bitfield doesn't support members with variable length");

            static const std::size_t MaxLength = FieldType::maxLength();
            std::uint8_t buf[MaxLength];
            auto* writeIter = &buf[0];
            using FieldEndian = typename FieldType::Endian;
            comms::util::writeData<MaxLength>(fieldSerValue, writeIter, FieldEndian());

            const auto* readIter = &buf[0];
            es_ = field.read(readIter, MaxLength);
        }

    private:
        SerialisedType value_;
        ErrorStatus& es_;
    };

    class ReadNoStatusHelper
    {
    public:
        ReadNoStatusHelper(SerialisedType val)
          : value_(val) {}

        template <std::size_t TIdx, typename TFieldParam>
        void operator()(TFieldParam&& field)
        {
            using FieldType = typename std::decay<decltype(field)>::type;
            using FieldOptions = typename FieldType::ParsedOptions;
            static const auto Pos = details::getMemberShiftPos<TIdx, ValueType>();
            static const auto Mask =
                (static_cast<SerialisedType>(1) << FieldOptions::FixedBitLength) - 1;

            auto fieldSerValue =
                static_cast<SerialisedType>((value_ >> Pos) & Mask);

            static_assert(FieldType::minLength() == FieldType::maxLength(),
                "Bitfield doesn't support members with variable length");

            static const std::size_t MaxLength = FieldType::maxLength();
            std::uint8_t buf[MaxLength];
            auto* writeIter = &buf[0];
            using FieldEndian = typename FieldType::Endian;
            comms::util::writeData<MaxLength>(fieldSerValue, writeIter, FieldEndian());

            const auto* readIter = &buf[0];
            field.readNoStatus(readIter);
        }

    private:
        SerialisedType value_;
    };

    class WriteHelper
    {
    public:
        WriteHelper(SerialisedType& val, ErrorStatus& es)
          : value_(val),
            es_(es) {}

        template <std::size_t TIdx, typename TFieldParam>
        void operator()(TFieldParam&& field)
        {
            if (es_ != comms::ErrorStatus::Success) {
                return;
            }

            using FieldType = typename std::decay<decltype(field)>::type;

            static_assert(FieldType::minLength() == FieldType::maxLength(),
                "Bitfield supports fixed length members only.");

            static const std::size_t MaxLength = FieldType::maxLength();
            std::uint8_t buf[MaxLength];
            auto* writeIter = &buf[0];
            es_ = field.write(writeIter, MaxLength);
            if (es_ != comms::ErrorStatus::Success) {
                return;
            }

            using FieldEndian = typename FieldType::Endian;
            const auto* readIter = &buf[0];
            auto fieldSerValue = comms::util::readData<SerialisedType, MaxLength>(readIter, FieldEndian());

            static const auto Pos = details::getMemberShiftPos<TIdx, ValueType>();
            static const auto Mask =
                (static_cast<SerialisedType>(1) << details::BitfieldMemberLengthRetriever<FieldType>::Value) - 1;

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

    class WriteNoStatusHelper
    {
    public:
        WriteNoStatusHelper(SerialisedType& val)
          : value_(val) {}

        template <std::size_t TIdx, typename TFieldParam>
        void operator()(TFieldParam&& field)
        {

            using FieldType = typename std::decay<decltype(field)>::type;

            static_assert(FieldType::minLength() == FieldType::maxLength(),
                "Bitfield supports fixed length members only.");

            static const std::size_t MaxLength = FieldType::maxLength();
            std::uint8_t buf[MaxLength];
            auto* writeIter = &buf[0];
            field.writeNoStatus(writeIter);

            using FieldEndian = typename FieldType::Endian;
            const auto* readIter = &buf[0];
            auto fieldSerValue = comms::util::readData<SerialisedType, MaxLength>(readIter, FieldEndian());

            using FieldOptions = typename FieldType::ParsedOptions;
            static const auto Pos = details::getMemberShiftPos<TIdx, ValueType>();
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
    };

    struct ValidHelper
    {
        template <typename TFieldParam>
        bool operator()(bool soFar, const TFieldParam& field)
        {
            return soFar && field.valid();
        }
    };

    struct RefreshHelper
    {
        template <typename TFieldParam>
        bool operator()(bool soFar, TFieldParam& field)
        {
            return field.refresh() || soFar;
        }
    };


    ValueType members_;
};

}  // namespace basic

}  // namespace field

}  // namespace comms


