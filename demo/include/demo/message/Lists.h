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
/// @brief Contains definition of Lists message and its fields.

#pragma once

#include "comms/fields.h"
#include "comms/MessageBase.h"
#include "demo/MsgId.h"
#include "demo/FieldBase.h"
#include "demo/DefaultOptions.h"

#include <iostream>

namespace demo
{

namespace message
{

/// @brief Accumulates details of all the Lists message fields.
/// @tparam TOpt Extra options
/// @see Lists
template <typename TOpt = demo::DefaultOptions>
struct ListsFields
{
    /// @brief Raw data list that uses 2 bytes size prefix
    using field1 =
        comms::field::ArrayList<
            FieldBase,
            std::uint8_t,
            typename TOpt::message::ListsFields::field1,
            comms::option::SequenceSizeFieldPrefix<
                comms::field::IntValue<
                    FieldBase,
                    std::uint16_t
                >
            >
        >;

    /// @brief Element of @ref field2 list
    using field2Element =
        comms::field::IntValue<
            FieldBase,
            std::int16_t,
            typename TOpt::message::ListsFields::field2Element
        >;

    /// @brief List of 2 bytes integer value fields, with fixed size of 3 elements
    using field2 =
        comms::field::ArrayList<
            FieldBase,
            field2Element,
            typename TOpt::message::ListsFields::field2,
            comms::option::SequenceFixedSize<3>
        >;

    /// @brief Element of @ref field3 list
    using field3Element =
        comms::field::IntValue<
            FieldBase,
            std::uint16_t,
            typename TOpt::message::ListsFields::field3Element
        >;

    /// @brief List of 2 bytes integer value fields, prefixed with
    ///     2 bytes serialisation length
    using field3 =
        comms::field::ArrayList<
            FieldBase,
            field3Element,
            typename TOpt::message::ListsFields::field3,
            comms::option::SequenceSerLengthFieldPrefix<
                comms::field::IntValue<
                    FieldBase,
                    std::uint16_t
                >
            >
        >;

    /// Scope for members of @ref field4Element
    struct field4Members
    {
        /// @brief 2 byte unsigned integer field
        using mem1 =
            comms::field::IntValue<
                FieldBase,
                std::uint16_t,
                typename TOpt::message::ListsFields::field4Members::mem1
            >;

        /// @brief 1 byte unsigned integer field
        using mem2 =
            comms::field::IntValue<
                FieldBase,
                std::int8_t,
                typename TOpt::message::ListsFields::field4Members::mem2
            >;

        /// @brief string with serialisation length prefix. The prefix is of variable
        ///         length, i.e. uses base-128 encoding.
        using mem3 =
            comms::field::String<
                FieldBase,
                typename TOpt::message::ListsFields::field4Members::mem3,
                comms::option::SequenceSizeFieldPrefix<
                    comms::field::IntValue<
                        FieldBase,
                        std::uint32_t,
                        comms::option::VarLength<1, 4>
                    >
                >
            >;
    };

    /// @brief Element of @ref field4 list
    using field4Element =
        comms::field::Bundle<
            FieldBase,
            std::tuple<
                typename field4Members::mem1,
                typename field4Members::mem2,
                typename field4Members::mem3
            >,
            typename TOpt::message::ListsFields::field4Element
        >;

    /// @brief List of bundles
    /// @details Every bundle contains:
    ///     @li 2 byte unsigned integer field
    ///     @li 1 byte unsigned integer field
    ///     @li string with serialisation length prefix. The prefix is of variable
    ///         length, i.e. uses base-128 encoding.
    ///
    ///     Every list element (bundle) is prefixed with its total serialisation
    ///     length. The length info is written using base-128 encoding.
    ///
    ///     The list itself is prefixed with total serialisation length. The length
    ///     info is written using base-128 encoding.
    using field4 =
        comms::field::ArrayList<
            FieldBase,
            field4Element,
            typename TOpt::message::ListsFields::field4,
            comms::option::SequenceSerLengthFieldPrefix<
                comms::field::IntValue<
                    FieldBase,
                    std::uint32_t,
                    comms::option::VarLength<1, 4>
                >
            >,
            comms::option::SequenceElemSerLengthFieldPrefix<
                comms::field::IntValue<
                    FieldBase,
                    std::uint32_t,
                    comms::option::VarLength<1, 4>
                >
            >
        >;

    /// @brief All the fields bundled in std::tuple.
    using All = std::tuple<
        field1,
        field2,
        field3,
        field4
    >;
};

/// @brief Accumulates various list fields.
/// @details Inherits from
///     @b comms::MessageBase
///     while providing @b TMsgBase as common interface class as well as
///     various implementation options. @n
///     See @ref ListsFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
/// @tparam TOpt Extra options
template <typename TMsgBase, typename TOpt = demo::DefaultOptions>
class Lists : public
    comms::MessageBase<
        TMsgBase,
        typename TOpt::message::Lists,
        comms::option::StaticNumIdImpl<MsgId_Lists>,
        comms::option::FieldsImpl<typename ListsFields<TOpt>::All>,
        comms::option::MsgType<Lists<TMsgBase, TOpt> >,
        comms::option::HasName
    >
{
    // Required for compilation with gcc earlier than v5.0,
    // later versions don't require this type definition.
    using Base =
        comms::MessageBase<
            TMsgBase,
            typename TOpt::message::Lists,
            comms::option::StaticNumIdImpl<MsgId_Lists>,
            comms::option::FieldsImpl<typename ListsFields<TOpt>::All>,
            comms::option::MsgType<Lists<TMsgBase, TOpt> >,
            comms::option::HasName
        >;

    static_assert(!Base::areFieldsVersionDependent(), "Fields mustn't be version dependent");
public:

    /// @brief Allow access to internal fields.
    /// @details See definition of @b COMMS_MSG_FIELDS_ACCESS macro
    ///     related to @b comms::MessageBase class from COMMS library
    ///     for details.
    ///
    COMMS_MSG_FIELDS_ACCESS(field1, field2, field3, field4);

    // Check serialisation lengths
    // For some reason VS2015 compiler fails when call to doMinLength() and
    // is performed inside static_assert.
    static const std::size_t MsgMinLen = Base::doMinLength();
    static_assert(MsgMinLen == 11, "Unexpected min serialisation length");

    /// @brief Default constructor
    Lists() = default;

    /// @brief Copy constructor
    Lists(const Lists&) = default;

    /// @brief Move constructor
    Lists(Lists&& other) = default;

    /// @brief Destructor
    ~Lists() noexcept = default;

    /// @brief Copy assignment
    Lists& operator=(const Lists&) = default;

    /// @brief Move assignment
    Lists& operator=(Lists&&) = default;

    /// @brief Name of the message.
    static const char* doName()
    {
        return "Lists";
    }
};

}  // namespace message

}  // namespace demo


