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
/// @brief Contains definition of Lists message and its fields.

#pragma once

#include "demo/Message.h"

namespace demo
{

namespace message
{

/// @brief Accumulates details of all the Lists message fields.
/// @tparam TFieldBase base class for all the fields.
/// @see Lists
template <typename TFieldBase>
struct ListsFields
{
    /// @brief Raw data list that uses 2 bytes size prefix
    using field1 =
        comms::field::ArrayList<
            TFieldBase,
            std::uint8_t,
            comms::option::SequenceSizeFieldPrefix<
                comms::field::IntValue<
                    TFieldBase,
                    std::uint16_t
                >
            >
        >;

    /// @brief List of 2 bytes integer value fields, with fixed size of 3 elements
    using field2 =
        comms::field::ArrayList<
            TFieldBase,
            comms::field::IntValue<
                TFieldBase,
                std::int16_t
            >,
            comms::option::SequenceFixedSize<3>
        >;

    /// @brief List of 2 bytes integer value fields, prefixed with
    ///     2 bytes serialisation length
    using field3 =
        comms::field::ArrayList<
            TFieldBase,
            comms::field::IntValue<
                TFieldBase,
                std::uint16_t
            >,
            comms::option::SequenceSerLengthFieldPrefix<
                comms::field::IntValue<
                    TFieldBase,
                    std::uint16_t
                >
            >
        >;

    /// @brief List of bundles, every bundle has two integer values member fields.
    using field4 =
        comms::field::ArrayList<
            TFieldBase,
            comms::field::Bundle<
                TFieldBase,
                std::tuple<
                    comms::field::IntValue<
                        TFieldBase,
                        std::uint16_t
                    >,
                    comms::field::IntValue<
                        TFieldBase,
                        std::int8_t
                    >
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
template <typename TMsgBase = Message>
class Lists : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Lists>,
        comms::option::FieldsImpl<typename ListsFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<Lists<TMsgBase> >
    >
{
public:

    /// @brief Allow access to internal fields.
    /// @details See definition of @b COMMS_MSG_FIELDS_ACCESS macro
    ///     related to @b comms::MessageBase class from COMMS library
    ///     for details.
    ///
    COMMS_MSG_FIELDS_ACCESS(field1, field2, field3, field4);

    /// @brief Default constructor
    Lists() = default;

    /// @brief Copy constructor
    Lists(const Lists&) = default;

    /// @brief Move constructor
    Lists(Lists&& other) = default;

    /// @brief Destructor
    virtual ~Lists() = default;

    /// @brief Copy assignment
    Lists& operator=(const Lists&) = default;

    /// @brief Move assignment
    Lists& operator=(Lists&&) = default;
};

}  // namespace message

}  // namespace demo


