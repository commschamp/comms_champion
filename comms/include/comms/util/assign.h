//
// Copyright 2020 (C). Alex Robenko. All rights reserved.
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
    details::AssignHelper::assign(obj, from, to);
}

} // namespace util

} // namespace comms

