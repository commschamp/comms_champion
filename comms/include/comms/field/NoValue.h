//
// Copyright 2016 - 2019 (C). Alex Robenko. All rights reserved.
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
#include "tag.h"

namespace comms
{

namespace field
{

/// @brief Dummy field with "do-nothing" read/write operations.
/// @details Can be used with other classes that require field types.
/// @tparam TFieldBase Base class for this field, expected to be a variant of
///     comms::Field.
/// @extends comms::Field
/// @headerfile comms/field/NoValue.h
template <typename TFieldBase>
class NoValue : private details::AdaptBasicFieldT<basic::NoValue<TFieldBase> >
{
    using BaseImpl = details::AdaptBasicFieldT<basic::NoValue<TFieldBase> >;
public:

    /// @brief Endian used for serialisation.
    using Endian = typename BaseImpl::Endian;

    /// @brief Version type
    using VersionType = typename BaseImpl::VersionType;

    /// @brief All the options provided to this class bundled into struct.
    using ParsedOptions = details::OptionsParser<>;

    /// @brief Tag indicating type of the field
    using Tag = tag::NoValue;

    /// @brief Type of underlying value.
    /// @details Defined to be "unsigned", not really used
    using ValueType = typename BaseImpl::ValueType;

    /// @brief Default constructor
    /// @details Initialises internal value to 0.
    NoValue() = default;

    /// @brief Constructor
    explicit NoValue(ValueType val)
    {
        BaseImpl::value() = val;
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
        return BaseImpl::value();
    }

    /// @brief Get length required to serialise the current field value.
    /// @return Always 0.
    static constexpr std::size_t length()
    {
        return BaseImpl::length();
    }

    /// @brief Get minimal length that is required to serialise field of this type.
    /// @return Always 0.
    static constexpr std::size_t minLength()
    {
        return BaseImpl::minLength();
    }

    /// @brief Get maximal length that is required to serialise field of this type.
    /// @return Always 0.
    static constexpr std::size_t maxLength()
    {
        return BaseImpl::maxLength();
    }

    /// @brief Check validity of the field value.
    bool valid() const
    {
        return BaseImpl::valid();
    }

    /// @brief Refresh the field's value
    /// @return @b true if the value has been updated, @b false otherwise
    bool refresh()
    {
        return BaseImpl::refresh();
    }

    /// @brief Read field value from input data sequence.
    /// @details The function does nothing, always reporting success.
    /// @param[in, out] iter Iterator to read the data.
    /// @param[in] size Number of bytes available for reading.
    /// @return Status of read operation.
    template <typename TIter>
    static ErrorStatus read(TIter& iter, std::size_t size)
    {
        return BaseImpl::read(iter, size);
    }

    /// @brief Read field value from input data sequence without error check and status report.
    /// @details Similar to @ref read(), but doesn't perform any correctness
    ///     checks and doesn't report any failures.
    /// @param[in, out] iter Iterator to read the data.
    /// @post Iterator is advanced.
    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        BaseImpl::readNoStatus(iter);
    }

    /// @brief Write current field value to output data sequence
    /// @details The function does nothing, always reporting success.
    /// @param[in, out] iter Iterator to write the data.
    /// @param[in] size Maximal number of bytes that can be written.
    /// @return Status of write operation.
    template <typename TIter>
    static ErrorStatus write(TIter& iter, std::size_t size)
    {
        return BaseImpl::write(iter, size);
    }

    /// @brief Write current field value to output data sequence  without error check and status report.
    /// @details Similar to @ref write(), but doesn't perform any correctness
    ///     checks and doesn't report any failures.
    /// @param[in, out] iter Iterator to write the data.
    /// @post Iterator is advanced.
    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        BaseImpl::writeNoStatus(iter);
    }

    /// @brief Compile time check if this class is version dependent
    static constexpr bool isVersionDependent()
    {
        return ParsedOptions::HasCustomVersionUpdate || BaseImpl::isVersionDependent();
    }

    /// @brief Compile time check if this class has non-default refresh functionality
    static constexpr bool hasNonDefaultRefresh()
    {
        return BaseImpl::hasNonDefaultRefresh();
    }

    /// @brief Default implementation of version update.
    /// @return @b true in case the field contents have changed, @b false otherwise
    bool setVersion(VersionType version)
    {
        return BaseImpl::setVersion(version);
    }

protected:
    using BaseImpl::readData;
    using BaseImpl::writeData;

private:
#ifdef _MSC_VER
    // VS compiler has problems having 0 size objects in tuple.
    int dummy_ = 0;
#endif
};

/// @brief Equality comparison operator.
/// @details To @ref NoValue fields are always equal.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
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
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return Always false.
/// @related NoValue
template <typename TFieldBase>
bool operator!=(const NoValue<TFieldBase>& field1, const NoValue<TFieldBase>& field2)
{
    return !(field1 == field2);
}

/// @brief Equivalence comparison operator.
/// @details To @ref NoValue fields are always equal.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return Always false.
/// @related NoValue
template <typename TFieldBase>
bool operator<(const NoValue<TFieldBase>& field1, const NoValue<TFieldBase>& field2)
{
    static_cast<void>(field1);
    static_cast<void>(field2);
    return false;
}

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::NoValue.
/// @tparam T Any type.
/// @return true in case provided type is any variant of @ref NoValue
/// @related comms::field::NoValue
template <typename T>
constexpr bool isNoValue()
{
    return std::is_same<typename T::Tag, tag::NoValue>::value;
}

}  // namespace field

}  // namespace comms



