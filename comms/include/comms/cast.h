//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
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

/// @file
/// Contains definition of various casts

#pragma once

#include "comms/details/ValueAssignWrapper.h"
#include "comms/details/FieldCastHelper.h"

namespace comms
{


/// @brief Helper function to assign value with static_cast to
///     appropriate type.
/// @details Can be used to assign any value, including value of a field.
///     @code
///     auto someValue = ...;
///     comms::cast_assign(field.value()) = someValue; // Sets field's value with appropriate static_cast.
///     @endcode
/// @note Defined in "comms/cast.h" headerfile
template <typename T>
details::ValueAssignWrapper<T> cast_assign(T& value)
{
    return details::ValueAssignWrapper<T>(value);
}

/// @brief Cast between fields.
/// @details Sometimes the protocol may treat some specific field differently
///     based on indication bit in some other field. This function can be
///     used to cast one field to another type.
///     @code
///     SomeFieldType someField = comms::field_cast<SomeFieldType>(someOtherField);
///     @endcode
/// @tparam TFieldTo Type to cast to.
/// @tparam TFieldFrom Type to cast from.
/// @param[in] field Original field
/// @return Field of the new type with internal value equivalent to one of the
///     original field.
/// @note Defined in "comms/cast.h" headerfile
template <typename TFieldTo, typename TFieldFrom>
TFieldTo field_cast(const TFieldFrom& field)
{
    static const bool Convertible =
        std::is_convertible<typename TFieldFrom::ValueType, typename TFieldTo::ValueType>::value;
    static const bool FromIntegral =
        std::is_enum<typename TFieldFrom::ValueType>::value ||
        std::is_integral<typename TFieldFrom::ValueType>::value;
    static const bool ToIntegral =
        std::is_enum<typename TFieldTo::ValueType>::value ||
        std::is_integral<typename TFieldTo::ValueType>::value;

    static const bool UseStaticCast =
        Convertible || (FromIntegral && ToIntegral);

    return details::FieldCastHelper<TFieldTo, TFieldFrom, UseStaticCast>::cast(field);
}


} // namespace comms
