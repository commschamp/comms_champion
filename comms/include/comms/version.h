//
// Copyright 2018 - 2019 (C). Alex Robenko. All rights reserved.
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

/// @file
/// Contains version information of the library

#pragma once

/// @brief Major verion of the library
#define COMMS_MAJOR_VERSION 2U

/// @brief Minor verion of the library
#define COMMS_MINOR_VERSION 0U

/// @brief Patch level of the library
#define COMMS_PATCH_VERSION 2U

/// @brief Macro to create numeric version as single unsigned number
#define COMMS_MAKE_VERSION(major_, minor_, patch_) \
    ((static_cast<unsigned>(major_) << 24) | \
     (static_cast<unsigned>(minor_) << 8) | \
     (static_cast<unsigned>(patch_)))

/// @brief Version of the COMMS library as single numeric value
#define COMMS_VERSION COMMS_MAKE_VERSION(COMMS_MAJOR_VERSION, COMMS_MINOR_VERSION, COMMS_PATCH_VERSION)

namespace comms
{

/// @brief Major verion of the library
inline
constexpr unsigned versionMajor()
{
    return COMMS_MAJOR_VERSION;
}

/// @brief Minor verion of the library
inline
constexpr unsigned versionMinor()
{
    return COMMS_MINOR_VERSION;
}

/// @brief Patch level of the library
inline
constexpr unsigned versionPatch()
{
    return COMMS_PATCH_VERSION;
}

/// @brief Create version of the library as single unsigned numeric value.
inline
constexpr unsigned versionCreate(unsigned major, unsigned minor, unsigned patch)
{
    return COMMS_MAKE_VERSION(major, minor, patch);
}

/// @brief Version of the COMMS library as single numeric value
inline
constexpr unsigned version()
{
    return COMMS_VERSION;
}

} // namespace comms
