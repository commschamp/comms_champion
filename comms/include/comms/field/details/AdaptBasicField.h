//
// Copyright 2015 - 2017 (C). Alex Robenko. All rights reserved.
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

template <bool THasCustomValueReader>
struct AdaptBasicFieldCustomValueReader;

template <>
struct AdaptBasicFieldCustomValueReader<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::CustomValueReader<typename TOpts::CustomValueReader, TField>;
};

template <>
struct AdaptBasicFieldCustomValueReader<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldCustomValueReaderT =
    typename AdaptBasicFieldCustomValueReader<TOpts::HasCustomValueReader>::template Type<TField, TOpts>;

template <bool THasSerOffset>
struct AdaptBasicFieldSerOffset;

template <>
struct AdaptBasicFieldSerOffset<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::SerOffset<TOpts::SerOffset, TField>;
};

template <>
struct AdaptBasicFieldSerOffset<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldSerOffsetT =
    typename AdaptBasicFieldSerOffset<TOpts::HasSerOffset>::template Type<TField, TOpts>;


template <bool THasFixedLength>
struct AdaptBasicFieldFixedLength;

template <>
struct AdaptBasicFieldFixedLength<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::FixedLength<TOpts::FixedLength, TField>;
};

template <>
struct AdaptBasicFieldFixedLength<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldFixedLengthT =
    typename AdaptBasicFieldFixedLength<TOpts::HasFixedLengthLimit>::template Type<TField, TOpts>;

template <bool THasFixedBitLength>
struct AdaptBasicFieldFixedBitLength;

template <>
struct AdaptBasicFieldFixedBitLength<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::FixedBitLength<TOpts::FixedBitLength, TField>;
};

template <>
struct AdaptBasicFieldFixedBitLength<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldFixedBitLengthT =
    typename AdaptBasicFieldFixedBitLength<TOpts::HasFixedBitLengthLimit>::template Type<TField, TOpts>;

template <bool THasVarLengths>
struct AdaptBasicFieldVarLength;

template <>
struct AdaptBasicFieldVarLength<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::VarLength<TOpts::MinVarLength, TOpts::MaxVarLength, TField>;
};

template <>
struct AdaptBasicFieldVarLength<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldVarLengthT =
    typename AdaptBasicFieldVarLength<TOpts::HasVarLengthLimits>::template Type<TField, TOpts>;

template <bool THasSequenceSizeForcing>
struct AdaptBasicFieldSequenceSizeForcing;

template <>
struct AdaptBasicFieldSequenceSizeForcing<true>
{
    template <typename TField>
    using Type = comms::field::adapter::SequenceSizeForcing<TField>;
};

template <>
struct AdaptBasicFieldSequenceSizeForcing<false>
{
    template <typename TField>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldSequenceSizeForcingT =
    typename AdaptBasicFieldSequenceSizeForcing<TOpts::HasSequenceSizeForcing>::template Type<TField>;

template <bool THasSequenceFixedSize>
struct AdaptBasicFieldSequenceFixedSize;

template <>
struct AdaptBasicFieldSequenceFixedSize<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::SequenceFixedSize<TOpts::SequenceFixedSize, TField>;
};

template <>
struct AdaptBasicFieldSequenceFixedSize<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldSequenceFixedSizeT =
    typename AdaptBasicFieldSequenceFixedSize<TOpts::HasSequenceFixedSize>::template Type<TField, TOpts>;

template <bool THasSequenceSizeFieldPrefix>
struct AdaptBasicFieldSequenceSizeFieldPrefix;

template <>
struct AdaptBasicFieldSequenceSizeFieldPrefix<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::SequenceSizeFieldPrefix<typename TOpts::SequenceSizeFieldPrefix, TField>;
};

template <>
struct AdaptBasicFieldSequenceSizeFieldPrefix<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldSequenceSizeFieldPrefixT =
    typename AdaptBasicFieldSequenceSizeFieldPrefix<TOpts::HasSequenceSizeFieldPrefix>::template Type<TField, TOpts>;

template <bool THasSequenceTrailingFieldSuffix>
struct AdaptBasicFieldSequenceTrailingFieldSuffix;

template <>
struct AdaptBasicFieldSequenceTrailingFieldSuffix<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::SequenceTrailingFieldSuffix<typename TOpts::SequenceTrailingFieldSuffix, TField>;
};

template <>
struct AdaptBasicFieldSequenceTrailingFieldSuffix<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldSequenceTrailingFieldSuffixT =
    typename AdaptBasicFieldSequenceTrailingFieldSuffix<TOpts::HasSequenceTrailingFieldSuffix>::template Type<TField, TOpts>;

template <bool THasSequenceTerminationFieldSuffix>
struct AdaptBasicFieldSequenceTerminationFieldSuffix;

template <>
struct AdaptBasicFieldSequenceTerminationFieldSuffix<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::SequenceTerminationFieldSuffix<typename TOpts::SequenceTerminationFieldSuffix, TField>;
};

template <>
struct AdaptBasicFieldSequenceTerminationFieldSuffix<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldSequenceTerminationFieldSuffixT =
    typename AdaptBasicFieldSequenceTerminationFieldSuffix<TOpts::HasSequenceTerminationFieldSuffix>::template Type<TField, TOpts>;

template <bool THasDefaultValueInitialiser>
struct AdaptBasicFieldDefaultValueInitialiser;

template <>
struct AdaptBasicFieldDefaultValueInitialiser<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::DefaultValueInitialiser<typename TOpts::DefaultValueInitialiser, TField>;
};

template <>
struct AdaptBasicFieldDefaultValueInitialiser<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldDefaultValueInitialiserT =
    typename AdaptBasicFieldDefaultValueInitialiser<TOpts::HasDefaultValueInitialiser>::template Type<TField, TOpts>;

template <bool THasCustomValidator>
struct AdaptBasicFieldCustomValidator;

template <>
struct AdaptBasicFieldCustomValidator<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::CustomValidator<typename TOpts::CustomValidator, TField>;
};

template <>
struct AdaptBasicFieldCustomValidator<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldCustomValidatorT =
    typename AdaptBasicFieldCustomValidator<TOpts::HasCustomValidator>::template Type<TField, TOpts>;

template <bool THasFailOnInvalid>
struct AdaptBasicFieldFailOnInvalid;

template <>
struct AdaptBasicFieldFailOnInvalid<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::FailOnInvalid<TOpts::FailOnInvalidStatus, TField>;
};

template <>
struct AdaptBasicFieldFailOnInvalid<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldFailOnInvalidT =
    typename AdaptBasicFieldFailOnInvalid<TOpts::HasFailOnInvalid>::template Type<TField, TOpts>;

template <bool THasIgnoreInvalid>
struct AdaptBasicFieldIgnoreInvalid;

template <>
struct AdaptBasicFieldIgnoreInvalid<true>
{
    template <typename TField>
    using Type = comms::field::adapter::IgnoreInvalid<TField>;
};

template <>
struct AdaptBasicFieldIgnoreInvalid<false>
{
    template <typename TField>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptBasicFieldIgnoreInvalidT =
    typename AdaptBasicFieldIgnoreInvalid<TOpts::HasIgnoreInvalid>::template Type<TField>;

template <typename TBasic, typename... TOptions>
class AdaptBasicField
{
    using ParsedOptions = OptionsParser<TOptions...>;
    using CustomReaderAdapted = AdaptBasicFieldCustomValueReaderT<
        TBasic, ParsedOptions>;
    using SerOffsetAdapted = AdaptBasicFieldSerOffsetT<
        CustomReaderAdapted, ParsedOptions>;
    using FixedLengthAdapted = AdaptBasicFieldFixedLengthT<
        SerOffsetAdapted, ParsedOptions>;
    using FixedBitLengthAdapted = AdaptBasicFieldFixedBitLengthT<
        FixedLengthAdapted, ParsedOptions>;
    using VarLengthAdapted = AdaptBasicFieldVarLengthT<
        FixedBitLengthAdapted, ParsedOptions>;
    using SequenceSizeForcingAdapted = AdaptBasicFieldSequenceSizeForcingT<
        VarLengthAdapted, ParsedOptions>;
    using SequenceFixedSizeAdapted = AdaptBasicFieldSequenceFixedSizeT<
        SequenceSizeForcingAdapted, ParsedOptions>;
    using SequenceSizeFieldPrefixAdapted = AdaptBasicFieldSequenceSizeFieldPrefixT<
        SequenceFixedSizeAdapted, ParsedOptions>;
    using SequenceTrailingFieldSuffixAdapted = AdaptBasicFieldSequenceTrailingFieldSuffixT<
        SequenceSizeFieldPrefixAdapted, ParsedOptions>;
    using SequenceTerminationFieldSuffixAdapted = AdaptBasicFieldSequenceTerminationFieldSuffixT<
        SequenceTrailingFieldSuffixAdapted, ParsedOptions>;
    using DefaultValueInitialiserAdapted = AdaptBasicFieldDefaultValueInitialiserT<
        SequenceTerminationFieldSuffixAdapted, ParsedOptions>;
    using CustomValidatorAdapted = AdaptBasicFieldCustomValidatorT<
        DefaultValueInitialiserAdapted, ParsedOptions>;
    using FailOnInvalidAdapted = AdaptBasicFieldFailOnInvalidT<
        CustomValidatorAdapted, ParsedOptions>;
    using IgnoreInvalidAdapted = AdaptBasicFieldIgnoreInvalidT<
        FailOnInvalidAdapted, ParsedOptions>;
public:
    using Type = IgnoreInvalidAdapted;
};

template <typename TBasic, typename... TOptions>
using AdaptBasicFieldT = typename AdaptBasicField<TBasic, TOptions...>::Type;

}  // namespace details

}  // namespace field

}  // namespace comms


