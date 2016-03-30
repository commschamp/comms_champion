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
/// @brief Contains definition of Optionals message and its fields.

#pragma once

#include "demo/Message.h"

namespace comms_champion
{

namespace demo
{


namespace message
{

/// @brief Accumulates details of all the Optionals message fields.
/// @tparam TFieldBase base class for all the fields.
/// @see Optionals
template <typename TFieldBase>
struct OptionalsFields
{
    /// @brief Bits indices of the @ref field1 bitmask.
    enum
    {
        field1_enableField2, ///< index of the bit controlling existence of @ref field2
        field1_enableField3, ///< index of the bit controlling existence of @ref field3
        field1_numOfBits ///< number of bits in use
    };
    /// @brief Bitmask that is used to enable/disable other fields
    using field1 =
        comms::field::BitmaskValue<
            TFieldBase,
            comms::option::FixedLength<1>,
            comms::option::BitmaskReservedBits<0xfc, 0>
        >;

    /// @brief Optional 2 bytes unsigned integer value
    /// @details Existence of this field is controlled by bit 0 in @ref field1
    using field2 =
        comms::field::Optional<
            comms::field::IntValue<
                TFieldBase,
                std::uint16_t
            >
        >;

    /// @brief Optional string with 1 byte size information prefix.
    /// @details Existence of this field is controlled by bit 1 in @ref field1
    using field3 =
        comms::field::Optional<
            comms::field::String<
                TFieldBase,
                comms::option::SequenceSizeFieldPrefix<
                    comms::field::IntValue<
                        TFieldBase,
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
///     <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/classcomms_1_1MessageBase.html">comms::MessageBase</a>
///     while providing @b TMsgBase as common interface class as well as
///     @b comms::option::StaticNumIdImpl, @b comms::option::FieldsImpl, and
///     @b comms::option::DispatchImpl as options. @n
///     See @ref OptionalsFields for definition of the fields this message contains.
/// @tparam TMsgBase Common interface class for all the messages.
template <typename TMsgBase = Message>
class Optionals : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Optionals>,
        comms::option::FieldsImpl<typename OptionalsFields<typename TMsgBase::Field>::All>,
        comms::option::DispatchImpl<Optionals<TMsgBase> >
    >
{
    typedef comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Optionals>,
        comms::option::FieldsImpl<typename OptionalsFields<typename TMsgBase::Field>::All>,
        comms::option::DispatchImpl<Optionals<TMsgBase> >
    > Base;
public:

    /// @brief Index to access the fields
    enum FieldIdx
    {
        FieldIdx_field1, ///< field1 field, see @ref OptionalsFields::field1
        FieldIdx_field2, ///< field2 field, see @ref OptionalsFields::field2
        FieldIdx_field3, ///< field3 field, see @ref OptionalsFields::field3
        FieldIdx_numOfValues ///< number of available fields
    };

    static_assert(std::tuple_size<typename Base::AllFields>::value == FieldIdx_numOfValues,
        "Number of fields is incorrect");

    /// @brief Default constructor
    Optionals()
    {
        auto& allFields = Base::fields();
        auto& field2 = std::get<FieldIdx_field2>(allFields);
        auto& field3 = std::get<FieldIdx_field3>(allFields);

        field2.setMode(comms::field::OptionalMode::Missing);
        field3.setMode(comms::field::OptionalMode::Missing);
    }

    /// @brief Copy constructor
    Optionals(const Optionals&) = default;

    /// @brief Move constructor
    Optionals(Optionals&& other) = default;

    /// @brief Destructor
    virtual ~Optionals() = default;

    /// @brief Copy assignment
    Optionals& operator=(const Optionals&) = default;

    /// @brief Move assignment
    Optionals& operator=(Optionals&&) = default;

protected:
    typedef OptionalsFields<typename TMsgBase::Field> FieldsStruct;

    virtual comms::ErrorStatus readImpl(typename Base::ReadIterator& iter, std::size_t len) override
    {
        auto es = Base::template readFieldsUntil<FieldIdx_field2>(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto& allFields = Base::fields();
        auto& field1 = std::get<FieldIdx_field1>(allFields);
        auto& field2 = std::get<FieldIdx_field2>(allFields);
        auto& field3 = std::get<FieldIdx_field3>(allFields);

        auto field2Mode = comms::field::OptionalMode::Missing;
        if (field1.getBitValue(FieldsStruct::field1_enableField2)) {
            field2Mode = comms::field::OptionalMode::Exists;
        }

        auto field3Mode = comms::field::OptionalMode::Missing;
        if (field1.getBitValue(FieldsStruct::field1_enableField3)) {
            field3Mode = comms::field::OptionalMode::Exists;
        }

        field2.setMode(field2Mode);
        field3.setMode(field3Mode);
        return Base::template readFieldsFrom<FieldIdx_field2>(iter, len);
    }

    virtual bool refreshImpl() override
    {
        auto& allFields = Base::fields();
        auto& field1 = std::get<FieldIdx_field1>(allFields);
        auto& field2 = std::get<FieldIdx_field2>(allFields);
        auto& field3 = std::get<FieldIdx_field3>(allFields);

        auto field2ExpectedMode = comms::field::OptionalMode::Missing;
        if (field1.getBitValue(FieldsStruct::field1_enableField2)) {
            field2ExpectedMode = comms::field::OptionalMode::Exists;
        }

        auto field3ExpectedMode = comms::field::OptionalMode::Missing;
        if (field1.getBitValue(FieldsStruct::field1_enableField3)) {
            field3ExpectedMode = comms::field::OptionalMode::Exists;
        }

        bool refreshed = false;
        if (field2.getMode() != field2ExpectedMode) {
            field2.setMode(field2ExpectedMode);
            refreshed = true;
        }

        if (field3.getMode() != field3ExpectedMode) {
            field3.setMode(field3ExpectedMode);
            refreshed = true;
        }

        return refreshed;
    }
};

}  // namespace message

}  // namespace demo

}  // namespace comms_champion


