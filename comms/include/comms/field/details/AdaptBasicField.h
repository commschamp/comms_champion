//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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

template<
    bool T1 = false,
    bool T2 = false,
    bool T3 = false,
    bool T4 = false,
    bool T5 = false,
    bool T6 = false>
struct FieldsOptionsCompatibilityCalc
{
    static const std::size_t Value =
        static_cast<std::size_t>(T1) +\
        static_cast<std::size_t>(T2) +
        static_cast<std::size_t>(T3) +
        static_cast<std::size_t>(T4) +
        static_cast<std::size_t>(T5) +
        static_cast<std::size_t>(T6);
};

template <bool THasVersionStorage>
struct AdaptFieldVersionStorage;

template <>
struct AdaptFieldVersionStorage<true>
{
    template <typename TField>
    using Type = comms::field::adapter::VersionStorage<TField>;
};

template <>
struct AdaptFieldVersionStorage<false>
{
    template <typename TField>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldVersionStorageT =
    typename AdaptFieldVersionStorage<TOpts::HasVersionStorage>::template Type<TField>;

template <bool THasInvalidByDefault>
struct AdaptFieldInvalidByDefault;

template <>
struct AdaptFieldInvalidByDefault<true>
{
    template <typename TField>
    using Type = comms::field::adapter::InvalidByDefault<TField>;
};

template <>
struct AdaptFieldInvalidByDefault<false>
{
    template <typename TField>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldInvalidByDefaultT =
    typename AdaptFieldInvalidByDefault<TOpts::HasInvalidByDefault>::template Type<TField>;


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

template <bool THasVersionsRange>
struct AdaptFieldVersionsRange;

template <>
struct AdaptFieldVersionsRange<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::ExistsBetweenVersions<TOpts::ExistsFromVersion, TOpts::ExistsUntilVersion, TField>;
};

template <>
struct AdaptFieldVersionsRange<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldVersionsRangeT =
    typename AdaptFieldVersionsRange<TOpts::HasVersionsRange>::template Type<TField, TOpts>;


template <bool THasFixedLength>
struct AdaptFieldFixedLength;

template <>
struct AdaptFieldFixedLength<true>
{
    template <typename TField, typename TOpts>
    using Type =
        comms::field::adapter::FixedLength<
            TOpts::FixedLength,
            TOpts::FixedLengthSignExtend,
            TField
        >;
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

template <bool THasSequenceElemLengthForcing>
struct AdaptFieldSequenceElemLengthForcing;

template <>
struct AdaptFieldSequenceElemLengthForcing<true>
{
    template <typename TField>
    using Type = comms::field::adapter::SequenceElemLengthForcing<TField>;
};

template <>
struct AdaptFieldSequenceElemLengthForcing<false>
{
    template <typename TField>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldSequenceElemLengthForcingT =
    typename AdaptFieldSequenceElemLengthForcing<TOpts::HasSequenceElemLengthForcing>::template Type<TField>;


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

template <bool THasSequenceLengthForcing>
struct AdaptFieldSequenceLengthForcing;

template <>
struct AdaptFieldSequenceLengthForcing<true>
{
    template <typename TField>
    using Type = comms::field::adapter::SequenceLengthForcing<TField>;
};

template <>
struct AdaptFieldSequenceLengthForcing<false>
{
    template <typename TField>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldSequenceLengthForcingT =
    typename AdaptFieldSequenceLengthForcing<TOpts::HasSequenceLengthForcing>::template Type<TField>;


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

template <bool THasSequenceElemSerLengthFieldPrefix>
struct AdaptFieldSequenceElemSerLengthFieldPrefix;

template <>
struct AdaptFieldSequenceElemSerLengthFieldPrefix<true>
{
    template <typename TField, typename TOpts>
    using Type =
        comms::field::adapter::SequenceElemSerLengthFieldPrefix<
            typename TOpts::SequenceElemSerLengthFieldPrefix,
            TOpts::SequenceElemSerLengthFieldReadErrorStatus,
            TField
        >;
};

template <>
struct AdaptFieldSequenceElemSerLengthFieldPrefix<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldSequenceElemSerLengthFieldPrefixT =
    typename AdaptFieldSequenceElemSerLengthFieldPrefix<TOpts::HasSequenceElemSerLengthFieldPrefix>::template Type<TField, TOpts>;

//--

template <bool THasSequenceElemFixedSerLengthFieldPrefix>
struct AdaptFieldSequenceElemFixedSerLengthFieldPrefix;

template <>
struct AdaptFieldSequenceElemFixedSerLengthFieldPrefix<true>
{
    template <typename TField, typename TOpts>
    using Type =
        comms::field::adapter::SequenceElemFixedSerLengthFieldPrefix<
            typename TOpts::SequenceElemFixedSerLengthFieldPrefix,
            TOpts::SequenceElemFixedSerLengthFieldReadErrorStatus,
            TField
        >;
};

template <>
struct AdaptFieldSequenceElemFixedSerLengthFieldPrefix<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldSequenceElemFixedSerLengthFieldPrefixT =
    typename AdaptFieldSequenceElemFixedSerLengthFieldPrefix<TOpts::HasSequenceElemFixedSerLengthFieldPrefix>::template Type<TField, TOpts>;

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
    typename AdaptFieldSequenceTerminationFieldSuffix<TOpts::HasSequenceTerminationFieldSuffix>::
        template Type<TField, TOpts>;

template <bool THasRemLengthMemberField>
struct AdaptFieldRemLengthMemberField;

template <>
struct AdaptFieldRemLengthMemberField<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::RemLengthMemberField<TOpts::RemLengthMemberFieldIdx, TField>;
};

template <>
struct AdaptFieldRemLengthMemberField<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};        

template <typename TField, typename TOpts>
using AdaptFieldRemLengthMemberFieldT = 
    typename AdaptFieldRemLengthMemberField<TOpts::HasRemLengthMemberField>::
        template Type<TField, TOpts>;

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

template <bool THasMultiRangeValidation>
struct AdaptFieldNumValueMultiRangeValidator;

template <>
struct AdaptFieldNumValueMultiRangeValidator<true>
{
    template <typename TField, typename TOpts>
    using Type = comms::field::adapter::NumValueMultiRangeValidator<typename TOpts::MultiRangeValidationRanges, TField>;
};

template <>
struct AdaptFieldNumValueMultiRangeValidator<false>
{
    template <typename TField, typename TOpts>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldNumValueMultiRangeValidatorT =
    typename AdaptFieldNumValueMultiRangeValidator<TOpts::HasMultiRangeValidation>::template Type<TField, TOpts>;


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

template <bool THasCustomRefresh>
struct AdaptFieldCustomRefreshWrap;

template <>
struct AdaptFieldCustomRefreshWrap<true>
{
    template <typename TField>
    using Type = comms::field::adapter::CustomRefreshWrap<TField>;
};

template <>
struct AdaptFieldCustomRefreshWrap<false>
{
    template <typename TField>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldCustomRefreshWrapT =
    typename AdaptFieldCustomRefreshWrap<TOpts::HasCustomRefresh>::template Type<TField>;

template <bool THasContentsRefresher>
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
    typename AdaptFieldCustomRefresher<TOpts::HasContentsRefresher>::template Type<TField, TOpts>;

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

template <bool THasEmptySerialization>
struct AdaptFieldEmptySerialization;

template <>
struct AdaptFieldEmptySerialization<true>
{
    template <typename TField>
    using Type = comms::field::adapter::EmptySerialization<TField>;
};

template <>
struct AdaptFieldEmptySerialization<false>
{
    template <typename TField>
    using Type = TField;
};

template <typename TField, typename TOpts>
using AdaptFieldEmptySerializationT =
    typename AdaptFieldEmptySerialization<TOpts::HasEmptySerialization>::template Type<TField>;

template <typename TBasic, typename... TOptions>
class AdaptBasicField
{
    using ParsedOptions = OptionsParser<TOptions...>;

    static const bool CustomReaderIncompatible =
            ParsedOptions::HasSerOffset ||
            ParsedOptions::HasFixedLengthLimit ||
            ParsedOptions::HasFixedBitLengthLimit ||
            ParsedOptions::HasVarLengthLimits ||
            ParsedOptions::HasSequenceElemLengthForcing ||
            ParsedOptions::HasSequenceSizeForcing ||
            ParsedOptions::HasSequenceLengthForcing ||
            ParsedOptions::HasSequenceFixedSize ||
            ParsedOptions::HasSequenceSizeFieldPrefix ||
            ParsedOptions::HasSequenceSerLengthFieldPrefix ||
            ParsedOptions::HasSequenceElemSerLengthFieldPrefix ||
            ParsedOptions::HasSequenceElemFixedSerLengthFieldPrefix ||
            ParsedOptions::HasSequenceTrailingFieldSuffix ||
            ParsedOptions::HasSequenceTerminationFieldSuffix |\
            ParsedOptions::HasEmptySerialization;

    static_assert(
            (!ParsedOptions::HasCustomValueReader) || (!CustomReaderIncompatible),
            "CustomValueReader option is incompatible with following options: "
            "NumValueSerOffset, FixedLength, FixedBitLength, VarLength, "
            "HasSequenceElemLengthForcing, "
            "SequenceSizeForcingEnabled, SequenceLengthForcingEnabled, SequenceFixedSize, SequenceSizeFieldPrefix, "
            "SequenceSerLengthFieldPrefix, SequenceElemSerLengthFieldPrefix, "
            "SequenceElemFixedSerLengthFieldPrefix, SequenceTrailingFieldSuffix, "
            "SequenceTerminationFieldSuffix, EmptySerialization");

    static const bool VarLengthIncompatible =
            ParsedOptions::HasFixedLengthLimit ||
            ParsedOptions::HasFixedBitLengthLimit;

    static_assert(
            (!ParsedOptions::HasVarLengthLimits) || (!VarLengthIncompatible),
            "VarLength option is incompatible with FixedLength and FixedBitLength");

    static_assert(
            1U >= FieldsOptionsCompatibilityCalc<
                ParsedOptions::HasSequenceSizeFieldPrefix,
                ParsedOptions::HasSequenceSerLengthFieldPrefix,
                ParsedOptions::HasSequenceFixedSize,
                ParsedOptions::HasSequenceSizeForcing,
                ParsedOptions::HasSequenceLengthForcing,
                ParsedOptions::HasSequenceTerminationFieldSuffix>::Value,
            "The following options are incompatible, cannot be used together: "
            "SequenceSizeFieldPrefix, SequenceSerLengthFieldPrefix, "
            "SequenceFixedSize, SequenceSizeForcingEnabled, SequenceLengthForcingEnabled, "
            "SequenceTerminationFieldSuffix");

    static_assert(
            1U >= FieldsOptionsCompatibilityCalc<
                ParsedOptions::HasSequenceElemSerLengthFieldPrefix,
                ParsedOptions::HasSequenceElemFixedSerLengthFieldPrefix,
                ParsedOptions::HasSequenceTerminationFieldSuffix>::Value,
            "The following options are incompatible, cannot be used together: "
            "SequenceElemSerLengthFieldPrefix, SequenceElemFixedSerLengthFieldPrefix "
            "SequenceTerminationFieldSuffix");

    static_assert(
            (!ParsedOptions::HasSequenceTrailingFieldSuffix) ||
            (!ParsedOptions::HasSequenceTerminationFieldSuffix),
            "The following options are incompatible, cannot be used together: "
            "SequenceTrailingFieldSuffix, SequenceTerminationFieldSuffix");

    static_assert(
            (!ParsedOptions::HasFailOnInvalid) ||
            (!ParsedOptions::HasIgnoreInvalid),
            "The following options are incompatible, cannot be used together: "
            "FailOnInvalid, IgnoreInvalid");

    static_assert(
            1U >= FieldsOptionsCompatibilityCalc<
                ParsedOptions::HasCustomValueReader,
                ParsedOptions::HasFixedSizeStorage,
                ParsedOptions::HasOrigDataView>::Value,
            "The following options are incompatible, cannot be used together: "
            "CustomStorageType, FixedSizeStorage, OrigDataView");

    static_assert(
            (!ParsedOptions::HasSequenceFixedSizeUseFixedSizeStorage) ||
            (ParsedOptions::HasSequenceFixedSize),
            "The option SequenceFixedSizeUseFixedSizeStorage cannot be used without SequenceFixedSize.");

    static_assert(
            (!ParsedOptions::HasSequenceFixedSizeUseFixedSizeStorage) ||
            (!ParsedOptions::HasFixedSizeStorage),
            "The following options are incompatible, cannot be used together: "
            "SequenceFixedSizeUseFixedSizeStorage, FixedSizeStorage");

    using InvalidByDefaultAdapted = AdaptFieldInvalidByDefaultT<
        TBasic, ParsedOptions>;
    using VersionStorageAdapted = AdaptFieldVersionStorageT<
        InvalidByDefaultAdapted, ParsedOptions>;
    using CustomReaderAdapted = AdaptFieldCustomValueReaderT<
        VersionStorageAdapted, ParsedOptions>;
    using SerOffsetAdapted = AdaptFieldSerOffsetT<
        CustomReaderAdapted, ParsedOptions>;
    using VersionsRangeAdapted = AdaptFieldVersionsRangeT<
        SerOffsetAdapted, ParsedOptions>;
    using FixedLengthAdapted = AdaptFieldFixedLengthT<
        VersionsRangeAdapted, ParsedOptions>;
    using FixedBitLengthAdapted = AdaptFieldFixedBitLengthT<
        FixedLengthAdapted, ParsedOptions>;
    using VarLengthAdapted = AdaptFieldVarLengthT<
        FixedBitLengthAdapted, ParsedOptions>;
    using SequenceElemLengthForcingAdapted = AdaptFieldSequenceElemLengthForcingT<
        VarLengthAdapted, ParsedOptions>;
    using SequenceElemSerLengthFieldPrefixAdapted = AdaptFieldSequenceElemSerLengthFieldPrefixT<
        SequenceElemLengthForcingAdapted, ParsedOptions>;
    using SequenceElemFixedSerLengthFieldPrefixAdapted = AdaptFieldSequenceElemFixedSerLengthFieldPrefixT<
        SequenceElemSerLengthFieldPrefixAdapted, ParsedOptions>;
    using SequenceSizeForcingAdapted = AdaptFieldSequenceSizeForcingT<
        SequenceElemFixedSerLengthFieldPrefixAdapted, ParsedOptions>;
    using SequenceLengthForcingAdapted = AdaptFieldSequenceLengthForcingT<
        SequenceSizeForcingAdapted, ParsedOptions>;
    using SequenceFixedSizeAdapted = AdaptFieldSequenceFixedSizeT<
        SequenceLengthForcingAdapted, ParsedOptions>;
    using SequenceSizeFieldPrefixAdapted = AdaptFieldSequenceSizeFieldPrefixT<
        SequenceFixedSizeAdapted, ParsedOptions>;
    using SequenceSerLengthFieldPrefixAdapted = AdaptFieldSequenceSerLengthFieldPrefixT<
        SequenceSizeFieldPrefixAdapted, ParsedOptions>;
    using SequenceTrailingFieldSuffixAdapted = AdaptFieldSequenceTrailingFieldSuffixT<
        SequenceSerLengthFieldPrefixAdapted, ParsedOptions>;
    using SequenceTerminationFieldSuffixAdapted = AdaptFieldSequenceTerminationFieldSuffixT<
        SequenceTrailingFieldSuffixAdapted, ParsedOptions>;
     using RemLengthMemberFieldAdapted = AdaptFieldRemLengthMemberFieldT<
        SequenceTerminationFieldSuffixAdapted, ParsedOptions>;
    using DefaultValueInitialiserAdapted = AdaptFieldDefaultValueInitialiserT<
        RemLengthMemberFieldAdapted, ParsedOptions>;
    using NumValueMultiRangeValidatorAdapted = AdaptFieldNumValueMultiRangeValidatorT<
        DefaultValueInitialiserAdapted, ParsedOptions>;
    using CustomValidatorAdapted = AdaptFieldCustomValidatorT<
        NumValueMultiRangeValidatorAdapted, ParsedOptions>;
    using CustomRefreshWrapAdapted = AdaptFieldCustomRefreshWrapT<
        CustomValidatorAdapted, ParsedOptions>;
    using CustomRefresherAdapted = AdaptFieldCustomRefresherT<
        CustomRefreshWrapAdapted, ParsedOptions>;
    using FailOnInvalidAdapted = AdaptFieldFailOnInvalidT<
        CustomRefresherAdapted, ParsedOptions>;
    using IgnoreInvalidAdapted = AdaptFieldIgnoreInvalidT<
        FailOnInvalidAdapted, ParsedOptions>;
    using EmptySerializationAdapted = AdaptFieldEmptySerializationT<
        IgnoreInvalidAdapted, ParsedOptions>;

public:
    using Type = EmptySerializationAdapted;
};

template <typename TBasic, typename... TOptions>
using AdaptBasicFieldT = typename AdaptBasicField<TBasic, TOptions...>::Type;

}  // namespace details

}  // namespace field

}  // namespace comms


