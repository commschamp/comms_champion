//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
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
/// @brief Various compile-time detection functions of whether specific member functions and/or types exist

#pragma once

#include "comms/util/details/detect.h"

namespace comms
{

namespace util
{

namespace detect
{

/// @brief Detect whether provided type has @b clear() member function
/// @details
///     @code
///         static_assert(comms::util::detect::hasClearFunc<std::string>(), "std::string is expected to have clear() member function.");
///     @endcode
template <typename T>
constexpr bool hasClearFunc()
{
    return details::HasClearFunc<T>::Value;
}

/// @brief Detect whether provided type has @b reserve() member function
/// @details
///     @code
///         static_assert(comms::util::detect::hasReserveFunc<std::string>(), "std::string is expected to have reserve() member function.");
///     @endcode
template <typename T>
constexpr bool hasReserveFunc()
{
    return details::HasReserveFunc<T>::Value;
}

/// @brief Detect whether provided type has @b resize() member function
/// @details
///     @code
///         static_assert(comms::util::detect::hasResizeFunc<std::string>(), "std::string is expected to have resize() member function.");
///     @endcode
template <typename T>
constexpr bool hasResizeFunc()
{
    return details::HasResizeFunc<T>::Value;
}

/// @brief Detect whether provided type has @b remove_suffix() member function
/// @details
///     @code
///         static_assert(comms::util::detect::hasRemoveSuffixFunc<comms::util::StringView>, 
///             "comms::util::StringView is expected to have remove_suffix() member function.");
///         static_assert(comms::util::detect::hasRemoveSuffixFunc<std::string_view>, 
///             "std::string_view is expected to have remove_suffix() member function.");
///     @endcode
template <typename T>
constexpr bool hasRemoveSuffixFunc()
{
    return details::HasRemoveSuffixFunc<T>::Value;
}

/// @brief Detect whether provided type has @b assign() member function which can
///     receive two pointers in its arguments
/// @details
///     @code
///         static_assert(comms::util::detect::HasAssignFunc<std::string>(), "std::string is expected to have assign() member function.");
///     @endcode
template <typename T>
constexpr bool hasAssignFunc()
{
    return details::HasAssignFunc<T>::Value;
}

/// @brief Detect whether provided type has a constructor that receives a pointer + size parameters.
/// @details
///     @code
///         static_assert(comms::util::detect::hasPtrSizeConstructor<std::string_view>(), 
///             "std::string_view is expected to have ptr + size constructor.");
///     @endcode
template <typename T>
constexpr bool hasPtrSizeConstructor()
{
    return details::HasPtrSizeConstructor<T>::Value;
}

} // namespace detect

} // namespace util

} // namespace comms