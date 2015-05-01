//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include "comms/field/category.h"
#include "comms/field/adapter/adapters.h"
#include "OptionsParser.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename TField, typename TOpts, bool THasFixedLength>
struct AdaptBasicFieldFixedLength;

template <typename TField, typename TOpts>
struct AdaptBasicFieldFixedLength<TField, TOpts, true>
{
    static_assert(std::is_base_of<comms::field::category::NumericValueField, typename TField::Category>::value,
        "The FixedLength option is supported only for numeric value fields.");
    typedef comms::field::adapter::FixedLength<TOpts::FixedLength, TField> Type;
};

template <typename TField, typename TOpts>
struct AdaptBasicFieldFixedLength<TField, TOpts, false>
{
    typedef TField Type;
};

template <typename TField, typename TOpts, bool THasFixedLength>
using AdaptBasicFieldFixedLengthT =
    typename AdaptBasicFieldFixedLength<TField, TOpts, THasFixedLength>::Type;


template <typename TBasic, typename... TOptions>
class AdaptBasicField
{
    typedef OptionsParser<TOptions...> ParsedOptions;
    typedef AdaptBasicFieldFixedLengthT<
        TBasic, ParsedOptions, ParsedOptions::HasFixedLengthLimit> Field1;
public:
    typedef Field1 Type;
};

template <typename TBasic, typename... TOptions>
using AdaptBasicFieldT = typename AdaptBasicField<TBasic, TOptions...>::Type;

}  // namespace details

}  // namespace field

}  // namespace comms


