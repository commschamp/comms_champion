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

template <typename T, typename TIter>
void assign(T& obj, TIter from, TIter to)
{
    details::AssignHelper::assign(obj, from, to);
}

} // namespace util

} // namespace comms

