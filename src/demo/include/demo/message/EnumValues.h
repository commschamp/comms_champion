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
/// @brief Contains definition of EnumValues message and its fields.

#pragma once

#include "demo/Message.h"

namespace comms_champion
{

namespace demo
{


namespace message
{

/// @brief Accumulates details of all the EnumValues message fields.
/// @tparam TFieldBase base class for all the fields.
/// @see EnumValues
template <typename TFieldBase>
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
            TFieldBase,
            ValuesField1,
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
            TFieldBase,
            ValuesField2,
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
            TFieldBase,
            ValuesField3,
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
///     <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/classcomms_1_1MessageBase.html">comms::MessageBase</a>
///     while providing @b TMsgBase as common interface class as well as
///     @b comms::option::StaticNumIdImpl, @b comms::option::FieldsImpl, and
///     @b comms::option::DispatchImpl as options. @n
///     See @ref EnumValuesFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase = Message>
class EnumValues : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_EnumValues>,
        comms::option::FieldsImpl<typename EnumValuesFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<EnumValues<TMsgBase> >,
        comms::option::DispatchImpl
    >
{
    typedef comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_EnumValues>,
        comms::option::FieldsImpl<typename EnumValuesFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<EnumValues<TMsgBase> >,
        comms::option::DispatchImpl
    > Base;
public:

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Index to access the fields
    enum FieldIdx
    {
        FieldIdx_field1, ///< field1 field, see @ref EnumValuesFields::field1
        FieldIdx_field2, ///< field2 field, see @ref EnumValuesFields::field2
        FieldIdx_field3, ///< field3 field, see @ref EnumValuesFields::field3
        FieldIdx_numOfValues ///< number of available fields
    };

    /// @brief Access to fields, bundled into struct
    struct FieldsAsStruct
    {
        EnumValuesFields::field1& field1; ///< Access to field1
        EnumValuesFields::field2& field2; ///< Access to field2
        EnumValuesFields::field3& field3; ///< Access to field3
    };

    /// @brief Access to @b const fields, bundled into struct
    struct ConstFieldsAsStruct
    {
        const EnumValuesFields::field1& field1; ///< Access to field1
        const EnumValuesFields::field2& field2; ///< Access to field2
        const EnumValuesFields::field3& field3; ///< Access to field3
    };

    /// @brief Get access to fields, bundled into struct
    FieldsAsStruct fieldsAsStruct();

    /// @brief Get access to @b const fields, bundled into struct
    ConstFieldsAsStruct fieldsAsStruct() const;

#else
    COMMS_MSG_FIELDS_ACCESS(Base, field1, field2, field3);
#endif

    /// @brief Default constructor
    EnumValues() = default;

    /// @brief Copy constructor
    EnumValues(const EnumValues&) = default;

    /// @brief Move constructor
    EnumValues(EnumValues&& other) = default;

    /// @brief Destructor
    virtual ~EnumValues() = default;

    /// @brief Copy assignment
    EnumValues& operator=(const EnumValues&) = default;

    /// @brief Move assignment
    EnumValues& operator=(EnumValues&&) = default;
};

}  // namespace message

}  // namespace demo

}  // namespace comms_champion


