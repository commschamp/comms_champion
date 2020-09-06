//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
    // return details::PresenceDetector<void, details::HasClearOp, T>::Value;
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