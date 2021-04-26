//
// Copyright 2020 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief Provides helper assign() function to allow easy assignment of values to collections or views.

#pragma once

#include "details/AssignHelper.h"

namespace comms
{

namespace util
{

/// @brief Assigns a new value to provided object
/// @details The function detects at <b>compile-time</b> presence of 
///     assign() member function and uses it. In case the assign()
///     member function does not exist (for types like @b std::string_view or @b std::span)
///     the function checks for presence of a constructor that can receive a pointer to 
///     the first element + number of elements (size) and uses it instead to create temporary
///     object and then uses move / copy constructor to assign the value. 
///     @code
///     static const std::string Str("hello");
///
///     std::string s1;
///     comms::util::assign(s1, Str.begin(), Str.end()) // <-- equivalent to s1.assign(Str.begin(), Str.end());
///
///     std::string_view s2;
///     comms::util::assign(s2, Str.begin(), Str.end()) // <-- equivalent to s2 = std::string_view(&(*Str.begin()), std::distance(Str.begin(), Str.end()));
///     @endcode
///     
template <typename T, typename TIter>
void assign(T& obj, TIter from, TIter to)
{
    details::AssignHelper<>::assign(obj, from, to);
}

} // namespace util

} // namespace comms

