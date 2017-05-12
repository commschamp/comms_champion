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
/// @brief Contains definition of Variants message and its fields.

#pragma once

#include "demo/Message.h"

namespace demo
{

namespace message
{

/// @brief Accumulates details of all the Variants message fields.
/// @tparam TFieldBase base class for all the fields.
/// @see Variants
template <typename TFieldBase>
struct VariantsFields
{
    /// @brief Enumerator vor @ref varIdField
    enum class VarId : std::uint8_t
    {
        Elem1, ///< Value to identify @ref field1_var1
        Elem2, ///< Value to identify @ref field1_var2
        Elem3, ///< Value to identify @ref field1_var3
        NumOfValues ///< number of available values
    };

    /// @brief field used to identify the value of the variant field @ref field1.
    template <VarId TId>
    using varIdField =
        comms::field::EnumValue<
            TFieldBase,
            VarId,
            comms::option::DefaultNumValue<(int)TId>,
            comms::option::ValidNumValueRange<(int)TId, (int)TId>,
            comms::option::FailOnInvalid<>
        >;

    /// @brief First type that can be stored in @ref field1 variant field.
    /// @details Identified by @ref VarId::Elem1 (0) value which is followed by
    ///     the 1 byte unsigned integer.
    struct field1_var1 : public
        comms::field::Bundle<
            TFieldBase,
            std::tuple<
                varIdField<VarId::Elem1>,
                comms::field::IntValue<
                    TFieldBase,
                    std::uint8_t
                >
            >
        >
    {
        /// @brief Allow access to internal fields.
        /// @details See definition of @b COMMS_FIELD_MEMBERS_ACCESS macro
        ///     related to @b comms::field::Bundle class from COMMS library
        ///     for details. @n
        ///     The names are:
        ///     @b id for @ref varIdField<VarId::Elem1>
        ///     @b value for 1 byte unsigned integer field.
        COMMS_FIELD_MEMBERS_ACCESS(id, value);
    };

    /// @brief Second type that can be stored in @ref field1 variant field.
    /// @details Identified by @ref VarId::Elem2 (1) value which is followed by
    ///     the 4 bytes unsigned integer.
    struct field1_var2 : public
        comms::field::Bundle<
            TFieldBase,
            std::tuple<
                varIdField<VarId::Elem2>,
                comms::field::IntValue<
                    TFieldBase,
                    std::uint32_t
                >
            >
        >
    {
        /// @brief Allow access to internal fields.
        /// @details See definition of @b COMMS_FIELD_MEMBERS_ACCESS macro
        ///     related to @b comms::field::Bundle class from COMMS library
        ///     for details. @n
        ///     The names are:
        ///     @b id for @ref varIdField<VarId::Elem1>
        ///     @b value for 4 bytes unsigned integer field.
        COMMS_FIELD_MEMBERS_ACCESS(id, value);
    };

    /// @brief Third type that can be stored in @ref field1 variant field.
    /// @details Identified by @ref VarId::Elem3 (2) value which is followed by
    ///     the string prefixed with its lengths (1 byte).
    struct field1_var3 : public
        comms::field::Bundle<
            TFieldBase,
            std::tuple<
                varIdField<VarId::Elem3>,
                comms::field::String<
                    TFieldBase,
                    comms::option::SequenceSizeFieldPrefix<
                        comms::field::IntValue<
                            TFieldBase,
                            std::uint8_t
                        >
                    >
                >
            >
        >
    {
        /// @brief Allow access to internal fields.
        /// @details See definition of @b COMMS_FIELD_MEMBERS_ACCESS macro
        ///     related to @b comms::field::Bundle class from COMMS library
        ///     for details. @n
        ///     The names are:
        ///     @b id for @ref varIdField<VarId::Elem1>
        ///     @b value for string field prefixed with its size.
        COMMS_FIELD_MEMBERS_ACCESS(id, value);
    };

    /// @brief Variant field.
    /// @details Can be one of the following:
    ///     @li @ref field1_var1
    ///     @li @ref field1_var2
    ///     @li @ref field1_var3
    struct field1 : public
        comms::field::Variant<
            TFieldBase,
            std::tuple<
                field1_var1,
                field1_var2,
                field1_var3
            >
        >
    {
        /// @brief Allow access to internal fields.
        /// @details See definition of @b COMMS_FIELD_MEMBERS_ACCESS macro
        ///     related to @b comms::field::Bundle class from COMMS library
        ///     for details. @n
        ///     The names are:
        ///     @b val1 for @ref field1_var1
        ///     @b val2 for @ref field1_var2
        ///     @b val3 for @ref field1_var3
        COMMS_VARIANT_MEMBERS_ACCESS(val1, val2, val3);
    };

    /// @brief All the fields bundled in std::tuple.
    using All = std::tuple<
        field1
    >;
};

/// @brief Accumulates various list fields.
/// @details Inherits from
///     @b comms::MessageBase
///     while providing @b TMsgBase as common interface class as well as
///     various implementation options. @n
///     See @ref VariantsFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase = Message>
class Variants : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Variants>,
        comms::option::FieldsImpl<typename VariantsFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<Variants<TMsgBase> >
    >
{
    typedef comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Variants>,
        comms::option::FieldsImpl<typename VariantsFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<Variants<TMsgBase> >
    > Base;
public:

    /// @brief Allow access to internal fields.
    /// @details See definition of @b COMMS_MSG_FIELDS_ACCESS macro
    ///     related to @b comms::MessageBase class from COMMS library
    ///     for details.
    ///
    COMMS_MSG_FIELDS_ACCESS(field1);

    /// @brief Default constructor
    Variants() = default;

    /// @brief Copy constructor
    Variants(const Variants&) = default;

    /// @brief Move constructor
    Variants(Variants&& other) = default;

    /// @brief Destructor
    virtual ~Variants() = default;

    /// @brief Copy assignment
    Variants& operator=(const Variants&) = default;

    /// @brief Move assignment
    Variants& operator=(Variants&&) = default;
};

}  // namespace message

}  // namespace demo


