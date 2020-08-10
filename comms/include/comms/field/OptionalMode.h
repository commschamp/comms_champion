//
// Copyright 2016 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief Contains definition of the mode used for comms::field::Optional fields.

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


