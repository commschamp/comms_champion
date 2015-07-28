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
#include "comms/field/adapters.h"
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

template <typename TField, typename TOpts>
using AdaptBasicFieldSerOffsetT =
    typename AdaptBasicFieldSerOffset<TField, TOpts, TOpts::HasSerOffset>::Type;


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

template <typename TField, typename TOpts>
using AdaptBasicFieldFixedLengthT =
    typename AdaptBasicFieldFixedLength<TField, TOpts, TOpts::HasFixedLengthLimit>::Type;

template <typename TField, typename TOpts, bool THasFixedBitLength>
struct AdaptBasicFieldFixedBitLength;

template <typename TField, typename TOpts>
struct AdaptBasicFieldFixedBitLength<TField, TOpts, true>
{
    static_assert(std::is_base_of<comms::field::category::NumericValueField, typename TField::Category>::value,
        "The FixedBitLength option is supported only for numeric value fields.");
    typedef comms::field::adapter::FixedBitLength<TOpts::FixedBitLength, TField> Type;
};

template <typename TField, typename TOpts>
struct AdaptBasicFieldFixedBitLength<TField, TOpts, false>
{
    typedef TField Type;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldFixedBitLengthT =
    typename AdaptBasicFieldFixedBitLength<TField, TOpts, TOpts::HasFixedBitLengthLimit>::Type;

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

template <typename TField, typename TOpts>
using AdaptBasicFieldVarLengthT =
    typename AdaptBasicFieldVarLength<TField, TOpts, TOpts::HasVarLengthLimits>::Type;

template <typename TField, typename TOpts, bool THasSequenceSizeFieldPrefix>
struct AdaptBasicFieldSequenceSizeFieldPrefix;

template <typename TField, typename TOpts>
struct AdaptBasicFieldSequenceSizeFieldPrefix<TField, TOpts, true>
{
    typedef comms::field::adapter::SequenceSizeFieldPrefix<typename TOpts::SequenceSizeFieldPrefix, TField> Type;
};

template <typename TField, typename TOpts>
struct AdaptBasicFieldSequenceSizeFieldPrefix<TField, TOpts, false>
{
    typedef TField Type;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldSequenceSizeFieldPrefixT =
    typename AdaptBasicFieldSequenceSizeFieldPrefix<TField, TOpts, TOpts::HasSequenceSizeFieldPrefix>::Type;

template <typename TField, bool THasSequenceSizeForcing>
struct AdaptBasicFieldSequenceSizeForcing;

template <typename TField>
struct AdaptBasicFieldSequenceSizeForcing<TField, true>
{
    typedef comms::field::adapter::SequenceSizeForcing<TField> Type;
};

template <typename TField>
struct AdaptBasicFieldSequenceSizeForcing<TField, false>
{
    typedef TField Type;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldSequenceSizeForcingT =
    typename AdaptBasicFieldSequenceSizeForcing<TField, TOpts::HasSequenceSizeForcing>::Type;

template <typename TField, typename TOpts, bool THasSequenceFixedSize>
struct AdaptBasicFieldSequenceFixedSize;

template <typename TField, typename TOpts>
struct AdaptBasicFieldSequenceFixedSize<TField, TOpts, true>
{
    typedef comms::field::adapter::SequenceFixedSize<TOpts::SequenceFixedSize, TField> Type;
};

template <typename TField, typename TOpts>
struct AdaptBasicFieldSequenceFixedSize<TField, TOpts, false>
{
    typedef TField Type;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldSequenceFixedSizeT =
    typename AdaptBasicFieldSequenceFixedSize<TField, TOpts, TOpts::HasSequenceFixedSize>::Type;


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

template <typename TField, typename TOpts>
using AdaptBasicFieldDefaultValueInitialiserT =
    typename AdaptBasicFieldDefaultValueInitialiser<TField, TOpts, TOpts::HasDefaultValueInitialiser>::Type;

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

template <typename TField, typename TOpts>
using AdaptBasicFieldCustomValidatorT =
    typename AdaptBasicFieldCustomValidator<TField, TOpts, TOpts::HasCustomValidator>::Type;

template <typename TField, bool THasFailOnInvalid>
struct AdaptBasicFieldFailOnInvalid;

template <typename TField>
struct AdaptBasicFieldFailOnInvalid<TField, true>
{
    typedef comms::field::adapter::FailOnInvalid<TField> Type;
};

template <typename TField>
struct AdaptBasicFieldFailOnInvalid<TField, false>
{
    typedef TField Type;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldFailOnInvalidT =
    typename AdaptBasicFieldFailOnInvalid<TField, TOpts::HasFailOnInvalid>::Type;

template <typename TField, bool THasIgnoreInvalid>
struct AdaptBasicFieldIgnoreInvalid;

template <typename TField>
struct AdaptBasicFieldIgnoreInvalid<TField, true>
{
    typedef comms::field::adapter::IgnoreInvalid<TField> Type;
};

template <typename TField>
struct AdaptBasicFieldIgnoreInvalid<TField, false>
{
    typedef TField Type;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldIgnoreInvalidT =
    typename AdaptBasicFieldIgnoreInvalid<TField, TOpts::HasIgnoreInvalid>::Type;

template <typename TBasic, typename... TOptions>
class AdaptBasicField
{
    typedef OptionsParser<TOptions...> ParsedOptions;
    typedef AdaptBasicFieldSerOffsetT<
        TBasic, ParsedOptions> SerOffsetAdapted;
    typedef AdaptBasicFieldFixedLengthT<
        SerOffsetAdapted, ParsedOptions> FixedLengthAdapted;
    typedef AdaptBasicFieldFixedBitLengthT<
        FixedLengthAdapted, ParsedOptions> FixedBitLengthAdapted;
    typedef AdaptBasicFieldVarLengthT<
        FixedBitLengthAdapted, ParsedOptions> VarLengthAdapted;
    typedef AdaptBasicFieldSequenceSizeFieldPrefixT<
        VarLengthAdapted, ParsedOptions> SequenceSizeFieldPrefixAdapted;
    typedef AdaptBasicFieldSequenceSizeForcingT<
        SequenceSizeFieldPrefixAdapted, ParsedOptions> SequenceSizeForcingAdapted;
    typedef AdaptBasicFieldSequenceFixedSizeT<
        SequenceSizeForcingAdapted, ParsedOptions> SequenceFixedSizeAdapted;
    typedef AdaptBasicFieldDefaultValueInitialiserT<
        SequenceFixedSizeAdapted, ParsedOptions> DefaultValueInitialiserAdapted;
    typedef AdaptBasicFieldCustomValidatorT<
        DefaultValueInitialiserAdapted, ParsedOptions> CustomValidatorAdapted;
    typedef AdaptBasicFieldFailOnInvalidT<
        CustomValidatorAdapted, ParsedOptions> FailOnInvalidAdapted;
    typedef AdaptBasicFieldIgnoreInvalidT<
        FailOnInvalidAdapted, ParsedOptions> IgnoreInvalidAdapted;
public:
    typedef IgnoreInvalidAdapted Type;
};

template <typename TBasic, typename... TOptions>
using AdaptBasicFieldT = typename AdaptBasicField<TBasic, TOptions...>::Type;

}  // namespace details

}  // namespace field

}  // namespace comms


