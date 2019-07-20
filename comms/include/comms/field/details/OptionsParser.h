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

#include <tuple>
#include <ratio>
#include "comms/options.h"
#include "comms/CompileControl.h"

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
    static const bool HasCustomValueReader = false;
    static const bool HasCustomRead = false;
    static const bool HasSerOffset = false;
    static const bool HasFixedLengthLimit = false;
    static const bool HasFixedBitLengthLimit = false;
    static const bool HasVarLengthLimits = false;
    static const bool HasSequenceElemLengthForcing = false;
    static const bool HasSequenceSizeForcing = false;
    static const bool HasSequenceLengthForcing = false;
    static const bool HasSequenceFixedSize = false;
    static const bool HasSequenceFixedSizeUseFixedSizeStorage = false;
    static const bool HasSequenceSizeFieldPrefix = false;
    static const bool HasSequenceSerLengthFieldPrefix = false;
    static const bool HasSequenceElemSerLengthFieldPrefix = false;
    static const bool HasSequenceElemFixedSerLengthFieldPrefix = false;
    static const bool HasSequenceTrailingFieldSuffix = false;
    static const bool HasSequenceTerminationFieldSuffix = false;
    static const bool HasDefaultValueInitialiser = false;
    static const bool HasCustomValidator = false;
    static const bool HasContentsRefresher = false;
    static const bool HasCustomRefresh = false;
    static const bool HasFailOnInvalid = false;
    static const bool HasIgnoreInvalid = false;
    static const bool HasInvalidByDefault = false;
    static const bool HasFixedSizeStorage = false;
    static const bool HasCustomStorageType = false;
    static const bool HasScalingRatio = false;
    static const bool HasUnits = false;
    static const bool HasOrigDataView = false;
    static const bool HasEmptySerialization = false;
    static const bool HasMultiRangeValidation = false;
    static const bool HasCustomVersionUpdate = false;
    static const bool HasVersionsRange = false;
    static const bool HasVersionStorage = false;
    static const bool HasRemLengthMemberField = false;
};

template <typename T, typename... TOptions>
class OptionsParser<
    comms::option::def::CustomValueReader<T>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasCustomValueReader = true;
    using CustomValueReader = T;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::HasCustomRead,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasCustomRead = true;
};

template <std::intmax_t TOffset, typename... TOptions>
class OptionsParser<
    comms::option::def::NumValueSerOffset<TOffset>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasSerOffset = true;
    static const auto SerOffset = TOffset;
};

template <std::size_t TLen, bool TSignExtend, typename... TOptions>
class OptionsParser<
    comms::option::def::FixedLength<TLen, TSignExtend>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasFixedLengthLimit = true;
    static const std::size_t FixedLength = TLen;
    static const bool FixedLengthSignExtend = TSignExtend;
};

template <std::size_t TLen, typename... TOptions>
class OptionsParser<
    comms::option::def::FixedBitLength<TLen>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasFixedBitLengthLimit = true;
    static const std::size_t FixedBitLength = TLen;
};

template <std::size_t TMinLen, std::size_t TMaxLen, typename... TOptions>
class OptionsParser<
    comms::option::def::VarLength<TMinLen, TMaxLen>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasVarLengthLimits = true;
    static const std::size_t MinVarLength = TMinLen;
    static const std::size_t MaxVarLength = TMaxLen;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceSizeForcingEnabled,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasSequenceSizeForcing = true;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceLengthForcingEnabled,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasSequenceLengthForcing = true;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceElemLengthForcingEnabled,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasSequenceElemLengthForcing = true;
};

template <std::size_t TSize, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceFixedSize<TSize>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasSequenceFixedSize = true;
    static const auto SequenceFixedSize = TSize;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::app::SequenceFixedSizeUseFixedSizeStorage,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasSequenceFixedSizeUseFixedSizeStorage = true;
};

template <typename TSizeField, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceSizeFieldPrefix<TSizeField>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasSequenceSizeFieldPrefix = true;
    using SequenceSizeFieldPrefix = TSizeField;
};

template <typename TField, comms::ErrorStatus TReadErrorStatus, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceSerLengthFieldPrefix<TField, TReadErrorStatus>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasSequenceSerLengthFieldPrefix = true;
    using SequenceSerLengthFieldPrefix = TField;
    static const comms::ErrorStatus SequenceSerLengthFieldReadErrorStatus = TReadErrorStatus;
};

template <typename TField, comms::ErrorStatus TReadErrorStatus, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceElemSerLengthFieldPrefix<TField, TReadErrorStatus>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasSequenceElemSerLengthFieldPrefix = true;
    using SequenceElemSerLengthFieldPrefix = TField;
    static const comms::ErrorStatus SequenceElemSerLengthFieldReadErrorStatus = TReadErrorStatus;
};

template <typename TField, comms::ErrorStatus TReadErrorStatus, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceElemFixedSerLengthFieldPrefix<TField, TReadErrorStatus>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasSequenceElemFixedSerLengthFieldPrefix = true;
    using SequenceElemFixedSerLengthFieldPrefix = TField;
    static const comms::ErrorStatus SequenceElemFixedSerLengthFieldReadErrorStatus = TReadErrorStatus;
};

template <typename TTrailField, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceTrailingFieldSuffix<TTrailField>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasSequenceTrailingFieldSuffix = true;
    using SequenceTrailingFieldSuffix = TTrailField;
};

template <typename TTermField, typename... TOptions>
class OptionsParser<
    comms::option::def::SequenceTerminationFieldSuffix<TTermField>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasSequenceTerminationFieldSuffix = true;
    using SequenceTerminationFieldSuffix = TTermField;
};

template <typename TInitialiser, typename... TOptions>
class OptionsParser<
    comms::option::def::DefaultValueInitialiser<TInitialiser>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasDefaultValueInitialiser = true;
    using DefaultValueInitialiser = TInitialiser;
};

template <typename TValidator, typename... TOptions>
class OptionsParser<
    comms::option::def::ContentsValidator<TValidator>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasCustomValidator = true;
    using CustomValidator = TValidator;
};

template <typename TRefresher, typename... TOptions>
class OptionsParser<
    comms::option::def::ContentsRefresher<TRefresher>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasContentsRefresher = true;
    using CustomRefresher = TRefresher;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::HasCustomRefresh,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasCustomRefresh = true;
};

template <comms::ErrorStatus TStatus, typename... TOptions>
class OptionsParser<
    comms::option::def::FailOnInvalid<TStatus>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasFailOnInvalid = true;
    static const comms::ErrorStatus FailOnInvalidStatus = TStatus;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::IgnoreInvalid,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasIgnoreInvalid = true;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::InvalidByDefault,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasInvalidByDefault = true;
};

template <std::size_t TSize, typename... TOptions>
class OptionsParser<
    comms::option::app::FixedSizeStorage<TSize>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasFixedSizeStorage = true;
    static const std::size_t FixedSizeStorage = TSize;
};

template <typename TType, typename... TOptions>
class OptionsParser<
    comms::option::app::CustomStorageType<TType>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasCustomStorageType = true;
    using CustomStorageType = TType;
};

template <std::intmax_t TNum, std::intmax_t TDenom, typename... TOptions>
class OptionsParser<
    comms::option::def::ScalingRatio<TNum, TDenom>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasScalingRatio = true;
    using ScalingRatio = std::ratio<TNum, TDenom>;
};

template <typename TType, typename TRatio, typename... TOptions>
class OptionsParser<
    comms::option::def::Units<TType, TRatio>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasUnits = true;
    using UnitsType = TType;
    using UnitsRatio = TRatio;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::app::OrigDataView,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasOrigDataView = true;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::EmptySerialization,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasEmptySerialization = true;
};

template <bool THasMultiRangeValidation>
struct MultiRangeAssembler;

template <>
struct MultiRangeAssembler<false>
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
    static const bool HasMultiRangeValidation = true;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::ValidRangesClear,
    TOptions...> : public OptionsParser<TOptions...>
{
    using BaseImpl = OptionsParser<TOptions...>;
public:
    using MultiRangeValidationRanges = void;
    static const bool HasMultiRangeValidation = false;
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
    static const bool HasMultiRangeValidation = true;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::HasCustomVersionUpdate,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasCustomVersionUpdate = true;
};

template <std::uintmax_t TFrom, std::uintmax_t TUntil, typename... TOptions>
class OptionsParser<
    comms::option::def::ExistsBetweenVersions<TFrom, TUntil>,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasVersionsRange = true;
    static const std::uintmax_t ExistsFromVersion = TFrom;
    static const std::uintmax_t ExistsUntilVersion = TUntil;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::def::VersionStorage,
    TOptions...> : public OptionsParser<TOptions...>
{
public:
    static const bool HasVersionStorage = true;
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
    static const bool HasRemLengthMemberField = true;
    static const std::size_t RemLengthMemberFieldIdx = TIdx;
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


