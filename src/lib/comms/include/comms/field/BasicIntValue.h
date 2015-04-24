//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
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

/// @file comms/field/BasicIntValue.h
/// This file contains definition of basic integral value field that
/// can be used in message definition.

#pragma once

#include <type_traits>

#include "comms/ErrorStatus.h"
#include "comms/Assert.h"
#include "comms/util/SizeToType.h"
#include "comms/util/IntegralPromotion.h"

#include "details/BasicIntValueBase.h"

namespace comms
{

namespace field
{

/// @addtogroup comms
/// @{

/// @brief Defines "Basic Integral Value Field".
/// @details The class provides an API to access the value of the field as
///          well as to serialise/deserialise the value to/from stream buffer.
/// @tparam TField Base (interface) class for this field.
/// @tparam T Integral value type.
/// @tparam TLen Length of serialised data in bytes. Default value is sizeof(T).
/// @tparam TOff Offset to apply when serialising data.
/// @pre @code std::is_integral<T>::value == true @endcode
/// @headerfile comms/field/BasicIntValue.h
template <typename TField, typename T, typename... TOptions>
class BasicIntValue : public details::BasicIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral value");

    typedef details::BasicIntValueBase<TField, T, TOptions...> Base;

public:

    /// @brief Value Type
    typedef T ValueType;

    /// @brief Serialised Type
    typedef typename Base::SerialisedType SerialisedType;

    /// @brief Offset Type
    typedef typename Base::OffsetType OffsetType;

    /// @brief Default constructor
    /// @details Sets default value to be 0.
    BasicIntValue()
      : value_(static_cast<ValueType>(0))
    {
        typedef typename std::conditional<
            Base::HasCustomInitialiser,
            CustomInitialisationTag,
            DefaultInitialisationTag
        >::type Tag;
        completeDefaultInitialisation(Tag());
    }

    /// @brief Constructor
    /// @details Sets initial value.
    /// @param value Initial value
    explicit BasicIntValue(ValueType value)
      : value_(value)
    {
    }

    /// @brief Copy constructor is default
    BasicIntValue(const BasicIntValue&) = default;

    /// @brief Destructor is default
    ~BasicIntValue() = default;

    /// @brief Copy assignment is default
    BasicIntValue& operator=(const BasicIntValue&) = default;

    /// @brief Retrieve the value.
    const ValueType getValue() const
    {
        return value_;
    }

    /// @brief Set the value
    /// @param value Value to set.
    void setValue(ValueType value)
    {
        GASSERT(minValue() <= value);
        GASSERT(value <= maxValue());
        value_ = value;
    }

    /// @brief Retrieve serialised data
    const SerialisedType getSerialisedValue() const
    {
        return toSerialised(value_);
    }

    /// @brief Set serialised data
    void setSerialisedValue(SerialisedType value)
    {
        value_ = fromSerialised(value);
    }

    /// @brief Convert value to serialised data
    static constexpr const SerialisedType toSerialised(ValueType value)
    {
        return static_cast<SerialisedType>(Offset + value);
    }

    /// @brief Convert serialised data to actual value
    static constexpr const ValueType fromSerialised(SerialisedType value)
    {
        return static_cast<ValueType>((-Offset) + value);
    }

    /// @brief Get length of serialised data
    constexpr std::size_t length() const
    {
        return lengthInternal(LengthTag());
    }

    /// @brief Read the serialised field value from the some data structure.
    /// @tparam TIter Type of input iterator
    /// @param[in, out] iter Input iterator.
    /// @param[in] size Size of the data in iterated data structure.
    /// @return Status of the read operation.
    /// @pre Value of provided "size" must be less than or equal to
    ///      available data in the used data structure/stream
    /// @post The iterator will be incremented.
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        return readInternal(iter, size, LengthTag());
    }

    /// @brief Write the serialised field value to some data structure.
    /// @tparam TIter Type of output iterator
    /// @param[in, out] iter Output iterator.
    /// @param[in] size Size of the buffer, field data must fit it.
    /// @return Status of the write operation.
    /// @pre Value of provided "size" must be less than or equal to
    ///      available space in the data structure.
    /// @post The iterator will be incremented.
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        return writeInternal(iter, size, LengthTag());
    }

    constexpr bool valid() const {

        return validInternal(
            typename std::conditional<
                Base::HasCustomValidator,
                CustomValidatorTag,
                DefaultValidatorTag
            >::type());
    }

    static constexpr std::size_t maxLength()
    {
        return MaxLength;
    }

    static constexpr std::size_t minLength()
    {
        return MinLength;
    }

    static ValueType minValue()
    {
        return minValueInternal(LengthTag());
    }

    static ValueType maxValue()
    {
        return maxValueInternal(LengthTag());
    }

    static constexpr bool hasFixedLength()
    {
        return HasFixedLength;
    }

    static constexpr OffsetType serOffset()
    {
        return Offset;
    }

private:
    static const std::size_t MinLength = Base::MinLength;
    static const std::size_t MaxLength = Base::MaxLength;
    static const bool HasFixedLength = (MinLength == MaxLength);
    static const auto Offset = Base::Offset;

    struct DefaultInitialisationTag {};
    struct CustomInitialisationTag {};
    struct DefaultValidatorTag {};
    struct CustomValidatorTag {};
    struct FixedLengthTag {};
    struct VarLengthTag {};
    struct NoAdjustment {};
    struct ShorterLengthAdjustment {};

    typedef typename std::conditional<
        HasFixedLength,
        FixedLengthTag,
        VarLengthTag
    >::type LengthTag;

    typedef typename std::conditional<
        sizeof(SerialisedType) == MaxLength,
        NoAdjustment,
        ShorterLengthAdjustment
    >::type AdjustmentTag;

    typedef typename std::make_unsigned<SerialisedType>::type UnsignedSerType;

    void completeDefaultInitialisation(DefaultInitialisationTag)
    {
    }

    void completeDefaultInitialisation(CustomInitialisationTag)
    {
        typedef typename Base::DefaultValueInitialiser DefaultValueInitialiser;
        DefaultValueInitialiser()(*this);
    }

    static constexpr bool validInternal(DefaultValidatorTag)
    {
        return true;
    }

    constexpr bool validInternal(CustomValidatorTag) const
    {
        typedef typename Base::ContentsValidator ContentsValidator;
        return ContentsValidator()(*this);
    }

    constexpr std::size_t lengthInternal(FixedLengthTag) const
    {
        static_cast<void>(this);
        return MaxLength;
    }

    UnsignedSerType adjustToUnsignedSerialisedVarLength() const
    {
        static_assert(MaxLength <= sizeof(UnsignedSerType),
            "MaxLength is expected to be shorter than size of serialised type.");

        static const auto ZeroBitsCount =
            ((sizeof(UnsignedSerType) - MaxLength) * std::numeric_limits<std::uint8_t>::digits) + MaxLength;

        static const auto TotalBits =
            sizeof(UnsignedSerType) * std::numeric_limits<std::uint8_t>::digits;

        static const auto Mask =
            (static_cast<UnsignedSerType>(1U) << (TotalBits - ZeroBitsCount)) - 1;

        return static_cast<UnsignedSerType>(getSerialisedValue()) & Mask;
    }

    SerialisedType adjustFromUnsignedSerialisedVarLength(UnsignedSerType value)
    {
        static_assert(MaxLength <= sizeof(UnsignedSerType),
            "MaxLength is expected to be shorter than size of serialised type.");

        static const auto ZeroBitsCount =
            ((sizeof(UnsignedSerType) - MaxLength) * std::numeric_limits<std::uint8_t>::digits) + MaxLength;

        static const auto TotalBits =
            sizeof(UnsignedSerType) * std::numeric_limits<std::uint8_t>::digits;

        static const auto Mask =
            (static_cast<UnsignedSerType>(1U) << (TotalBits - ZeroBitsCount)) - 1;

        static const auto SignPos = (TotalBits - ZeroBitsCount) - 1;
        static const auto SignMask =
            static_cast<UnsignedSerType>(1U) << SignPos;

        bool negValue =
            (std::is_signed<SerialisedType>::value) &&
            ((value & SignMask) != 0);

        if ((std::is_signed<SerialisedType>::value) &&
            (negValue)) {
            value |= (~Mask);
        }

        return static_cast<SerialisedType>(value);
    }

    constexpr std::size_t lengthInternal(VarLengthTag) const
    {
        auto serValue = adjustToUnsignedSerialisedVarLength();

        std::size_t len = 0U;
        while (0 < serValue) {
            serValue >>= VarLengthShift;
            ++len;
        }

        return std::max(std::size_t(MinLength), len);
    }

    template <typename TIter>
    ErrorStatus readFixedLength(TIter& iter, std::size_t size)
    {
        if (size < minLength()) {
            return ErrorStatus::NotEnoughData;
        }

        auto serialisedValue =
            Base::template readData<SerialisedType, MaxLength>(iter);
        setSerialisedValue(serialisedValue);
        return ErrorStatus::Success;
    }

    static void addByteToSerialisedValueBigEndian(
        std::uint8_t byte,
        UnsignedSerType& value)
    {
        GASSERT((byte & VarLengthContinueBit) == 0);
        value <<= VarLengthShift;
        value |= byte;
    }

    static void addByteToSerialisedValueLittleEndian(
        std::uint8_t byte,
        std::size_t byteCount,
        UnsignedSerType& value)
    {
        GASSERT((byte & VarLengthContinueBit) == 0);
        auto shift =
            byteCount * VarLengthShift;
        value = (static_cast<SerialisedType>(byte) << shift) | value;
    }

    static void addByteToSerialisedValue(
        std::uint8_t byte,
        std::size_t byteCount,
        UnsignedSerType& value,
        comms::traits::endian::Big)
    {
        static_cast<void>(byteCount);
        addByteToSerialisedValueBigEndian(byte, value);
    }

    static void addByteToSerialisedValue(
        std::uint8_t byte,
        std::size_t byteCount,
        UnsignedSerType& value,
        comms::traits::endian::Little)
    {
        addByteToSerialisedValueLittleEndian(byte, byteCount, value);
    }

    template <typename TIter>
    ErrorStatus readVarLength(TIter& iter, std::size_t size)
    {
        UnsignedSerType value = 0;
        std::size_t byteCount = 0;
        while (true) {
            if (size == 0) {
                return ErrorStatus::NotEnoughData;
            }
            auto byte = Base::template readData<std::uint8_t>(iter);
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
            std::advance(iter, minLength() - byteCount);
        }

        auto adjustedValue = adjustFromUnsignedSerialisedVarLength(value);
        setSerialisedValue(adjustedValue);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus readInternal(TIter& iter, std::size_t size, FixedLengthTag)
    {
        return readFixedLength(iter, size);
    }

    template <typename TIter>
    ErrorStatus readInternal(TIter& iter, std::size_t size, VarLengthTag)
    {
        return readVarLength(iter, size);
    }

    template <typename TIter>
    ErrorStatus writeFixedLength(TIter& iter, std::size_t size) const
    {
        if (size < length()) {
            return ErrorStatus::BufferOverflow;
        }

        Base::template writeData<MaxLength>(getSerialisedValue(), iter);
        return ErrorStatus::Success;
    }

    static std::uint8_t removeByteFromSerialisedValueBigEndian(
        UnsignedSerType& value)
    {
        static const auto Mask = ~(static_cast<SerialisedType>(VarLengthValueBitsMask));

        auto valueTmp = value;
        std::size_t shift = 0;
        while ((valueTmp & Mask) != 0) {
            valueTmp >>= VarLengthShift;
            shift += VarLengthShift;
        }

        auto clearMask = ~(static_cast<SerialisedType>(VarLengthValueBitsMask) << shift);
        value &= clearMask;
        return static_cast<std::uint8_t>(valueTmp);
    }

    static std::uint8_t removeByteFromSerialisedValueLittleEndian(
        UnsignedSerType& value)
    {
        auto byte = static_cast<std::uint8_t>(value & VarLengthValueBitsMask);
        value >>= VarLengthShift;
        return byte;
    }

    static std::uint8_t removeByteFromSerialisedValue(
        UnsignedSerType& value,
        comms::traits::endian::Big)
    {
        return removeByteFromSerialisedValueBigEndian(value);
    }

    static std::uint8_t removeByteFromSerialisedValue(
        UnsignedSerType& value,
        comms::traits::endian::Little)
    {
        return removeByteFromSerialisedValueLittleEndian(value);
    }

    template <typename TIter>
    ErrorStatus writeVarLength(TIter& iter, std::size_t size) const
    {
        auto value = adjustToUnsignedSerialisedVarLength();
        std::size_t byteCount = 0;
        while (true) {
            if (size == 0) {
                return ErrorStatus::BufferOverflow;
            }

            auto byte = removeByteFromSerialisedValue(value, typename Base::Endian());
            auto mustStop = (value == 0);
            if (!mustStop) {
                GASSERT((byte & VarLengthContinueBit) == 0);
                byte |= VarLengthContinueBit;
            }

            Base::writeData(byte, iter);
            ++byteCount;

            if (mustStop) {
                break;
            }

            GASSERT(byteCount < maxLength());
            --size;
        }

        GASSERT(
            (std::is_same<typename Base::Endian, comms::traits::endian::Little>::value) ||
            (minLength() <= byteCount));

        while (byteCount < minLength()) {
            Base::writeData(std::uint8_t(0), iter);
            ++byteCount;
        }

        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus writeInternal(TIter& iter, std::size_t size, FixedLengthTag) const
    {
        return writeFixedLength(iter, size);
    }

    template <typename TIter>
    ErrorStatus writeInternal(TIter& iter, std::size_t size, VarLengthTag) const
    {
        return writeVarLength(iter, size);
    }

    static constexpr SerialisedType adjustSerialisedFixedLength(
        SerialisedType value,
        NoAdjustment)
    {
        return value;
    }

    static SerialisedType adjustSerialisedFixedLength(
        SerialisedType value,
        ShorterLengthAdjustment)
    {
        static const auto Shift =
            MaxLength * std::numeric_limits<std::uint8_t>::digits;
        static const auto Mask =
            (static_cast<UnsignedSerType>(1) << Shift) - 1;
        static const auto SignBitMask =
            (static_cast<UnsignedSerType>(1) << (Shift - 1));

        auto unsignedResult = static_cast<UnsignedSerType>(value) & Mask;
        if ((std::is_signed<SerialisedType>::value) &&
            ((unsignedResult & SignBitMask) != 0U)) {
            unsignedResult |= ~(Mask);
        }

        return static_cast<SerialisedType>(unsignedResult);
    }

    static ValueType minValueFixedLength()
    {
        auto minSer = std::numeric_limits<SerialisedType>::min();
        if (std::is_unsigned<SerialisedType>::value && (0 < serOffset())) {
            minSer = static_cast<SerialisedType>(
                std::max(static_cast<OffsetType>(minSer), serOffset()));
        }

        auto adjustedSerialised = adjustSerialisedFixedLength(minSer, AdjustmentTag());
        return fromSerialised(adjustedSerialised);
    }

    static ValueType minValueInternal(FixedLengthTag)
    {
        return minValueFixedLength();
    }

    static SerialisedType adjustSerialisedVarLength(
        SerialisedType value)
    {
        auto fixedAdjusted = adjustSerialisedFixedLength(value, AdjustmentTag());
        static const auto TotalBits =
            sizeof(SerialisedType) * std::numeric_limits<std::uint8_t>::digits;
        static const auto SignPos = TotalBits - 1;
        static const auto SignMask =
            static_cast<UnsignedSerType>(1U) << SignPos;

        bool negValue =
            (std::is_signed<SerialisedType>::value) &&
            ((fixedAdjusted & SignMask) != 0);

        static const auto Shift = MaxLength;

        static const auto Mask =
            (static_cast<UnsignedSerType>(1) << (TotalBits - Shift)) - 1;

        auto unsignedResult =
            (static_cast<UnsignedSerType>(fixedAdjusted) >> Shift) & Mask;

        if (negValue) {
            unsignedResult |= ~(Mask);
        }

        return static_cast<SerialisedType>(unsignedResult);
    }

    static ValueType minValueVarLength()
    {
        auto value = std::numeric_limits<SerialisedType>::min();
        if (std::is_unsigned<SerialisedType>::value) {
            return fromSerialised(static_cast<SerialisedType>(0U));
        }

        auto adjustedSerialised = adjustSerialisedVarLength(value);
        return fromSerialised(adjustedSerialised);
    }

    static ValueType minValueInternal(VarLengthTag)
    {
        return minValueVarLength();
    }

    static ValueType maxValueFixedLength()
    {
        auto value = std::numeric_limits<SerialisedType>::max();
        auto adjustedSerialised = adjustSerialisedFixedLength(value, AdjustmentTag());
        return fromSerialised(adjustedSerialised);
    }

    static ValueType maxValueVarLength()
    {
        static const auto value = std::numeric_limits<SerialisedType>::max();
        static_assert(
            0 < value,
            "Expected maximal value to be positive.");
        auto adjustedSerialised = adjustSerialisedVarLength(value);
        return fromSerialised(adjustedSerialised);
    }

    static ValueType maxValueInternal(FixedLengthTag)
    {
        return maxValueFixedLength();
    }

    static ValueType maxValueInternal(VarLengthTag)
    {
        return maxValueVarLength();
    }

    static const unsigned VarLengthShift = 7;
    static const std::uint8_t VarLengthValueBitsMask =
        (static_cast<std::uint8_t>(1U) << VarLengthShift) - 1;
    static const std::uint8_t VarLengthContinueBit =
        static_cast<std::uint8_t>(~(VarLengthValueBitsMask));

    ValueType value_;
};

/// @brief Equality comparison operator.
/// @related BasicIntValue
template <typename... TArgs>
bool operator==(
    const BasicIntValue<TArgs...>& field1,
    const BasicIntValue<TArgs...>& field2)
{
    return field1.getValue() == field2.getValue();
}

/// @brief Non-equality comparison operator.
/// @related BasicIntValue
template <typename... TArgs>
bool operator!=(
    const BasicIntValue<TArgs...>& field1,
    const BasicIntValue<TArgs...>& field2)
{
    return field1.getValue() != field2.getValue();
}

/// @brief Equivalence comparison operator.
/// @related BasicIntValue
template <typename... TArgs>
bool operator<(
    const BasicIntValue<TArgs...>& field1,
    const BasicIntValue<TArgs...>& field2)
{
    return field1.getValue() < field2.getValue();
}

namespace details
{

template <typename T>
struct IsBasicIntValue
{
    static const bool Value = false;
};

template <typename... TArgs>
struct IsBasicIntValue<comms::field::BasicIntValue<TArgs...> >
{
    static const bool Value = true;
};

}  // namespace details

template <typename T>
constexpr bool isBasicIntValue()
{
    return details::IsBasicIntValue<T>::Value;
}

/// @}

}  // namespace field

}  // namespace comms

