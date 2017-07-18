//
// Copyright 2016 - 2017 (C). Alex Robenko. All rights reserved.
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
/// @brief Contains definition of FloatValues message and its fields.

#pragma once

#include "comms/fields.h"
#include "comms/MessageBase.h"
#include "demo/MsgId.h"
#include "demo/FieldBase.h"

namespace demo
{

namespace message
{

/// @brief Accumulates details of all the FloatValues message fields.
/// @see FloatValues
struct FloatValuesFields
{
    /// @brief Simple 4 byte IEEE 754 floating point value.
    using field1 =
        comms::field::FloatValue<
            FieldBase,
            float
    >;

    /// @brief Simple 8 byte IEEE 754 floating point value.
    using field2 =
        comms::field::FloatValue<
            FieldBase,
            double
    >;

    /// @brief Floating point value serialised as integer with (1e-2) scaling ratio.
    using field3 =
        comms::field::IntValue<
            FieldBase,
            std::uint8_t,
            comms::option::ScalingRatio<1, 100>
        >;

    /// @brief All the fields bundled in std::tuple.
    using All = std::tuple<
        field1,
        field2,
        field3
    >;
};

/// @brief Accumulates various floating point values.
/// @details Inherits from
///     @b comms::MessageBase
///     while providing @b TMsgBase as common interface class as well as
///     various implementation options. @n
///     See @ref FloatValuesFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase>
class FloatValues : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_FloatValues>,
        comms::option::FieldsImpl<FloatValuesFields::All>,
        comms::option::MsgType<FloatValues<TMsgBase> >
    >
{
    // Required for compilation with gcc earlier than v5.0,
    // later versions don't require this type definition.
    using Base =
        comms::MessageBase<
            TMsgBase,
            comms::option::StaticNumIdImpl<MsgId_FloatValues>,
            comms::option::FieldsImpl<FloatValuesFields::All>,
            comms::option::MsgType<FloatValues<TMsgBase> >
        >;
public:

    /// @brief Allow access to internal fields.
    /// @details See definition of @b COMMS_MSG_FIELDS_ACCESS macro
    ///     related to @b comms::MessageBase class from COMMS library
    ///     for details.
    ///
    COMMS_MSG_FIELDS_ACCESS(field1, field2, field3);

    /// @brief Default constructor
    FloatValues() = default;

    /// @brief Copy constructor
    FloatValues(const FloatValues&) = default;

    /// @brief Move constructor
    FloatValues(FloatValues&& other) = default;

    /// @brief Destructor
    ~FloatValues() = default;

    /// @brief Copy assignment
    FloatValues& operator=(const FloatValues&) = default;

    /// @brief Move assignment
    FloatValues& operator=(FloatValues&&) = default;
};

}  // namespace message

}  // namespace demo



