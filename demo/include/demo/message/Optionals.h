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
/// @brief Contains definition of Optionals message and its fields.

#pragma once

#include "comms/fields.h"
#include "comms/MessageBase.h"
#include "demo/MsgId.h"
#include "demo/FieldBase.h"

namespace demo
{

namespace message
{

/// @brief Accumulates details of all the Optionals message fields.
/// @see Optionals
struct OptionalsFields
{
    /// @brief Bitmask that is used to enable/disable other fields
    struct field1 : public
        comms::field::BitmaskValue<
            FieldBase,
            comms::option::FixedLength<1>,
            comms::option::BitmaskReservedBits<0xfc, 0>
        >
    {
        /// @brief Provide names for internal bits.
        /// @details See definition of @b COMMS_BITMASK_BITS macro
        ///     related to @b comms::field::BitmaskValue class from COMMS library
        ///     for details.
        COMMS_BITMASK_BITS(enableField2, enableField3);
    };

    /// @brief Optional 2 bytes unsigned integer value
    /// @details Existence of this field is controlled by bit 0 in @ref field1
    using field2 =
        comms::field::Optional<
            comms::field::IntValue<
                FieldBase,
                std::uint16_t
            >
        >;

    /// @brief Optional string with 1 byte size information prefix.
    /// @details Existence of this field is controlled by bit 1 in @ref field1
    using field3 =
        comms::field::Optional<
            comms::field::String<
                FieldBase,
                comms::option::SequenceSizeFieldPrefix<
                    comms::field::IntValue<
                        FieldBase,
                        std::uint8_t
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

/// @brief Accumulates various string fields.
/// @details Inherits from
///     @b comms::MessageBase
///     while providing @b TMsgBase as common interface class as well as
///     various implementation options. @n
///     See @ref OptionalsFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase>
class Optionals : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Optionals>,
        comms::option::FieldsImpl<OptionalsFields::All>,
        comms::option::MsgType<Optionals<TMsgBase> >,
        comms::option::HasDoRefresh
    >
{
    // Required for compilation with gcc earlier than v5.0,
    // later versions don't require this type definition.
    using Base =
        comms::MessageBase<
            TMsgBase,
            comms::option::StaticNumIdImpl<MsgId_Optionals>,
            comms::option::FieldsImpl<OptionalsFields::All>,
            comms::option::MsgType<Optionals<TMsgBase> >,
            comms::option::HasDoRefresh
        >;

public:
    /// @brief Allow access to internal fields.
    /// @details See definition of @b COMMS_MSG_FIELDS_ACCESS macro
    ///     related to @b comms::MessageBase class from COMMS library
    ///     for details.
    ///
    COMMS_MSG_FIELDS_ACCESS(field1, field2, field3);

    /// @brief Default constructor
    Optionals()
    {
        field_field2().setMissing();
        field_field3().setMissing();
    }

    /// @brief Copy constructor
    Optionals(const Optionals&) = default;

    /// @brief Move constructor
    Optionals(Optionals&& other) = default;

    /// @brief Destructor
    ~Optionals() = default;

    /// @brief Copy assignment
    Optionals& operator=(const Optionals&) = default;

    /// @brief Move assignment
    Optionals& operator=(Optionals&&) = default;

    /// @brief Implement custom read functionality
    /// @param[in, out] iter Reference to iterator used for reading.
    /// @param[in] len Number of remaining bytes in the buffer to read
    template <typename TIter>
    comms::ErrorStatus doRead(TIter& iter, std::size_t len)
    {
        using Base = typename std::decay<decltype(comms::toMessageBase(*this))>::type;
        auto es = Base::template readFieldsUntil<FieldIdx_field2>(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        using Field1Type = typename std::decay<decltype(field_field1())>::type;
        auto field2Mode = comms::field::OptionalMode::Missing;
        if (field_field1().getBitValue(Field1Type::BitIdx_enableField2)) {
            field2Mode = comms::field::OptionalMode::Exists;
        }

        auto field3Mode = comms::field::OptionalMode::Missing;
        if (field_field1().getBitValue(Field1Type::BitIdx_enableField3)) {
            field3Mode = comms::field::OptionalMode::Exists;
        }

        field_field2().setMode(field2Mode);
        field_field3().setMode(field3Mode);
        return Base::template readFieldsFrom<FieldIdx_field2>(iter, len);
    }

    /// @brief Implement custom refresh functionality.
    /// @details Brings the message fields into a consistent state.
    /// @return @b true in case any of the optional fields has been updated,
    ///     @b false otherwise.
    bool doRefresh()
    {
        using Field1Type = typename std::decay<decltype(field_field1())>::type;
        auto field2ExpectedMode = comms::field::OptionalMode::Missing;
        if (field_field1().getBitValue(Field1Type::BitIdx_enableField2)) {
            field2ExpectedMode = comms::field::OptionalMode::Exists;
        }

        auto field3ExpectedMode = comms::field::OptionalMode::Missing;
        if (field_field1().getBitValue(Field1Type::BitIdx_enableField3)) {
            field3ExpectedMode = comms::field::OptionalMode::Exists;
        }

        bool refreshed = false;
        if (field_field2().getMode() != field2ExpectedMode) {
            field_field2().setMode(field2ExpectedMode);
            refreshed = true;
        }

        if (field_field3().getMode() != field3ExpectedMode) {
            field_field3().setMode(field3ExpectedMode);
            refreshed = true;
        }

        return refreshed;
    }

};

}  // namespace message

}  // namespace demo

