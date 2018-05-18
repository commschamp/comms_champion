//
// Copyright 2016 - 2018 (C). Alex Robenko. All rights reserved.
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
#include "demo/DefaultOptions.h"

namespace demo
{

namespace message
{

/// @brief Accumulates details of all the IntValues message fields.
/// @tparam TOpt Extra options
/// @see IntValues
template <typename TOpt = demo::DefaultOptions>
struct IntValuesFields
{
    /// @brief Simple 2 byte unsigned value.
    /// @details The valid values are in range [0, 10]
    using field1 =
        comms::field::IntValue<
            FieldBase,
            std::uint16_t,
            typename TOpt::message::IntValuesFields::field1,
            comms::option::ValidNumValueRange<0, 10>
    >;

    /// @brief Signed integer serialised using only 3 bytes
    using field2 =
        comms::field::IntValue<
            FieldBase,
            std::int32_t,
            typename TOpt::message::IntValuesFields::field2,
            comms::option::FixedLength<3>
    >;

    /// @brief Variable length (base-128) encoded unsigned integer value
    using field3 =
        comms::field::IntValue<
            FieldBase,
            std::uint32_t,
            typename TOpt::message::IntValuesFields::field3,
            comms::option::VarLength<1, 4>
        >;

    /// @brief Example of serialising year information as a single byte.
    /// @details Serialised as offset from year 2000 using 1 byte.
    ///     Default constructed value is 2016
    using field4 =
        comms::field::IntValue<
            FieldBase,
            std::int16_t,
            typename TOpt::message::IntValuesFields::field4,
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
            typename TOpt::message::IntValuesFields::field5,
            comms::option::FixedLength<6>,
            comms::option::ValidNumValueRange<(std::int64_t)0xffff800000000000, 0x7fffffffffff>
        >;

    /// @brief Unsigned integer serialised using 8 bytes
    using field6 =
        comms::field::IntValue<
            FieldBase,
            std::uint64_t,
            typename TOpt::message::IntValuesFields::field6
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
/// @tparam TOpt Extra options
template <typename TMsgBase, typename TOpt = demo::DefaultOptions>
class IntValues : public
    comms::MessageBase<
        TMsgBase,
        typename TOpt::message::IntValues,
        comms::option::StaticNumIdImpl<MsgId_IntValues>,
        comms::option::FieldsImpl<typename IntValuesFields<TOpt>::All>,
        comms::option::MsgType<IntValues<TMsgBase, TOpt> >,
        comms::option::HasName
    >
{
    // Required for compilation with gcc earlier than v5.0,
    // later versions don't require this type definition.
    using Base =
        comms::MessageBase<
            TMsgBase,
            typename TOpt::message::IntValues,
            comms::option::StaticNumIdImpl<MsgId_IntValues>,
            comms::option::FieldsImpl<typename IntValuesFields<TOpt>::All>,
            comms::option::MsgType<IntValues<TMsgBase, TOpt> >,
            comms::option::HasName
        >;

    static const bool AreFieldsVersionDependent = Base::areFieldsVersionDependent();
    static_assert(!AreFieldsVersionDependent, "Fields mustn't be version dependent");
public:

    /// @brief Allow access to internal fields.
    /// @details See definition of @b COMMS_MSG_FIELDS_ACCESS macro
    ///     related to @b comms::MessageBase class from COMMS library
    ///     for details.
    ///
    COMMS_MSG_FIELDS_ACCESS(field1, field2, field3, field4, field5, field6);

    // Check serialisation lengths
    // For some reason VS2015 compiler fails when calls to doMinLength() and
    // doMaxLength() are performed inside static_assert.
    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static_assert(MsgMinLen == 21, "Unexpected min serialisation length");
    static_assert(MsgMaxLen == 24, "Unexpected max serialisation length");

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

    /// @brief Name of the message.
    static const char* doName()
    {
        return "IntValues";
    }
};

}  // namespace message

}  // namespace demo


