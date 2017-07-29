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
/// @brief Contains definition of Bitfields message and its fields.

#pragma once

#include "comms/fields.h"
#include "comms/MessageBase.h"
#include "demo/MsgId.h"
#include "demo/FieldBase.h"

namespace demo
{


namespace message
{

/// @brief Accumulates details of all the Bitfields message fields.
/// @see Bitfields
struct BitfieldsFields
{
    /// @brief Bit access indices for @ref field1_bitmask
    enum
    {
        field1_bit0, ///< index of bit 0
        field1_bit1, ///< index of bit 1
        field1_bit2, ///< index of bit 2
        field1_numOfValues ///< limit to available bits
    };

    /// @brief Simple 4 bits bitmask value, residing as a member in @ref field1 bitfield.
    /// @details It uses 4 bits of @ref field1. The "bit 3" of the bitmask is
    ///     defined to be reserved and must be 0.
    using field1_bitmask =
        comms::field::BitmaskValue<
            FieldBase,
            comms::option::FixedLength<1>,
            comms::option::FixedBitLength<4>,
            comms::option::BitmaskReservedBits<0xf8, 0>
    >;

    /// @brief Enumeration type for the @ref field1_enum
    enum class Field1Enum : std::uint8_t
    {
        Value1, ///< Value1
        Value2, ///< Value2
        Value3, ///< Value3
        NumOfValues ///< Number of available values
    };

    /// @brief Enumeration field, that consumes 2 bits in @ref field1 bitfield.
    using field1_enum =
        comms::field::EnumValue<
            FieldBase,
            Field1Enum,
            comms::option::ValidNumValueRange<(int)0, (int)Field1Enum::NumOfValues - 1>,
            comms::option::FixedBitLength<2>
    >;

    /// @brief Integer field, that consumes 6 bits in @ref field1 bitfield.
    using field1_int1 =
        comms::field::IntValue<
            FieldBase,
            std::uint8_t,
            comms::option::FixedBitLength<6>,
            comms::option::ValidNumValueRange<0, 0x3f>
        >;

    /// @brief Integer field, that consumes 4 bits in @ref field1 bitfield.
    using field1_int2 =
        comms::field::IntValue<
            FieldBase,
            std::uint8_t,
            comms::option::FixedBitLength<4>,
            comms::option::ValidNumValueRange<0, 0xf>
        >;

    /// @brief Bitfield containing @ref field1_bitmask, @ref field1_enum,
    ///     @ref field1_int1, and @ref field1_int2 as its internal members
    struct field1 : public
        comms::field::Bitfield<
            FieldBase,
            std::tuple<
                field1_bitmask,
                field1_enum,
                field1_int1,
                field1_int2
            >
        >
    {
        /// @brief Allow access to internal fields.
        /// @details See definition of @b COMMS_FIELD_MEMBERS_ACCESS_NOTEMPLATE macro
        ///     related to @b comms::field::Bitfield class from COMMS library
        ///     for details. @n
        ///     The names are:
        ///     @b member1 for @ref field1_bitmask
        ///     @b member2 for @ref field1_enum
        ///     @b member3 for @ref field1_int1
        ///     @b member4 for @ref field1_int2
        ///
        COMMS_FIELD_MEMBERS_ACCESS_NOTEMPLATE(member1, member2, member3, member4);
    };

    /// @brief All the fields bundled in std::tuple.
    using All = std::tuple<
        field1
    >;
};

/// @brief Accumulates various bitfield fields.
/// @details Inherits from
///     @b comms::MessageBase
///     while providing @b TMsgBase as common interface class as well as
///     various implementation options. @n
///     See @ref BitfieldsFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase>
class Bitfields : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Bitfields>,
        comms::option::FieldsImpl<BitfieldsFields::All>,
        comms::option::MsgType<Bitfields<TMsgBase> >
    >
{
    // Required for compilation with gcc earlier than v5.0,
    // later versions don't require this type definition.
    using Base =
        comms::MessageBase<
            TMsgBase,
            comms::option::StaticNumIdImpl<MsgId_Bitfields>,
            comms::option::FieldsImpl<BitfieldsFields::All>,
            comms::option::MsgType<Bitfields<TMsgBase> >
        >;
public:

    /// @brief Allow access to internal fields.
    /// @details See definition of @b COMMS_MSG_FIELDS_ACCESS macro
    ///     related to @b comms::MessageBase class from COMMS library
    ///     for details.
    ///
    COMMS_MSG_FIELDS_ACCESS(field1);

    /// @brief Default constructor
    Bitfields() = default;

    /// @brief Copy constructor
    Bitfields(const Bitfields&) = default;

    /// @brief Move constructor
    Bitfields(Bitfields&& other) = default;

    /// @brief Destructor
    ~Bitfields() noexcept = default;

    /// @brief Copy assignment
    Bitfields& operator=(const Bitfields&) = default;

    /// @brief Move assignment
    Bitfields& operator=(Bitfields&&) = default;
};

}  // namespace message

}  // namespace demo


