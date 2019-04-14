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

/// @file comms/field/OptionalMode.h
/// Contains definition of the mode used for comms::field::Optional fields.

#pragma once

namespace comms
{

namespace field
{

/// @brief Mode to be used by comms::field::Optional
/// @headerfile comms/field/OptionalMode.h
enum class OptionalMode
{
    Tentative, ///< The field existence is tentative, i.e. If there is enough bytes
               /// to read the field's value, than field exists, if not
               /// then it doesn't exist.
    Exists, ///< Field must exist
    Missing, ///< Field doesn't exist
    NumOfModes ///< Number of possible modes, must be last
};

}  // namespace field

}  // namespace comms


