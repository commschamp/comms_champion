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

#include "demo/Message.h"

namespace demo
{

namespace message
{

/// @brief Accumulates details of all the FloatValues message fields.
/// @tparam TFieldBase base class for all the fields.
/// @see FloatValues
template <typename TFieldBase>
struct FloatValuesFields
{
    /// @brief Simple 4 byte IEEE 754 floating point value.
    using field1 =
        comms::field::FloatValue<
            TFieldBase,
            float
    >;

    /// @brief Simple 8 byte IEEE 754 floating point value.
    using field2 =
        comms::field::FloatValue<
            TFieldBase,
            double
    >;

    /// @brief Floating point value serialised as integer with (1e-2) scaling ratio.
    using field3 =
        comms::field::IntValue<
            TFieldBase,
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
///     <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/classcomms_1_1MessageBase.html">comms::MessageBase</a>
///     while providing @b TMsgBase as common interface class as well as
///     @b comms::option::StaticNumIdImpl, @b comms::option::FieldsImpl, and
///     @b comms::option::MsgType as options. @n
///     See @ref FloatValuesFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase = Message>
class FloatValues : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_FloatValues>,
        comms::option::FieldsImpl<typename FloatValuesFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<FloatValues<TMsgBase> >
    >
{
    typedef comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_FloatValues>,
        comms::option::FieldsImpl<typename FloatValuesFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<FloatValues<TMsgBase> >
    > Base;
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
    virtual ~FloatValues() = default;

    /// @brief Copy assignment
    FloatValues& operator=(const FloatValues&) = default;

    /// @brief Move assignment
    FloatValues& operator=(FloatValues&&) = default;
};

}  // namespace message

}  // namespace demo



