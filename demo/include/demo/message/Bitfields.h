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
/// @brief Contains definition of Bitfields message and its fields.

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

/// @brief Accumulates details of all the Bitfields message fields.
/// @tparam TOpt Extra options
/// @see Bitfields
template <typename TOpt = demo::DefaultOptions>
struct BitfieldsFields
{
    /// @brief Simple 4 bits bitmask value, residing as a member in @ref field1 bitfield.
    /// @details It uses 4 bits of @ref field1. The "bit 3" of the bitmask is
    ///     defined to be reserved and must be 0.
    struct field1_bitmask : public
        comms::field::BitmaskValue<
            FieldBase,
            typename TOpt::message::BitfieldsFields::field1_bitmask,
            comms::option::FixedLength<1>,
            comms::option::FixedBitLength<4>,
            comms::option::BitmaskReservedBits<0xf8, 0>
        >
    {
        /// @brief Provide names for internal bits.
        /// @details See definition of @b COMMS_BITMASK_BITS_SEQ_NOTEMPLATE macro
        ///     related to @b comms::field::BitmaskValue class from COMMS library
        ///     for details.
        COMMS_BITMASK_BITS_SEQ_NOTEMPLATE(bit0, bit1, bit2);
    };

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
            typename TOpt::message::BitfieldsFields::field1_enum,
            comms::option::ValidNumValueRange<(int)0, (int)Field1Enum::NumOfValues - 1>,
            comms::option::FixedBitLength<2>
    >;

    /// @brief Integer field, that consumes 6 bits in @ref field1 bitfield.
    using field1_int1 =
        comms::field::IntValue<
            FieldBase,
            std::uint8_t,
            typename TOpt::message::BitfieldsFields::field1_int1,
            comms::option::FixedBitLength<6>,
            comms::option::ValidNumValueRange<0, 0x3f>
        >;

    /// @brief Integer field, that consumes 4 bits in @ref field1 bitfield.
    using field1_int2 =
        comms::field::IntValue<
            FieldBase,
            std::uint8_t,
            typename TOpt::message::BitfieldsFields::field1_int2,
            comms::option::FixedBitLength<4>,
            comms::option::ValidNumValueRange<0, 0xf>
        >;

    /// @brief Bitfield containing @ref field1_bitmask, @ref field1_enum,
    ///     @ref field1_int1, and @ref field1_int2 as its internal members
    class field1 : public
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
#ifdef COMMS_MUST_DEFINE_BASE
        // For some reason VS2015 compiler doesn't like having this definition
        using Base =
            comms::field::Bitfield<
                FieldBase,
                std::tuple<
                    field1_bitmask,
                    field1_enum,
                    field1_int1,
                    field1_int2
                >
            >;
#endif // #ifdef COMMS_MUST_DEFINE_BASE
    public:
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
        COMMS_FIELD_MEMBERS_ACCESS(member1, member2, member3, member4);
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
/// @tparam TOpt Extra options
template <typename TMsgBase, typename TOpt = demo::DefaultOptions>
class Bitfields : public
    comms::MessageBase<
        TMsgBase,
        typename TOpt::message::Bitfields,
        comms::option::StaticNumIdImpl<MsgId_Bitfields>,
        comms::option::FieldsImpl<typename BitfieldsFields<TOpt>::All>,
        comms::option::MsgType<Bitfields<TMsgBase, TOpt> >,
        comms::option::HasName
    >
{
    // Required for compilation with gcc earlier than v5.0,
    // later versions don't require this type definition.
    using Base =
        comms::MessageBase<
            TMsgBase,
            typename TOpt::message::Bitfields,
            comms::option::StaticNumIdImpl<MsgId_Bitfields>,
            comms::option::FieldsImpl<typename BitfieldsFields<TOpt>::All>,
            comms::option::MsgType<Bitfields<TMsgBase, TOpt> >,
            comms::option::HasName
        >;

    static_assert(!Base::areFieldsVersionDependent(), "Fields mustn't be version dependent");
public:

    /// @brief Allow access to internal fields.
    /// @details See definition of @b COMMS_MSG_FIELDS_ACCESS macro
    ///     related to @b comms::MessageBase class from COMMS library
    ///     for details.
    ///
    COMMS_MSG_FIELDS_ACCESS(field1);

    // Check serialisation lengths
    // For some reason VS2015 compiler fails when calls to doMinLength() and
    // doMaxLength() are performed inside static_assert.
    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static_assert(MsgMinLen == 2, "Unexpected min serialisation length");
    static_assert(MsgMaxLen == 2, "Unexpected max serialisation length");

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

    /// @brief Name of the message.
    static const char* doName()
    {
        return "Bitfields";
    }
};

}  // namespace message

}  // namespace demo


