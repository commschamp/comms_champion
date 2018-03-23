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
/// @brief Contains definition of BitmaskValues message and its fields.

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

/// @brief Accumulates details of all the BitmaskValues message fields.
/// @tparam TOpt Extra options
/// @see BitmaskValues
template <typename TOpt = demo::DefaultOptions>
struct BitmaskValuesFields
{
    /// @brief Simple 1 byte bitmask value.
    /// @details Has 5 least significant bits in use, others are reserved and must
    ///     be 0.
    struct field1 : public
        comms::field::BitmaskValue<
            FieldBase,
            typename TOpt::message::BitmaskValuesFields::field1,
            comms::option::FixedLength<1>,
            comms::option::BitmaskReservedBits<0xe0, 0>
        >
    {
        /// @brief Provide names generates access functions for internal bits.
        /// @details See definition of @b COMMS_BITMASK_BITS_SEQ_NOTEMPLATE macro
        ///     related to @b comms::field::BitmaskValue class from COMMS library
        ///     for details.
        COMMS_BITMASK_BITS_SEQ_NOTEMPLATE(bit0, bit1, bit2, bit3, bit4);
    };

    /// @brief Bitmask with 2 bytes length
    /// @details The used bits are not sequential, multiple reserved bits
    ///     in the middle. The value of the reserved bit must be 0
    struct field2 : public
        comms::field::BitmaskValue<
            FieldBase,
            typename TOpt::message::BitmaskValuesFields::field2,
            comms::option::FixedLength<2>,
            comms::option::BitmaskReservedBits<0xfcf6, 0>
        >
    {
        /// @brief Provide names for internal bits.
        /// @details See definition of @b COMMS_BITMASK_BITS macro
        ///     related to @b comms::field::BitmaskValue class from COMMS library
        ///     for details.
        COMMS_BITMASK_BITS(bit0, bit3=3, bit8=8, bit9=9);

        /// @brief Generated independent access functions for internal bits.
        /// @details See definition of @b COMMS_BITMASK_BITS_ACCESS_NOTEMPLATE macro
        ///     related to @b comms::field::BitmaskValue class from COMMS library
        ///     for details.
        COMMS_BITMASK_BITS_ACCESS_NOTEMPLATE(bit0, bit3, bit8, bit9);
    };

    /// @brief All the fields bundled in std::tuple.
    using All = std::tuple<
        field1,
        field2
    >;
};

/// @brief Accumulates various bitmask values.
/// @details Inherits from
///     @b comms::MessageBase
///     while providing @b TMsgBase as common interface class as well as
///     various implementation options. @n
///     See @ref BitmaskValuesFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
/// @tparam TOpt Extra options
template <typename TMsgBase, typename TOpt = demo::DefaultOptions>
class BitmaskValues : public
    comms::MessageBase<
        TMsgBase,
        typename TOpt::message::BitmaskValues,
        comms::option::StaticNumIdImpl<MsgId_BitmaskValues>,
        comms::option::FieldsImpl<typename BitmaskValuesFields<TOpt>::All>,
        comms::option::MsgType<BitmaskValues<TMsgBase, TOpt> >
    >
{
    // Required for compilation with gcc earlier than v5.0,
    // later versions don't require this type definition.
    using Base =
        comms::MessageBase<
            TMsgBase,
            typename TOpt::message::BitmaskValues,
            comms::option::StaticNumIdImpl<MsgId_BitmaskValues>,
            comms::option::FieldsImpl<typename BitmaskValuesFields<TOpt>::All>,
            comms::option::MsgType<BitmaskValues<TMsgBase, TOpt> >
        >;
public:

    /// @brief Allow access to internal fields.
    /// @details See definition of @b COMMS_MSG_FIELDS_ACCESS macro
    ///     related to @b comms::MessageBase class from COMMS library
    ///     for details.
    ///
    COMMS_MSG_FIELDS_ACCESS(field1, field2);

    // Check serialisation lengths
    // For some reason VS2015 compiler fails when calls to doMinLength() and
    // doMaxLength() are performed inside static_assert.
    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static_assert(MsgMinLen == 3, "Unexpected min serialisation length");
    static_assert(MsgMaxLen == 3, "Unexpected max serialisation length");

    /// @brief Default constructor
    BitmaskValues() = default;

    /// @brief Copy constructor
    BitmaskValues(const BitmaskValues&) = default;

    /// @brief Move constructor
    BitmaskValues(BitmaskValues&& other) = default;

    /// @brief Destructor
    ~BitmaskValues() noexcept = default;

    /// @brief Copy assignment
    BitmaskValues& operator=(const BitmaskValues&) = default;

    /// @brief Move assignment
    BitmaskValues& operator=(BitmaskValues&&) = default;
};

}  // namespace message

}  // namespace demo


