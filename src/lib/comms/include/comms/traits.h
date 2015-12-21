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

/// @file comms/traits.h
/// This file contains all the classes necessary to properly
/// define message traits.


#pragma once

#include "util/access.h"

namespace comms
{

namespace traits
{

namespace endian
{

/// @brief Empty class used in traits to indicate Big Endian.
typedef util::traits::endian::Big Big;

/// @brief Empty class used in traits to indicate Little Endian.
typedef util::traits::endian::Little Little;

}  // namespace endian

}  // namespace traits

}  // namespace comms

