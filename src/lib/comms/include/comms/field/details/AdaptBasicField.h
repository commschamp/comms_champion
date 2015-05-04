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

template <typename TField, typename TOpts, bool THasSerOffset>
struct AdaptBasicFieldSerOffset;

template <typename TField, typename TOpts>
struct AdaptBasicFieldSerOffset<TField, TOpts, true>
{
    static_assert(std::is_base_of<comms::field::category::NumericValueField, typename TField::Category>::value,
        "The FixedLength option is supported only for numeric value fields.");
    typedef comms::field::adapter::SerOffset<TOpts::SerOffset, TField> Type;
};

template <typename TField, typename TOpts>
struct AdaptBasicFieldSerOffset<TField, TOpts, false>
{
    typedef TField Type;
};

template <typename TField, typename TOpts, bool THasSerOffset>
using AdaptBasicFieldSerOffsetT =
    typename AdaptBasicFieldSerOffset<TField, TOpts, THasSerOffset>::Type;


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


template <typename TField, typename TOpts, bool THasVarLengths>
struct AdaptBasicFieldVarLength;

template <typename TField, typename TOpts>
struct AdaptBasicFieldVarLength<TField, TOpts, true>
{
    static_assert(std::is_base_of<comms::field::category::NumericValueField, typename TField::Category>::value,
        "The VarLength option is supported only for numeric value fields.");
    typedef comms::field::adapter::VarLength<TOpts::MinVarLength, TOpts::MaxVarLength, TField> Type;
};

template <typename TField, typename TOpts>
struct AdaptBasicFieldVarLength<TField, TOpts, false>
{
    typedef TField Type;
};

template <typename TField, typename TOpts, bool THasVarLengths>
using AdaptBasicFieldVarLengthT =
    typename AdaptBasicFieldVarLength<TField, TOpts, THasVarLengths>::Type;


template <typename TField, typename TOpts, bool THasDefaultValueInitialiser>
struct AdaptBasicFieldDefaultValueInitialiser;

template <typename TField, typename TOpts>
struct AdaptBasicFieldDefaultValueInitialiser<TField, TOpts, true>
{
    typedef comms::field::adapter::DefaultValueInitialiser<typename TOpts::DefaultValueInitialiser, TField> Type;
};

template <typename TField, typename TOpts>
struct AdaptBasicFieldDefaultValueInitialiser<TField, TOpts, false>
{
    typedef TField Type;
};

template <typename TField, typename TOpts, bool THasDefaultValueInitialiser>
using AdaptBasicFieldDefaultValueInitialiserT =
    typename AdaptBasicFieldDefaultValueInitialiser<TField, TOpts, THasDefaultValueInitialiser>::Type;

template <typename TField, typename TOpts, bool THasCustomValidator>
struct AdaptBasicFieldCustomValidator;

template <typename TField, typename TOpts>
struct AdaptBasicFieldCustomValidator<TField, TOpts, true>
{
    typedef comms::field::adapter::CustomValidator<typename TOpts::CustomValidator, TField> Type;
};

template <typename TField, typename TOpts>
struct AdaptBasicFieldCustomValidator<TField, TOpts, false>
{
    typedef TField Type;
};

template <typename TField, typename TOpts, bool THasCustomValidator>
using AdaptBasicFieldCustomValidatorT =
    typename AdaptBasicFieldCustomValidator<TField, TOpts, THasCustomValidator>::Type;



template <typename TBasic, typename... TOptions>
class AdaptBasicField
{
    typedef OptionsParser<TOptions...> ParsedOptions;
    typedef AdaptBasicFieldSerOffsetT<
        TBasic, ParsedOptions, ParsedOptions::HasSerOffset> SerOffsetAdapted;
    typedef AdaptBasicFieldFixedLengthT<
        SerOffsetAdapted, ParsedOptions, ParsedOptions::HasFixedLengthLimit> FixedLengthAdapted;
    typedef AdaptBasicFieldVarLengthT<
        FixedLengthAdapted, ParsedOptions, ParsedOptions::HasVarLengthLimits> VarLengthAdapted;
    typedef AdaptBasicFieldDefaultValueInitialiserT<
        VarLengthAdapted, ParsedOptions, ParsedOptions::HasDefaultValueInitialiser> DefaultValueInitialiserAdapted;
    typedef AdaptBasicFieldCustomValidatorT<
        DefaultValueInitialiserAdapted, ParsedOptions, ParsedOptions::HasCustomValidator> CustomValidatorAdapted;
public:
    typedef CustomValidatorAdapted Type;
};

template <typename TBasic, typename... TOptions>
using AdaptBasicFieldT = typename AdaptBasicField<TBasic, TOptions...>::Type;

}  // namespace details

}  // namespace field

}  // namespace comms


