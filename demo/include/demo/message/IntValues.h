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
/// @brief Contains definition of IntValues message and its fields.

#pragma once

#include "comms/fields.h"
#include "comms/MessageBase.h"
#include "demo/MsgId.h"
#include "demo/FieldBase.h"

namespace demo
{

namespace message
{

/// @brief Accumulates details of all the IntValues message fields.
/// @see IntValues
struct IntValuesFields
{
    /// @brief Simple 2 byte unsigned value.
    /// @details The valid values are in range [0, 10]
    using field1 =
        comms::field::IntValue<
            FieldBase,
            std::uint16_t,
            comms::option::ValidNumValueRange<0, 10>
    >;

    /// @brief Signed integer serialised using only 3 bytes
    using field2 =
        comms::field::IntValue<
            FieldBase,
            std::int32_t,
            comms::option::FixedLength<3>
    >;

    /// @brief Variable length (base-128) encoded unsigned integer value
    using field3 =
        comms::field::IntValue<
            FieldBase,
            std::uint32_t,
            comms::option::VarLength<1, 4>
        >;

    /// @brief Example of serialising year information as a single byte.
    /// @details Serialised as offset from year 2000 using 1 byte.
    ///     Default constructed value is 2016
    using field4 =
        comms::field::IntValue<
            FieldBase,
            std::int16_t,
            comms::option::FixedLength<1>,
            comms::option::NumValueSerOffset<-2000>,
            comms::option::DefaultNumValue<2016>,
            comms::option::ValidNumValueRange<2000, 2255>
        >;

    /// @brief Signed integer serialised using 6 bytes
    using field5 =
        comms::field::IntValue<
            FieldBase,
            std::int64_t,
            comms::option::FixedLength<6>,
            comms::option::ValidNumValueRange<(std::int64_t)0xffff800000000000, 0x7fffffffffff>
        >;

    /// @brief Unsigned integer serialised using 8 bytes
    using field6 =
        comms::field::IntValue<
            FieldBase,
            std::uint64_t
        >;


    /// @brief All the fields bundled in std::tuple.
    using All = std::tuple<
        field1,
        field2,
        field3,
        field4,
        field5,
        field6
    >;
};

/// @brief Accumulates various integer values.
/// @details Inherits from
///     @b comms::MessageBase
///     while providing @b TMsgBase as common interface class as well as
///     various implementation options. @n
///     See @ref IntValuesFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase>
class IntValues : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_IntValues>,
        comms::option::FieldsImpl<IntValuesFields::All>,
        comms::option::MsgType<IntValues<TMsgBase> >
    >
{
    // Required for compilation with gcc earlier than v5.0,
    // later versions don't require this type definition.
    using Base =
        comms::MessageBase<
            TMsgBase,
            comms::option::StaticNumIdImpl<MsgId_IntValues>,
            comms::option::FieldsImpl<IntValuesFields::All>,
            comms::option::MsgType<IntValues<TMsgBase> >
        >;

public:

    /// @brief Allow access to internal fields.
    /// @details See definition of @b COMMS_MSG_FIELDS_ACCESS macro
    ///     related to @b comms::MessageBase class from COMMS library
    ///     for details.
    ///
    COMMS_MSG_FIELDS_ACCESS(field1, field2, field3, field4, field5, field6);

    // Check serialisation lengths
    static_assert(Base::doMinLength() == 21, "Unexpected min serialisation length");
    static_assert(Base::doMaxLength() == 24, "Unexpected max serialisation length");

    /// @brief Default constructor
    IntValues() = default;

    /// @brief Copy constructor
    IntValues(const IntValues&) = default;

    /// @brief Move constructor
    IntValues(IntValues&& other) = default;

    /// @brief Destructor
    ~IntValues() noexcept = default;

    /// @brief Copy assignment
    IntValues& operator=(const IntValues&) = default;

    /// @brief Move assignment
    IntValues& operator=(IntValues&&) = default;
};

}  // namespace message

}  // namespace demo


