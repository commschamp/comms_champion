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
/// @brief Contains definition of Strings message and its fields.

#pragma once

#include "demo/Message.h"

namespace comms_champion
{

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
///     <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/classcomms_1_1MessageBase.html">comms::MessageBase</a>
///     while providing @b TMsgBase as common interface class as well as
///     @b comms::option::StaticNumIdImpl, @b comms::option::FieldsImpl, and
///     @b comms::option::DispatchImpl as options. @n
///     See @ref StringsFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase = Message>
class Strings : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Strings>,
        comms::option::FieldsImpl<typename StringsFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<Strings<TMsgBase> >,
        comms::option::DispatchImpl
    >
{
    typedef comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Strings>,
        comms::option::FieldsImpl<typename StringsFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<Strings<TMsgBase> >,
        comms::option::DispatchImpl
    > Base;
public:

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Index to access the fields
    enum FieldIdx
    {
        FieldIdx_field1, ///< field1 field, see @ref StringsFields::field1
        FieldIdx_field2, ///< field2 field, see @ref StringsFields::field2
        FieldIdx_field3, ///< field3 field, see @ref StringsFields::field3
        FieldIdx_numOfValues ///< number of available fields
    };

    /// @brief Access to fields, bundled into struct
    struct FieldsAsStruct
    {
        StringsFields::field1& field1; ///< Access to field1
        StringsFields::field2& field2; ///< Access to field2
        StringsFields::field3& field3; ///< Access to field3
    };

    /// @brief Access to @b const fields, bundled into struct
    struct ConstFieldsAsStruct
    {
        const StringsFields::field1& field1; ///< Access to field1
        const StringsFields::field2& field2; ///< Access to field2
        const StringsFields::field3& field3; ///< Access to field3
    };

    /// @brief Get access to fields, bundled into struct
    FieldsAsStruct fieldsAsStruct();

    /// @brief Get access to @b const fields, bundled into struct
    ConstFieldsAsStruct fieldsAsStruct() const;

#else
    COMMS_MSG_FIELDS_ACCESS(Base, field1, field2, field3);
#endif

    /// @brief Default constructor
    Strings() = default;

    /// @brief Copy constructor
    Strings(const Strings&) = default;

    /// @brief Move constructor
    Strings(Strings&& other) = default;

    /// @brief Destructor
    virtual ~Strings() = default;

    /// @brief Copy assignment
    Strings& operator=(const Strings&) = default;

    /// @brief Move assignment
    Strings& operator=(Strings&&) = default;
};

}  // namespace message

}  // namespace demo

}  // namespace comms_champion


