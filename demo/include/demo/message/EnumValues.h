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
/// @brief Contains definition of EnumValues message and its fields.

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

/// @brief Accumulates details of all the EnumValues message fields.
/// @tparam TOpt Extra options
/// @see EnumValues
template <typename TOpt = demo::DefaultOptions>
struct EnumValuesFields
{
    /// @brief Enumeration type for the @ref field1
    /// @details The values are sequential and serialised as single byte
    enum class ValuesField1 : std::uint8_t
    {
        Value1, ///< Value1
        Value2, ///< Value2
        Value3, ///< Value3
        NumOfValues ///< Number of available values
    };

    /// @brief Simple 1 byte enumeration value.
    using field1 =
        comms::field::EnumValue<
            FieldBase,
            ValuesField1,
            typename TOpt::message::EnumValuesFields::field1,
            comms::option::ValidNumValueRange<(int)0, (int)ValuesField1::NumOfValues - 1>
    >;

    /// @brief Enumeration type for the @ref field2
    /// @details The values are sparse and signed. They are serialised
    ///     as two bytes.
    enum class ValuesField2 : std::int16_t
    {
        Value1 = -5, ///< Value1
        Value2 = 100, ///< Value2
        Value3 = 130, ///< Value3,
        Value4 = 1028 ///< Value4
    };

    /// @brief Value validator class for @ref field2
    struct ValuesField2Validator
    {
        template <typename TField>
        bool operator()(const TField& field) const
        {
            auto value = field.value();
            return
                (value == ValuesField2::Value1) ||
                (value == ValuesField2::Value2) ||
                (value == ValuesField2::Value3) ||
                (value == ValuesField2::Value4);
        }
    };

    /// @brief Enumeration, that has sparse and signed values, as well as
    ///     serialised using 2 bytes.
    using field2 =
        comms::field::EnumValue<
            FieldBase,
            ValuesField2,
            typename TOpt::message::EnumValuesFields::field2,
            comms::option::ContentsValidator<ValuesField2Validator>,
            comms::option::DefaultNumValue<(int)ValuesField2::Value1>
    >;

    /// @brief Enumeration type for the @ref field3
    /// @details The values are sparse and unsigned. They are serialised
    ///     using base-128 encoding.
    enum class ValuesField3 : std::uint16_t
    {
        Value1 = 5, ///< Value1
        Value2 = 100, ///< Value2
        Value3 = 127, ///< Value3,
        Value4 = 128, ///< Value4
        Value5 = 200 ///< Value5
    };

    /// @brief Value validator class for @ref field3
    struct ValuesField3Validator
    {
        template <typename TField>
        bool operator()(const TField& field) const
        {
            auto value = field.value();
            return
                (value == ValuesField3::Value1) ||
                (value == ValuesField3::Value2) ||
                (value == ValuesField3::Value3) ||
                (value == ValuesField3::Value4) ||
                (value == ValuesField3::Value5);
        }
    };

    /// @brief Enumeration, that has sparse and unsigned values, as well as
    ///     serialised using base-128 encoding.
    using field3 =
        comms::field::EnumValue<
            FieldBase,
            ValuesField3,
            typename TOpt::message::EnumValuesFields::field3,
            comms::option::ContentsValidator<ValuesField3Validator>,
            comms::option::VarLength<1, 2>,
            comms::option::DefaultNumValue<(int)ValuesField3::Value1>
    >;

    /// @brief All the fields bundled in std::tuple.
    using All = std::tuple<
        field1,
        field2,
        field3
    >;
};

/// @brief Accumulates various enum values.
/// @details Inherits from
///     @b comms::MessageBase
///     while providing @b TMsgBase as common interface class as well as
///     various implementation options. @n
///     See @ref EnumValuesFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
/// @tparam TOpt Extra options
template <typename TMsgBase, typename TOpt = demo::DefaultOptions>
class EnumValues : public
    comms::MessageBase<
        TMsgBase,
        typename TOpt::message::EnumValues,
        comms::option::StaticNumIdImpl<MsgId_EnumValues>,
        comms::option::FieldsImpl<typename EnumValuesFields<TOpt>::All>,
        comms::option::MsgType<EnumValues<TMsgBase, TOpt> >,
        comms::option::HasName
    >
{
    // Required for compilation with gcc earlier than v5.0,
    // later versions don't require this type definition.
    using Base =
        comms::MessageBase<
            TMsgBase,
            typename TOpt::message::EnumValues,
            comms::option::StaticNumIdImpl<MsgId_EnumValues>,
            comms::option::FieldsImpl<typename EnumValuesFields<TOpt>::All>,
            comms::option::MsgType<EnumValues<TMsgBase, TOpt> >,
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
    COMMS_MSG_FIELDS_ACCESS(field1, field2, field3);

    // Check serialisation lengths
    // For some reason VS2015 compiler fails when calls to doMinLength() and
    // doMaxLength() are performed inside static_assert.
    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static_assert(MsgMinLen == 4, "Unexpected min serialisation length");
    static_assert(MsgMaxLen == 5, "Unexpected max serialisation length");

    /// @brief Default constructor
    EnumValues() = default;

    /// @brief Copy constructor
    EnumValues(const EnumValues&) = default;

    /// @brief Move constructor
    EnumValues(EnumValues&& other) = default;

    /// @brief Destructor
    ~EnumValues() noexcept = default;

    /// @brief Copy assignment
    EnumValues& operator=(const EnumValues&) = default;

    /// @brief Move assignment
    EnumValues& operator=(EnumValues&&) = default;

    /// @brief Name of the message.
    static const char* doName()
    {
        return "EnumValues";
    }

};

}  // namespace message

}  // namespace demo



