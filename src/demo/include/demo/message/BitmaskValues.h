//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

#include "demo/Message.h"

namespace comms_champion
{

namespace demo
{


namespace message
{

/// @brief Accumulates details of all the BitmaskValues message fields.
/// @tparam TFieldBase base class for all the fields.
/// @see BitmaskValues
template <typename TFieldBase>
struct BitmaskValuesFields
{
    /// @brief Bit access indices for @ref field1 bitmask
    enum
    {
        field1_bit0, ///< index of bit 0
        field1_bit1, ///< index of bit 1
        field1_bit2, ///< index of bit 2
        field1_bit3, ///< index of bit 3
        field1_bit4, ///< index of bit 4
        field1_NumOfValues ///< limit to available bits
    };

    /// @brief Simple 1 byte bitmask value.
    /// @details Has 5 least significant bits in use, others are reserved and must
    ///     be 0.
    using field1 =
        comms::field::BitmaskValue<
            TFieldBase,
            comms::option::FixedLength<1>,
            comms::option::BitmaskReservedBits<0xe0, 0>
    >;

    /// @brief Bit access indices for @ref field2 bitmask
    enum
    {
        field2_bit0, ///< index of bit 0
        field2_bit3 = 3, ///< index of bit 3
        field2_bit8 = 8, ///< index of bit 8
        field2_bit9 = 9, ///< index of bit 9
        field2_NumOfValues ///< limit to available bits
    };

    /// @brief Bitmask with 2 bytes length
    /// @details The used bits are not sequential, multiple reserved bits
    ///     in the middle. The value of the reserved bit must be 0
    using field2 =
        comms::field::BitmaskValue<
            TFieldBase,
            comms::option::FixedLength<2>,
            comms::option::BitmaskReservedBits<0xfcf6, 0>
    >;

    /// @brief All the fields bundled in std::tuple.
    using All = std::tuple<
        field1,
        field2
    >;
};

/// @brief Accumulates various bitmask values.
/// @details Inherits from
///     <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/classcomms_1_1MessageBase.html">comms::MessageBase</a>
///     while providing @b TMsgBase as common interface class as well as
///     @b comms::option::StaticNumIdImpl, @b comms::option::FieldsImpl, and
///     @b comms::option::DispatchImpl as options. @n
///     See @ref BitmaskValuesFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase = Message>
class BitmaskValues : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_BitmaskValues>,
        comms::option::FieldsImpl<typename BitmaskValuesFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<BitmaskValues<TMsgBase> >
    >
{
    typedef comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_BitmaskValues>,
        comms::option::FieldsImpl<typename BitmaskValuesFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<BitmaskValues<TMsgBase> >
    > Base;
public:

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Index to access the fields
    enum FieldIdx
    {
        FieldIdx_field1, ///< field1 field, see @ref BitmaskValuesFields::field1
        FieldIdx_field2, ///< field2 field, see @ref BitmaskValuesFields::field2
        FieldIdx_numOfValues ///< number of available fields
    };

    /// @brief Access to fields, bundled into struct
    struct FieldsAsStruct
    {
        BitmaskValuesFields::field1& field1; ///< Access to field1
        BitmaskValuesFields::field2& field2; ///< Access to field2
    };

    /// @brief Access to @b const fields, bundled into struct
    struct ConstFieldsAsStruct
    {
        const BitmaskValuesFields::field1& field1; ///< Access to field1
        const BitmaskValuesFields::field2& field2; ///< Access to field2
    };

    /// @brief Get access to fields, bundled into struct
    FieldsAsStruct fieldsAsStruct();

    /// @brief Get access to @b const fields, bundled into struct
    ConstFieldsAsStruct fieldsAsStruct() const;

#else
    COMMS_MSG_FIELDS_ACCESS(Base, field1, field2);
#endif

    /// @brief Default constructor
    BitmaskValues() = default;

    /// @brief Copy constructor
    BitmaskValues(const BitmaskValues&) = default;

    /// @brief Move constructor
    BitmaskValues(BitmaskValues&& other) = default;

    /// @brief Destructor
    virtual ~BitmaskValues() = default;

    /// @brief Copy assignment
    BitmaskValues& operator=(const BitmaskValues&) = default;

    /// @brief Move assignment
    BitmaskValues& operator=(BitmaskValues&&) = default;
};

}  // namespace message

}  // namespace demo

}  // namespace comms_champion


