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
/// @brief Contains definition of Lists message and its fields.

#pragma once

#include "demo/Message.h"

namespace comms_champion
{

namespace demo
{


namespace message
{

/// @brief Accumulates details of all the Lists message fields.
/// @tparam TFieldBase base class for all the fields.
/// @see Lists
template <typename TFieldBase>
struct ListsFields
{
    /// @brief Raw data list that uses 2 bytes size prefix
    using field1 =
        comms::field::ArrayList<
            TFieldBase,
            std::uint8_t,
            comms::option::SequenceSizeFieldPrefix<
                comms::field::IntValue<
                    TFieldBase,
                    std::uint16_t
                >
            >
        >;

    /// @brief List of 2 bytes integer value fields, with fixed size of 3 elements
    using field2 =
        comms::field::ArrayList<
            TFieldBase,
            comms::field::IntValue<
                TFieldBase,
                std::int16_t
            >,
            comms::option::SequenceFixedSize<3>
        >;

    /// @brief List of bundles, every bundle has two integer values member fields.
    using field3 =
        comms::field::ArrayList<
            TFieldBase,
            comms::field::Bundle<
                TFieldBase,
                std::tuple<
                    comms::field::IntValue<
                        TFieldBase,
                        std::uint16_t
                    >,
                    comms::field::IntValue<
                        TFieldBase,
                        std::int8_t
                    >
                >
            >
        >;


    /// @brief All the fields bundled in std::tuple.
    using All = std::tuple<
        field1,
        field2,
        field3
    >;
};

/// @brief Accumulates various list fields.
/// @details Inherits from
///     <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/classcomms_1_1MessageBase.html">comms::MessageBase</a>
///     while providing @b TMsgBase as common interface class as well as
///     @b comms::option::StaticNumIdImpl, @b comms::option::FieldsImpl, and
///     @b comms::option::DispatchImpl as options. @n
///     See @ref ListsFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase = Message>
class Lists : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Lists>,
        comms::option::FieldsImpl<typename ListsFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<Lists<TMsgBase> >
    >
{
    typedef comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Lists>,
        comms::option::FieldsImpl<typename ListsFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<Lists<TMsgBase> >
    > Base;
public:

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Index to access the fields
    enum FieldIdx
    {
        FieldIdx_field1, ///< field1 field, see @ref ListsFields::field1
        FieldIdx_field2, ///< field2 field, see @ref ListsFields::field2
        FieldIdx_field3, ///< field3 field, see @ref ListsFields::field3
        FieldIdx_numOfValues ///< number of available fields
    };

    /// @brief Access to fields, bundled into struct
    struct FieldsAsStruct
    {
        ListsFields::field1& field1; ///< Access to field1
        ListsFields::field2& field2; ///< Access to field2
        ListsFields::field3& field3; ///< Access to field3
    };

    /// @brief Access to @b const fields, bundled into struct
    struct ConstFieldsAsStruct
    {
        const ListsFields::field1& field1; ///< Access to field1
        const ListsFields::field2& field2; ///< Access to field2
        const ListsFields::field3& field3; ///< Access to field3
    };

    /// @brief Get access to fields, bundled into struct
    FieldsAsStruct fieldsAsStruct();

    /// @brief Get access to @b const fields, bundled into struct
    ConstFieldsAsStruct fieldsAsStruct() const;

#else
    COMMS_MSG_FIELDS_ACCESS(Base, field1, field2, field3);
#endif

    /// @brief Default constructor
    Lists() = default;

    /// @brief Copy constructor
    Lists(const Lists&) = default;

    /// @brief Move constructor
    Lists(Lists&& other) = default;

    /// @brief Destructor
    virtual ~Lists() = default;

    /// @brief Copy assignment
    Lists& operator=(const Lists&) = default;

    /// @brief Move assignment
    Lists& operator=(Lists&&) = default;
};

}  // namespace message

}  // namespace demo

}  // namespace comms_champion


