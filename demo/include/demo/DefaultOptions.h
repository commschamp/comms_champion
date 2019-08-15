//
// Copyright 2018 (C). Alex Robenko. All rights reserved.
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
/// @brief Contains definition of @ref demo::DefaultOptions.

#pragma once

#include "comms/options.h"

namespace demo
{

/// @brief Definition of the protocol default options
struct DefaultOptions
{
    /// @brief Extra options for classes in @ref demo::message namespace
    struct message
    {
        /// @brief Extra options for fields in @ref demo::message::BitfieldsFields struct
        struct BitfieldsFields
        {
            /// @brief Extra options for @ref demo::message::BitfieldsFields::field1_bitmask
            using field1_bitmask = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::BitfieldsFields::field1_enum
            using field1_enum = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::BitfieldsFields::field1_int1
            using field1_int1 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::BitfieldsFields::field1_int2
            using field1_int2 = comms::option::EmptyOption;
        };

        /// @brief Extra options for fields in @ref demo::message::BitmaskValuesFields struct
        struct BitmaskValuesFields
        {
            /// @brief Extra options for @ref demo::message::BitmaskValuesFields::field1
            using field1 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::BitmaskValuesFields::field2
            using field2 = comms::option::EmptyOption;
        };

        /// @brief Extra options for fields in @ref demo::message::EnumValuesFields struct
        struct EnumValuesFields
        {
            /// @brief Extra options for @ref demo::message::EnumValuesFields::field1
            using field1 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::EnumValuesFields::field2
            using field2 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::EnumValuesFields::field3
            using field3 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::EnumValuesFields::field4
            using field4 = comms::option::EmptyOption;
        };

        /// @brief Extra options for fields in @ref demo::message::FloatValuesFields struct
        struct FloatValuesFields
        {
            /// @brief Extra options for @ref demo::message::FloatValuesFields::field1
            using field1 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::FloatValuesFields::field2
            using field2 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::FloatValuesFields::field3
            using field3 = comms::option::EmptyOption;
        };

        /// @brief Extra options for fields in @ref demo::message::IntValuesFields struct
        struct IntValuesFields
        {
            /// @brief Extra options for @ref demo::message::IntValuesFields::field1
            using field1 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::IntValuesFields::field2
            using field2 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::IntValuesFields::field3
            using field3 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::IntValuesFields::field4
            using field4 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::IntValuesFields::field5
            using field5 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::IntValuesFields::field6
            using field6 = comms::option::EmptyOption;
        };

        /// @brief Extra options for fields in @ref demo::message::ListsFields struct
        struct ListsFields
        {
            /// @brief Extra options for @ref demo::message::ListsFields::field1
            using field1 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::ListsFields::field2
            using field2 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::ListsFields::field3Element
            using field3Element = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::ListsFields::field3
            using field3 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::ListsFields::field4Element
            using field4Element = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::ListsFields::field4
            using field4 = comms::option::EmptyOption;

            /// @brief Extra options for fields in @ref demo::message::ListsFields::field5Members struct
            struct field5Members
            {
                /// @brief Extra options for @ref demo::message::ListsFields::field5Members::mem1
                using mem1 = comms::option::EmptyOption;

                /// @brief Extra options for @ref demo::message::ListsFields::field5Members::mem2
                using mem2 = comms::option::EmptyOption;

                /// @brief Extra options for @ref demo::message::ListsFields::field5Members::mem3
                using mem3 = comms::option::EmptyOption;
            };

            /// @brief Extra options for @ref demo::message::ListsFields::field5Element
            using field5Element = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::ListsFields::field5
            using field5 = comms::option::EmptyOption;
        };

        /// @brief Extra options for fields in @ref demo::message::OptionalsFields struct
        struct OptionalsFields
        {
            /// @brief Extra options for @ref demo::message::OptionalsFields::field1
            using field1 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::OptionalsFields::field2
            using field2 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::OptionalsFields::field3
            using field3 = comms::option::EmptyOption;
        };

        /// @brief Extra options for fields in @ref demo::message::StringsFields struct
        struct StringsFields
        {
            /// @brief Extra options for @ref demo::message::StringsFields::field1
            using field1 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::StringsFields::field2
            using field2 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::StringsFields::field3
            using field3 = comms::option::EmptyOption;
        };

        /// @brief Extra options for fields in @ref demo::message::VariantsFields struct
        struct VariantsFields
        {
            /// @brief Extra options for @ref demo::message::VariantsFields::field1_var1
            using field1_var1 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::VariantsFields::field1_var2
            using field1_var2 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::VariantsFields::field1_var3
            using field1_var3 = comms::option::EmptyOption;

            /// @brief Extra options for @ref demo::message::VariantsFields::field1
            using field1 = comms::option::EmptyOption;
        };

        /// @brief Extra options for @ref demo::message::Bitfields message
        using Bitfields = comms::option::EmptyOption;

        /// @brief Extra options for @ref demo::message::BitmaskValues message
        using BitmaskValues = comms::option::EmptyOption;

        /// @brief Extra options for @ref demo::message::EnumValues message
        using EnumValues = comms::option::EmptyOption;

        /// @brief Extra options for @ref demo::message::FloatValues message
        using FloatValues = comms::option::EmptyOption;

        /// @brief Extra options for @ref demo::message::IntValues message
        using IntValues = comms::option::EmptyOption;

        /// @brief Extra options for @ref demo::message::Lists message
        using Lists = comms::option::EmptyOption;

        /// @brief Extra options for @ref demo::message::Optionals message
        using Optionals = comms::option::EmptyOption;

        /// @brief Extra options for @ref demo::message::Strings message
        using Strings = comms::option::EmptyOption;

        /// @brief Extra options for @ref demo::message::Variants message
        using Variants = comms::option::EmptyOption;
    };
};

} // namespace demo

