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
/// @brief Contains definition of IntValues message and its fields.

#pragma once

#include "demo/Message.h"

namespace comms_champion
{

namespace demo
{


namespace message
{

/// @brief Accumulates details of all the IntValues message fields.
/// @tparam TFieldBase base class for all the fields.
/// @see IntValues
template <typename TFieldBase>
struct IntValuesFields
{
    /// @brief Simple 2 byte unsigned value.
    /// @details The valid values are in range [0, 10]
    using field1 =
        comms::field::IntValue<
            TFieldBase,
            std::uint16_t,
            comms::option::ValidNumValueRange<0, 10>
    >;

    /// @brief Signed integer serialised using only 3 bytes
    using field2 =
        comms::field::IntValue<
            TFieldBase,
            std::int32_t,
            comms::option::FixedLength<3>
    >;

    /// @brief Variable length (base-128) encoded unsigned integer value
    using field3 =
        comms::field::IntValue<
            TFieldBase,
            std::uint32_t,
            comms::option::VarLength<1, 4>
        >;

    /// @brief Example of serialising year information as a single byte.
    /// @details Serialised as offset from year 2000 using 1 byte.
    ///     Default constructed value is 2016
    using field4 =
        comms::field::IntValue<
            TFieldBase,
            std::int16_t,
            comms::option::FixedLength<1>,
            comms::option::NumValueSerOffset<-2000>,
            comms::option::DefaultNumValue<2016>,
            comms::option::ValidNumValueRange<2000, 2255>
        >;

    /// @brief All the fields bundled in std::tuple.
    using All = std::tuple<
        field1,
        field2,
        field3,
        field4
    >;
};

/// @brief Accumulates various integer values.
/// @details Inherits from
///     <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/classcomms_1_1MessageBase.html">comms::MessageBase</a>
///     while providing @b TMsgBase as common interface class as well as
///     @b comms::option::StaticNumIdImpl, @b comms::option::FieldsImpl, and
///     @b comms::option::DispatchImpl as options. @n
///     See @ref IntValuesFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase = Message>
class IntValues : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_IntValues>,
        comms::option::FieldsImpl<typename IntValuesFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<IntValues<TMsgBase> >,
        comms::option::DispatchImpl
    >
{
    typedef comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_IntValues>,
        comms::option::FieldsImpl<typename IntValuesFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<IntValues<TMsgBase> >,
        comms::option::DispatchImpl
    > Base;
public:

    /// @brief Index to access the fields
    enum FieldIdx
    {
        FieldIdx_field1, ///< field1 field, see @ref IntValuesFields::field1
        FieldIdx_field2, ///< field2 field, see @ref IntValuesFields::field2
        FieldIdx_field3, ///< field3 field, see @ref IntValuesFields::field3
        FieldIdx_field4, ///< field4 field, see @ref IntValuesFields::field4
        FieldIdx_numOfValues ///< number of available fields
    };

    static_assert(std::tuple_size<typename Base::AllFields>::value == FieldIdx_numOfValues,
        "Number of fields is incorrect");

    /// @brief Default constructor
    IntValues() = default;

    /// @brief Copy constructor
    IntValues(const IntValues&) = default;

    /// @brief Move constructor
    IntValues(IntValues&& other) = default;

    /// @brief Destructor
    virtual ~IntValues() = default;

    /// @brief Copy assignment
    IntValues& operator=(const IntValues&) = default;

    /// @brief Move assignment
    IntValues& operator=(IntValues&&) = default;
};

}  // namespace message

}  // namespace demo

}  // namespace comms_champion


