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

#include "adapters.h"
#include "OptionsParser.h"

namespace comms
{

namespace field
{

namespace details
{

template <bool THasCustomValueReader>
struct AdaptFieldCustomValueReader;

template <>
struct AdaptFieldCustomValueReader<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::CustomValueReader<typename TOpts::CustomValueReader, TField>;
};

template <>
struct AdaptFieldCustomValueReader<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldCustomValueReaderT =
    typename AdaptFieldCustomValueReader<TOpts::HasCustomValueReader>::template Type<TField, TOpts>;

template <bool THasSerOffset>
struct AdaptFieldSerOffset;

template <>
struct AdaptFieldSerOffset<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::SerOffset<TOpts::SerOffset, TField>;
};

template <>
struct AdaptFieldSerOffset<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldSerOffsetT =
    typename AdaptFieldSerOffset<TOpts::HasSerOffset>::template Type<TField, TOpts>;


template <bool THasFixedLength>
struct AdaptFieldFixedLength;

template <>
struct AdaptFieldFixedLength<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::FixedLength<TOpts::FixedLength, TField>;
};

template <>
struct AdaptFieldFixedLength<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldFixedLengthT =
    typename AdaptFieldFixedLength<TOpts::HasFixedLengthLimit>::template Type<TField, TOpts>;

template <bool THasFixedBitLength>
struct AdaptFieldFixedBitLength;

template <>
struct AdaptFieldFixedBitLength<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::FixedBitLength<TOpts::FixedBitLength, TField>;
};

template <>
struct AdaptFieldFixedBitLength<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldFixedBitLengthT =
    typename AdaptFieldFixedBitLength<TOpts::HasFixedBitLengthLimit>::template Type<TField, TOpts>;

template <bool THasVarLengths>
struct AdaptFieldVarLength;

template <>
struct AdaptFieldVarLength<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::VarLength<TOpts::MinVarLength, TOpts::MaxVarLength, TField>;
};

template <>
struct AdaptFieldVarLength<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldVarLengthT =
    typename AdaptFieldVarLength<TOpts::HasVarLengthLimits>::template Type<TField, TOpts>;

template <bool THasSequenceSizeForcing>
struct AdaptFieldSequenceSizeForcing;

template <>
struct AdaptFieldSequenceSizeForcing<true>
{
    template <typename TField>
    using Type = comms::field::adapter::SequenceSizeForcing<TField>;
};

template <>
struct AdaptFieldSequenceSizeForcing<false>
{
    template <typename TField>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldSequenceSizeForcingT =
    typename AdaptFieldSequenceSizeForcing<TOpts::HasSequenceSizeForcing>::template Type<TField>;

template <bool THasSequenceFixedSize>
struct AdaptFieldSequenceFixedSize;

template <>
struct AdaptFieldSequenceFixedSize<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::SequenceFixedSize<TOpts::SequenceFixedSize, TField>;
};

template <>
struct AdaptFieldSequenceFixedSize<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldSequenceFixedSizeT =
    typename AdaptFieldSequenceFixedSize<TOpts::HasSequenceFixedSize>::template Type<TField, TOpts>;

template <bool THasSequenceSizeFieldPrefix>
struct AdaptFieldSequenceSizeFieldPrefix;

template <>
struct AdaptFieldSequenceSizeFieldPrefix<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::SequenceSizeFieldPrefix<typename TOpts::SequenceSizeFieldPrefix, TField>;
};

template <>
struct AdaptFieldSequenceSizeFieldPrefix<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldSequenceSizeFieldPrefixT =
    typename AdaptFieldSequenceSizeFieldPrefix<TOpts::HasSequenceSizeFieldPrefix>::template Type<TField, TOpts>;

//--
template <bool THasSequenceSerLengthFieldPrefix>
struct AdaptFieldSequenceSerLengthFieldPrefix;

template <>
struct AdaptFieldSequenceSerLengthFieldPrefix<true>
{
    template <typename TField, typename TOpts>
    using Type =
        comms::field::adapter::SequenceSerLengthFieldPrefix<
            typename TOpts::SequenceSerLengthFieldPrefix,
            TOpts::SequenceSerLengthFieldReadErrorStatus,
            TField
        >;
};

template <>
struct AdaptFieldSequenceSerLengthFieldPrefix<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldSequenceSerLengthFieldPrefixT =
    typename AdaptFieldSequenceSerLengthFieldPrefix<TOpts::HasSequenceSerLengthFieldPrefix>::template Type<TField, TOpts>;

//--

template <bool THasSequenceTrailingFieldSuffix>
struct AdaptFieldSequenceTrailingFieldSuffix;

template <>
struct AdaptFieldSequenceTrailingFieldSuffix<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::SequenceTrailingFieldSuffix<typename TOpts::SequenceTrailingFieldSuffix, TField>;
};

template <>
struct AdaptFieldSequenceTrailingFieldSuffix<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldSequenceTrailingFieldSuffixT =
    typename AdaptFieldSequenceTrailingFieldSuffix<TOpts::HasSequenceTrailingFieldSuffix>::template Type<TField, TOpts>;

template <bool THasSequenceTerminationFieldSuffix>
struct AdaptFieldSequenceTerminationFieldSuffix;

template <>
struct AdaptFieldSequenceTerminationFieldSuffix<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::SequenceTerminationFieldSuffix<typename TOpts::SequenceTerminationFieldSuffix, TField>;
};

template <>
struct AdaptFieldSequenceTerminationFieldSuffix<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldSequenceTerminationFieldSuffixT =
    typename AdaptFieldSequenceTerminationFieldSuffix<TOpts::HasSequenceTerminationFieldSuffix>::template Type<TField, TOpts>;

template <bool THasDefaultValueInitialiser>
struct AdaptFieldDefaultValueInitialiser;

template <>
struct AdaptFieldDefaultValueInitialiser<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::DefaultValueInitialiser<typename TOpts::DefaultValueInitialiser, TField>;
};

template <>
struct AdaptFieldDefaultValueInitialiser<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldDefaultValueInitialiserT =
    typename AdaptFieldDefaultValueInitialiser<TOpts::HasDefaultValueInitialiser>::template Type<TField, TOpts>;

template <bool THasCustomValidator>
struct AdaptFieldCustomValidator;

template <>
struct AdaptFieldCustomValidator<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::CustomValidator<typename TOpts::CustomValidator, TField>;
};

template <>
struct AdaptFieldCustomValidator<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldCustomValidatorT =
    typename AdaptFieldCustomValidator<TOpts::HasCustomValidator>::template Type<TField, TOpts>;

template <bool THasCustomRefresher>
struct AdaptFieldCustomRefresher;

template <>
struct AdaptFieldCustomRefresher<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::CustomRefresher<typename TOpts::CustomRefresher, TField>;
};

template <>
struct AdaptFieldCustomRefresher<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldCustomRefresherT =
    typename AdaptFieldCustomRefresher<TOpts::HasCustomRefresher>::template Type<TField, TOpts>;

template <bool THasFailOnInvalid>
struct AdaptFieldFailOnInvalid;

template <>
struct AdaptFieldFailOnInvalid<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::FailOnInvalid<TOpts::FailOnInvalidStatus, TField>;
};

template <>
struct AdaptFieldFailOnInvalid<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldFailOnInvalidT =
    typename AdaptFieldFailOnInvalid<TOpts::HasFailOnInvalid>::template Type<TField, TOpts>;

template <bool THasIgnoreInvalid>
struct AdaptFieldIgnoreInvalid;

template <>
struct AdaptFieldIgnoreInvalid<true>
{
    template <typename TField>
    using Type = comms::field::adapter::IgnoreInvalid<TField>;
};

template <>
struct AdaptFieldIgnoreInvalid<false>
{
    template <typename TField>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldIgnoreInvalidT =
    typename AdaptFieldIgnoreInvalid<TOpts::HasIgnoreInvalid>::template Type<TField>;

template <typename TBasic, typename... TOptions>
class AdaptBasicField
{
    using ParsedOptions = OptionsParser<TOptions...>;
    using CustomReaderAdapted = AdaptFieldCustomValueReaderT<
        TBasic, ParsedOptions>;
    using SerOffsetAdapted = AdaptFieldSerOffsetT<
        CustomReaderAdapted, ParsedOptions>;
    using FixedLengthAdapted = AdaptFieldFixedLengthT<
        SerOffsetAdapted, ParsedOptions>;
    using FixedBitLengthAdapted = AdaptFieldFixedBitLengthT<
        FixedLengthAdapted, ParsedOptions>;
    using VarLengthAdapted = AdaptFieldVarLengthT<
        FixedBitLengthAdapted, ParsedOptions>;
    using SequenceSizeForcingAdapted = AdaptFieldSequenceSizeForcingT<
        VarLengthAdapted, ParsedOptions>;
    using SequenceFixedSizeAdapted = AdaptFieldSequenceFixedSizeT<
        SequenceSizeForcingAdapted, ParsedOptions>;
    using SequenceSizeFieldPrefixAdapted = AdaptFieldSequenceSizeFieldPrefixT<
        SequenceFixedSizeAdapted, ParsedOptions>;
    using SequenceSerLengthFieldPrefixAdapted = AdaptFieldSequenceSerLengthFieldPrefixT<
        SequenceSizeFieldPrefixAdapted, ParsedOptions>;
    using SequenceTrailingFieldSuffixAdapted = AdaptFieldSequenceTrailingFieldSuffixT<
        SequenceSerLengthFieldPrefixAdapted, ParsedOptions>;
    using SequenceTerminationFieldSuffixAdapted = AdaptFieldSequenceTerminationFieldSuffixT<
        SequenceTrailingFieldSuffixAdapted, ParsedOptions>;
    using DefaultValueInitialiserAdapted = AdaptFieldDefaultValueInitialiserT<
        SequenceTerminationFieldSuffixAdapted, ParsedOptions>;
    using CustomValidatorAdapted = AdaptFieldCustomValidatorT<
        DefaultValueInitialiserAdapted, ParsedOptions>;
    using CustomRefresherAdapted = AdaptFieldCustomRefresherT<
        CustomValidatorAdapted, ParsedOptions>;
    using FailOnInvalidAdapted = AdaptFieldFailOnInvalidT<
        CustomRefresherAdapted, ParsedOptions>;
    using IgnoreInvalidAdapted = AdaptFieldIgnoreInvalidT<
        FailOnInvalidAdapted, ParsedOptions>;
public:
    using Type = IgnoreInvalidAdapted;
};

template <typename TBasic, typename... TOptions>
using AdaptBasicFieldT = typename AdaptBasicField<TBasic, TOptions...>::Type;

}  // namespace details

}  // namespace field

}  // namespace comms


