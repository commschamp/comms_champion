//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <tuple>
#include <ratio>
#include "comms/options.h"
#include "comms/CompileControl.h"
#include "adapters.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename... TOptions>
class OptionsParser;

template <>
class OptionsParser<>
{
public:
    static constexpr bool HasInvalidByDefault = false;
    static constexpr bool HasVersionStorage = false;
    static constexpr bool HasCustomValueReader = false;
    static constexpr bool HasSerOffset = false;
    static constexpr bool HasVersionsRange = false;
    static constexpr bool HasFixedLengthLimit = false;
    static constexpr bool HasFixedBitLengthLimit = false;
    static constexpr bool HasVarLengthLimits = false;
    static constexpr bool HasSequenceElemLengthForcing = false;
    static constexpr bool HasSequenceElemSerLengthFieldPrefix = false;
    static constexpr bool HasSequenceElemFixedSerLengthFieldPrefix = false;
    static constexpr bool HasSequenceSizeForcing = false;
    static constexpr bool HasSequenceLengthForcing = false;
    static constexpr bool HasSequenceFixedSize = false;
    static constexpr bool HasSequenceSizeFieldPrefix = false;
    static constexpr bool HasSequenceSerLengthFieldPrefix = false;
    static constexpr bool HasSequenceTrailingFieldSuffix = false;
    static constexpr bool HasSequenceTerminationFieldSuffix = false;
    static constexpr bool HasRemLengthMemberField = false;
    static constexpr bool HasDefaultValueInitialiser = false;
    static constexpr bool HasMultiRangeValidation = false;
    static constexpr bool HasCustomValidator = false;
    static constexpr bool HasContentsRefresher = false;
    static constexpr bool HasFailOnInvalid = false;
    static constexpr bool HasIgnoreInvalid = false;
    static constexpr bool HasEmptySerialization = false;
    static constexpr bool HasCustomRead = false;
    static constexpr bool HasCustomRefresh = false;
    static constexpr bool HasCustomWrite = false;
    static constexpr bool HasSequenceFixedSizeUseFixedSizeStorage = false;
    static constexpr bool HasFixedSizeStorage = false;
    static constexpr bool HasCustomStorageType = false;
    static constexpr bool HasScalingRatio = false;
    static constexpr bool HasUnits = false;
    static constexpr bool HasOrigDataView = false;
    static constexpr bool HasCustomVersionUpdate = false;

    using UnitsType = void;
    using ScalingRatio = std::ratio<1, 1>;
    using UnitsRatio = std::ratio<1, 1>;

    template <typename TField>
    using AdaptInvalidByDefault = TField;

    template <typename TField>
    using AdaptVersionStorage = TField;

    template <typename TField>
    using AdaptCustomValueReader = TField;

    template <typename TField>
    using AdaptSerOffset = TField;

    template <typename TField>
    using AdaptVersionsRange = TField;

    template <typename TField>
    using AdaptFixedLengthLimit = TField;

    template <typename TField>
    using AdaptFixedBitLengthLimit = TField;

    template <typename TField>
    using AdaptVarLengthLimits = TField;

    template <typename TField>
    using AdaptSequenceElemLengthForcing = TField;

    template <typename TField>
    using AdaptSequenceElemSerLengthFieldPrefix = TField;

    template <typename TField>
    using AdaptSequenceElemFixedSerLengthFieldPrefix = TField;

    template <typename TField>
    using AdaptSequenceSizeForcing = TField;

    template <typename TField>
    using AdaptSequenceLengthForcing = TField;

    template <typename TField>
    using AdaptSequenceFixedSize = TField;

    template <typename TField>
    using AdaptSequenceSizeFieldPrefix = TField;

    template <typename TField>
    using AdaptSequenceSerLengthFieldPrefix = TField;

    template <typename TField>
    using AdaptSequenceTrailingFieldSuffix = TField;

    template <typename TField>
    using AdaptSequenceTerminationFieldSuffix = TField;

    template <typename TField>
    using AdaptRemLengthMemberField = TField;

    template <typename TField>
    using AdaptDefaultValueInitialiser = TField;

    template <typename TField>
    using AdaptMultiRangeValidation = TField;

    template <typename TField>
    using AdaptCustomValidator = TField;

    template <typename TField>
    using AdaptContentsRefresher = TField;
    
    template <typename TField>
    using AdaptFailOnInvalid = TField;

    template <typename TField>
    using AdaptIgnoreInvalid = TField;

    template <typename TField>
    using AdaptEmptySerialization = TField;

    template <typename TField>
    using AdaptCustomRead = TField;

    template <typename TField>
    using AdaptCustomRefresh = TField;

    template <typename TField>
    using AdaptCustomWrite = TField;
};

template <typename T, typename... TOptions>
class OptionsParser<
    comms::option::def::CustomValueReader<T>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasCustomValueReader = true;
    using CustomValueReader = T;

    template <typename TField>
    using AdaptCustomValueReader = comms::field::adapter::CustomValueReader<CustomValueReader, TField>;    
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::HasCustomRead,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasCustomRead = true;

    template <typename TField>
    using AdaptCustomRead = comms::field::adapter::CustomReadWrap<TField>;
};

template <std::intmax_t TOffset, typename... TOptions>
class OptionsParser<
    comms::option::def::NumValueSerOffset<TOffset>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasSerOffset = true;
    static const std::intmax_t SerOffset = TOffset;

    template <typename TField>
    using AdaptSerOffset = comms::field::adapter::SerOffset<SerOffset, TField>;
};

template <std::size_t TLen, bool TSignExtend, typename... TOptions>
class OptionsParser<
    comms::option::def::FixedLength<TLen, TSignExtend>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasFixedLengthLimit = true;
    static constexpr std::size_t FixedLength = TLen;
    static constexpr bool FixedLengthSignExtend = TSignExtend;

    template <typename TField>
    using AdaptFixedLengthLimit = 
        comms::field::adapter::FixedLength<
            FixedLength,
            FixedLengthSignExtend,
            TField
        >;            
};

template <std::size_t TLen, typename... TOptions>
class OptionsParser<
    comms::option::def::FixedBitLength<TLen>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasFixedBitLengthLimit = true;
    static constexpr std::size_t FixedBitLength = TLen;

    template <typename TField>
    using AdaptFixedBitLengthLimit = comms::field::adapter::FixedBitLength<FixedBitLength, TField>;
};

template <std::size_t TMinLen, std::size_t TMaxLen, typename... TOptions>
class OptionsParser<
    comms::option::def::VarLength<TMinLen, TMaxLen>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasVarLengthLimits = true;
    static constexpr std::size_t MinVarLength = TMinLen;
    static constexpr std::size_t MaxVarLength = TMaxLen;

    template <typename TField>
    using AdaptVarLengthLimits = 
        comms::field::adapter::VarLength<MinVarLength, MaxVarLength, TField>;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceSizeForcingEnabled,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasSequenceSizeForcing = true;

    template <typename TField>
    using AdaptSequenceSizeForcing = comms::field::adapter::SequenceSizeForcing<TField>;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceLengthForcingEnabled,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasSequenceLengthForcing = true;

    template <typename TField>
    using AdaptSequenceLengthForcing = comms::field::adapter::SequenceLengthForcing<TField>;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceElemLengthForcingEnabled,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasSequenceElemLengthForcing = true;

    template <typename TField>
    using AdaptSequenceElemLengthForcing = comms::field::adapter::SequenceElemLengthForcing<TField>;
};

template <std::size_t TSize, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceFixedSize<TSize>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasSequenceFixedSize = true;
    static constexpr std::size_t SequenceFixedSize = TSize;

    template <typename TField>
    using AdaptSequenceFixedSize = comms::field::adapter::SequenceFixedSize<SequenceFixedSize, TField>;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::app::SequenceFixedSizeUseFixedSizeStorage,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasSequenceFixedSizeUseFixedSizeStorage = true;
};

template <typename TSizeField, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceSizeFieldPrefix<TSizeField>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasSequenceSizeFieldPrefix = true;
    using SequenceSizeFieldPrefix = TSizeField;

    template <typename TField>
    using AdaptSequenceSizeFieldPrefix = comms::field::adapter::SequenceSizeFieldPrefix<SequenceSizeFieldPrefix, TField>;
};

template <typename TField, comms::ErrorStatus TReadErrorStatus, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceSerLengthFieldPrefix<TField, TReadErrorStatus>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasSequenceSerLengthFieldPrefix = true;
    using SequenceSerLengthFieldPrefix = TField;
    static constexpr comms::ErrorStatus SequenceSerLengthFieldReadErrorStatus = TReadErrorStatus;

    template <typename TWrappedField>
    using AdaptSequenceSerLengthFieldPrefix = 
        comms::field::adapter::SequenceSerLengthFieldPrefix<
            SequenceSerLengthFieldPrefix,
            SequenceSerLengthFieldReadErrorStatus,
            TWrappedField
        >;    
};

template <typename TField, comms::ErrorStatus TReadErrorStatus, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceElemSerLengthFieldPrefix<TField, TReadErrorStatus>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasSequenceElemSerLengthFieldPrefix = true;
    using SequenceElemSerLengthFieldPrefix = TField;
    static constexpr comms::ErrorStatus SequenceElemSerLengthFieldReadErrorStatus = TReadErrorStatus;

    template <typename TWrappedField>
    using AdaptSequenceElemSerLengthFieldPrefix =
        comms::field::adapter::SequenceElemSerLengthFieldPrefix<
            SequenceElemSerLengthFieldPrefix,
            SequenceElemSerLengthFieldReadErrorStatus,
            TWrappedField
        >;
};

template <typename TField, comms::ErrorStatus TReadErrorStatus, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceElemFixedSerLengthFieldPrefix<TField, TReadErrorStatus>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasSequenceElemFixedSerLengthFieldPrefix = true;
    using SequenceElemFixedSerLengthFieldPrefix = TField;
    static constexpr comms::ErrorStatus SequenceElemFixedSerLengthFieldReadErrorStatus = TReadErrorStatus;

    template <typename TWrappedField>
    using AdaptSequenceElemFixedSerLengthFieldPrefix = 
        comms::field::adapter::SequenceElemFixedSerLengthFieldPrefix<
            SequenceElemFixedSerLengthFieldPrefix,
            SequenceElemFixedSerLengthFieldReadErrorStatus,
            TWrappedField
        >;    
};

template <typename TTrailField, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceTrailingFieldSuffix<TTrailField>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasSequenceTrailingFieldSuffix = true;
    using SequenceTrailingFieldSuffix = TTrailField;

    template <typename TField>
    using AdaptSequenceTrailingFieldSuffix = comms::field::adapter::SequenceTrailingFieldSuffix<SequenceTrailingFieldSuffix, TField>;
};

template <typename TTermField, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceTerminationFieldSuffix<TTermField>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasSequenceTerminationFieldSuffix = true;
    using SequenceTerminationFieldSuffix = TTermField;

    template <typename TField>
    using AdaptSequenceTerminationFieldSuffix = 
        comms::field::adapter::SequenceTerminationFieldSuffix<SequenceTerminationFieldSuffix, TField>;
};

template <typename TInitialiser, typename... TOptions>
class OptionsParser<
    comms::option::def::DefaultValueInitialiser<TInitialiser>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasDefaultValueInitialiser = true;
    using DefaultValueInitialiser = TInitialiser;

    template <typename TField>
    using AdaptDefaultValueInitialiser = 
        comms::field::adapter::DefaultValueInitialiser<DefaultValueInitialiser, TField>;
};

template <typename TValidator, typename... TOptions>
class OptionsParser<
    comms::option::def::ContentsValidator<TValidator>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasCustomValidator = true;
    using CustomValidator = TValidator;

    template <typename TField>
    using AdaptCustomValidator = comms::field::adapter::CustomValidator<CustomValidator, TField>;
};

template <typename TRefresher, typename... TOptions>
class OptionsParser<
    comms::option::def::ContentsRefresher<TRefresher>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasContentsRefresher = true;
    using CustomRefresher = TRefresher;

    template <typename TField>
    using AdaptContentsRefresher = comms::field::adapter::CustomRefresher<CustomRefresher, TField>;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::HasCustomRefresh,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasCustomRefresh = true;

    template <typename TField>
    using AdaptCustomRefresh = comms::field::adapter::CustomRefreshWrap<TField>;    
};

template <comms::ErrorStatus TStatus, typename... TOptions>
class OptionsParser<
    comms::option::def::FailOnInvalid<TStatus>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasFailOnInvalid = true;
    static constexpr comms::ErrorStatus FailOnInvalidStatus = TStatus;

    template <typename TField>
    using AdaptFailOnInvalid = comms::field::adapter::FailOnInvalid<FailOnInvalidStatus, TField>;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::IgnoreInvalid,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasIgnoreInvalid = true;

    template <typename TField>
    using AdaptIgnoreInvalid = comms::field::adapter::IgnoreInvalid<TField>;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::InvalidByDefault,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasInvalidByDefault = true;

    template <typename TField>
    using AdaptInvalidByDefault = comms::field::adapter::InvalidByDefault<TField>;
};

template <std::size_t TSize, typename... TOptions>
class OptionsParser<
    comms::option::app::FixedSizeStorage<TSize>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasFixedSizeStorage = true;
    static constexpr std::size_t FixedSizeStorage = TSize;
};

template <typename TType, typename... TOptions>
class OptionsParser<
    comms::option::app::CustomStorageType<TType>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasCustomStorageType = true;
    using CustomStorageType = TType;
};

template <std::intmax_t TNum, std::intmax_t TDenom, typename... TOptions>
class OptionsParser<
    comms::option::def::ScalingRatio<TNum, TDenom>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasScalingRatio = true;
    using ScalingRatio = std::ratio<TNum, TDenom>;
};

template <typename TType, typename TRatio, typename... TOptions>
class OptionsParser<
    comms::option::def::Units<TType, TRatio>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasUnits = true;
    using UnitsType = TType;
    using UnitsRatio = TRatio;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::app::OrigDataView,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasOrigDataView = true;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::EmptySerialization,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasEmptySerialization = true;

    template <typename TField>
    using AdaptEmptySerialization = comms::field::adapter::EmptySerialization<TField>;
};

template <bool THasMultiRangeValidation>
struct MultiRangeAssembler
{
    template <typename TBase, typename T, T TMinValue, T TMaxValue>
    using Type =
        std::tuple<
            std::tuple<
                std::integral_constant<T, TMinValue>,
                std::integral_constant<T, TMaxValue>
            >
        >;
};

template <>
struct MultiRangeAssembler<true>
{
    using FalseAssembler = MultiRangeAssembler<false>;

    template <typename TBase, typename T, T TMinValue, T TMaxValue>
    using Type =
        typename std::decay<
            decltype(
                std::tuple_cat(
                    std::declval<typename TBase::MultiRangeValidationRanges>(),
                    std::declval<typename FalseAssembler::template Type<TBase, T, TMinValue, TMaxValue> >()
                )
            )
        >::type;
};


template <typename TBase, typename T, T TMinValue, T TMaxValue>
using MultiRangeAssemblerT =
    typename MultiRangeAssembler<TBase::HasMultiRangeValidation>::template Type<TBase, T, TMinValue, TMaxValue>;

template <std::intmax_t TMinValue, std::intmax_t TMaxValue, typename... TOptions>
class OptionsParser<
    comms::option::def::ValidNumValueRange<TMinValue, TMaxValue>,
    TOptions...> : public OptionsParser<TOptions...>
{
    using BaseImpl = OptionsParser<TOptions...>;
public:
#ifdef CC_COMPILER_GCC47
    static_assert(!BaseImpl::HasMultiRangeValidation,
        "Sorry gcc-4.7 fails to compile valid C++11 code that allows multiple usage"
        "of comms::option::def::ValidNumValueRange options. Either use it only once or"
        "upgrade your compiler.");
#endif
    using MultiRangeValidationRanges = MultiRangeAssemblerT<BaseImpl, std::intmax_t, TMinValue, TMaxValue>;
    static constexpr bool HasMultiRangeValidation = true;

    template <typename TField>
    using AdaptMultiRangeValidation = 
        comms::field::adapter::NumValueMultiRangeValidator<MultiRangeValidationRanges, TField>;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::ValidRangesClear,
    TOptions...> : public OptionsParser<TOptions...>
{
    using BaseImpl = OptionsParser<TOptions...>;
public:
    using MultiRangeValidationRanges = void;
    static constexpr bool HasMultiRangeValidation = false;

    template <typename TField>
    using AdaptMultiRangeValidation = TField;
};


template <std::uintmax_t TMinValue, std::uintmax_t TMaxValue, typename... TOptions>
class OptionsParser<
    comms::option::def::ValidBigUnsignedNumValueRange<TMinValue, TMaxValue>,
    TOptions...> : public OptionsParser<TOptions...>
{
    using BaseImpl = OptionsParser<TOptions...>;
public:
#ifdef CC_COMPILER_GCC47
    static_assert(!BaseImpl::HasMultiRangeValidation,
        "Sorry gcc-4.7 fails to compile valid C++11 code that allows multiple usage"
        "of comms::option::def::ValidNumValueRange options. Either use it only once or"
        "upgrade your compiler.");
#endif
    using MultiRangeValidationRanges = MultiRangeAssemblerT<BaseImpl, std::uintmax_t, TMinValue, TMaxValue>;
    static constexpr bool HasMultiRangeValidation = true;

    template <typename TField>
    using AdaptMultiRangeValidation = 
        comms::field::adapter::NumValueMultiRangeValidator<MultiRangeValidationRanges, TField>;    
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::HasCustomVersionUpdate,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasCustomVersionUpdate = true;
};

template <std::uintmax_t TFrom, std::uintmax_t TUntil, typename... TOptions>
class OptionsParser<
    comms::option::def::ExistsBetweenVersions<TFrom, TUntil>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasVersionsRange = true;
    static constexpr std::uintmax_t ExistsFromVersion = TFrom;
    static constexpr std::uintmax_t ExistsUntilVersion = TUntil;

    template <typename TField>
    using AdaptVersionsRange = 
        comms::field::adapter::ExistsBetweenVersions<ExistsFromVersion, ExistsUntilVersion, TField>;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::VersionStorage,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasVersionStorage = true;

    template <typename TField>
    using AdaptVersionStorage = comms::field::adapter::VersionStorage<TField>;    
};

template <std::size_t TIdx, typename... TOptions>
class OptionsParser<
    comms::option::def::RemLengthMemberField<TIdx>,
    TOptions...> : public OptionsParser<TOptions...>
{
    using BaseImpl = OptionsParser<TOptions...>;
    static_assert(!BaseImpl::HasRemLengthMemberField, 
        "Option comms::def::option::RemLengthMemberField used multiple times");
public:
    static constexpr bool HasRemLengthMemberField = true;
    static constexpr std::size_t RemLengthMemberFieldIdx = TIdx;

    template <typename TField>
    using AdaptRemLengthMemberField = 
        comms::field::adapter::RemLengthMemberField<RemLengthMemberFieldIdx, TField>;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::HasCustomWrite,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static constexpr bool HasCustomWrite = true;

    template <typename TField>
    using AdaptCustomWrite = comms::field::adapter::CustomWriteWrap<TField>;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public OptionsParser<TOptions...>
{
};

template <typename... TTupleOptions, typename... TOptions>
class OptionsParser<
    std::tuple<TTupleOptions...>,
    TOptions...> : public OptionsParser<TTupleOptions..., TOptions...>
{
};

}  // namespace details

}  // namespace field

}  // namespace comms


