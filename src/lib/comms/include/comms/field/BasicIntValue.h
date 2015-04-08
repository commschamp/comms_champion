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

    /// @brief Length of serialised data
    static const std::size_t SerialisedLen = Base::SerialisedLen;

    /// @brief Offset to be applied when serialising
    static const auto Offset = Base::Offset;

    /// @brief Minimal Valid Value
    static const auto MinValidValue = Base::MinValidValue;

    /// @brief Maximal Valid Value
    static const auto MaxValidValue = Base::MaxValidValue;

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
    static constexpr std::size_t length()
    {
        return SerialisedLen;
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
        if (size < length()) {
            return ErrorStatus::NotEnoughData;
        }

        auto serialisedValue =
            Base::template readData<SerialisedType, SerialisedLen>(iter);
        setSerialisedValue(serialisedValue);
        return ErrorStatus::Success;
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
        if (size < length()) {
            return ErrorStatus::BufferOverflow;
        }

        Base::template writeData<SerialisedLen>(getSerialisedValue(), iter);
        return ErrorStatus::Success;
    }

    constexpr bool valid() const {
        static_cast<void>(this);
        typedef typename std::conditional<
            (std::numeric_limits<ValueType>::min() < MinValidValue),
            Compare,
            ReturnTrue
        >::type MinTag;

        typedef typename std::conditional<
            (MaxValidValue < std::numeric_limits<ValueType>::max()),
            Compare,
            ReturnTrue
        >::type MaxTag;

        return aboveMin(MinTag()) && belowMax(MaxTag());
    }

private:
    struct Compare {};
    struct ReturnTrue {};
    struct DefaultInitialisationTag {};
    struct CustomInitialisationTag {};

    bool aboveMin(Compare) const
    {
        return (MinValidValue <= value_);
    }

    static constexpr bool aboveMin(ReturnTrue)
    {
        return true;
    }

    bool belowMax(Compare) const
    {
        return (value_ <= MaxValidValue);
    }

    static constexpr bool belowMax(ReturnTrue)
    {
        return true;
    }

    void completeDefaultInitialisation(DefaultInitialisationTag)
    {
    }

    void completeDefaultInitialisation(CustomInitialisationTag)
    {
        typedef typename Base::DefaultValueInitialiser DefaultValueInitialiser;
        DefaultValueInitialiser()(*this);
    }


    ValueType value_;
};

// Implementation

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

