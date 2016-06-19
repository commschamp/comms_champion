//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

#include <ratio>
#include <limits>
#include <type_traits>

#include "comms/ErrorStatus.h"
#include "comms/Assert.h"
#include "basic/NoValue.h"
#include "details/AdaptBasicField.h"

namespace comms
{

namespace field
{

/// @brief Dummy field with "do-nothing" read/write operations.
/// @details Can be used with other classes that require field types.
/// @tparam TFieldBase Base class for this field, expected to be a variant of
///     comms::Field.
template <typename TFieldBase>
class NoValue : public TFieldBase
{
    typedef TFieldBase Base;

    typedef basic::NoValue<TFieldBase> BasicField;
    typedef details::AdaptBasicFieldT<BasicField> ThisField;

    static_assert(std::is_base_of<comms::field::category::NumericValueField, typename ThisField::Category>::value,
        "ThisField is expected to be of NumericFieldCategory");
public:

    /// @brief All the options provided to this class bundled into struct.
    typedef details::OptionsParser<> ParsedOptions;

    /// @brief Type of underlying value.
    /// @details Defined to be "unsigned", not really used
    typedef typename ThisField::ValueType ValueType;

    /// @brief Default constructor
    /// @details Initialises internal value to 0.
    NoValue() = default;

    /// @brief Constructor
    explicit NoValue(ValueType val)
    {
        value() = val;
    }

    /// @brief Copy constructor
    NoValue(const NoValue&) = default;

    /// @brief Copy assignment
    NoValue& operator=(const NoValue&) = default;

    /// @brief Get access to the value storage.
    /// @details Should not really be used.
    /// @return Reference to a static value. All the independent get/set
    ///     operations on the different @ref NoValue fields access the same
    ///     static value.
    static ValueType& value()
    {
        return ThisField::value();
    }

    /// @brief Get length required to serialise the current field value.
    /// @return Always 0.
    static constexpr std::size_t length()
    {
        return ThisField::length();
    }

    /// @brief Get minimal length that is required to serialise field of this type.
    /// @return Always 0.
    static constexpr std::size_t minLength()
    {
        return length();
    }

    /// @brief Get maximal length that is required to serialise field of this type.
    /// @return Always 0.
    static constexpr std::size_t maxLength()
    {
        return length();
    }

    /// @brief Check validity of the field value.
    /// @details Always reported as valid.
    static constexpr bool valid()
    {
        return ThisField::valid();
    }

    /// @brief Read field value from input data sequence.
    /// @details The function does nothing, always reporting success.
    /// @param[in, out] iter Iterator to read the data.
    /// @param[in] size Number of bytes available for reading.
    /// @return Status of read operation.
    template <typename TIter>
    static ErrorStatus read(TIter& iter, std::size_t size)
    {
        return ThisField::read(iter, size);
    }

    /// @brief Write current field value to output data sequence
    /// @details The function does nothing, always reporting success.
    /// @param[in, out] iter Iterator to write the data.
    /// @param[in] size Maximal number of bytes that can be written.
    /// @return Status of write operation.
    template <typename TIter>
    static ErrorStatus write(TIter& iter, std::size_t size)
    {
        return ThisField::write(iter, size);
    }
};

/// @brief Equality comparison operator.
/// @details To @ref NoValue fields are always equal.
/// @return Always true.
/// @related NoValue
template <typename TFieldBase>
bool operator==(const NoValue<TFieldBase>& field1, const NoValue<TFieldBase>& field2)
{
    static_cast<void>(field1);
    static_cast<void>(field2);
    return true;
}

/// @brief Non-equality comparison operator.
/// @details To @ref NoValue fields are always equal.
/// @return Always false.
/// @related NoValue
template <typename TFieldBase>
bool operator!=(const NoValue<TFieldBase>& field1, const NoValue<TFieldBase>& field2)
{
    return !(field1 == field2);
}

/// @brief Equivalence comparison operator.
/// @details To @ref NoValue fields are always equal.
/// @return Always false.
/// @related NoValue
template <typename TFieldBase>
bool operator<(const NoValue<TFieldBase>& field1, const NoValue<TFieldBase>& field2)
{
    static_cast<void>(field1);
    static_cast<void>(field2);
    return false;
}

namespace details
{

template <typename T>
struct IsNoValue
{
    static const bool Value = false;
};

template <typename TFieldBase>
struct IsNoValue<comms::field::NoValue<TFieldBase> >
{
    static const bool Value = true;
};

}  // namespace details

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::NoValue.
/// @tparam T Any type.
/// @related comms::field::NoValue
template <typename T>
constexpr bool isNoValue()
{
    return details::IsNoValue<T>::Value;
}

}  // namespace field

}  // namespace comms



