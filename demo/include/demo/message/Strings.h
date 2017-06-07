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
/// @brief Contains definition of Strings message and its fields.

#pragma once

#include "comms/fields.h"
#include "comms/MessageBase.h"
#include "demo/MsgId.h"

namespace demo
{


namespace message
{

/// @brief Accumulates details of all the Strings message fields.
/// @tparam TFieldBase base class for all the fields.
/// @see Strings
template <typename TFieldBase>
struct StringsFields
{
    /// @brief String that uses 1 byte size prefix
    using field1 =
        comms::field::String<
            TFieldBase,
            comms::option::SequenceSizeFieldPrefix<
                comms::field::IntValue<
                    TFieldBase,
                    std::uint8_t
                >
            >
    >;

    /// @brief String that is zero terminated
    using field2 =
        comms::field::String<
            TFieldBase,
            comms::option::SequenceTerminationFieldSuffix<
                comms::field::IntValue<
                    TFieldBase,
                    std::uint8_t
                >
            >
    >;

    /// @brief Fixed size of 6 characters string
    using field3 =
        comms::field::String<
            TFieldBase,
            comms::option::SequenceFixedSize<6>
    >;


    /// @brief All the fields bundled in std::tuple.
    using All = std::tuple<
        field1,
        field2,
        field3
    >;
};

/// @brief Accumulates various string fields.
/// @details Inherits from
///     @b comms::MessageBase
///     while providing @b TMsgBase as common interface class as well as
///     various implementation options. @n
///     See @ref StringsFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase>
class Strings : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Strings>,
        comms::option::FieldsImpl<typename StringsFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<Strings<TMsgBase> >
    >
{
public:

    /// @brief Allow access to internal fields.
    /// @details See definition of @b COMMS_MSG_FIELDS_ACCESS macro
    ///     related to @b comms::MessageBase class from COMMS library
    ///     for details.
    ///
    COMMS_MSG_FIELDS_ACCESS(field1, field2, field3);

    /// @brief Default constructor
    Strings() = default;

    /// @brief Copy constructor
    Strings(const Strings&) = default;

    /// @brief Move constructor
    Strings(Strings&& other) = default;

    /// @brief Destructor
    ~Strings() = default;

    /// @brief Copy assignment
    Strings& operator=(const Strings&) = default;

    /// @brief Move assignment
    Strings& operator=(Strings&&) = default;
};

}  // namespace message

}  // namespace demo

