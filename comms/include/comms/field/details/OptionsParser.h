//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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
#include "comms/options.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename... TOptions>
struct OptionsParser;

template <>
struct OptionsParser<>
{
    static const bool HasCustomValueReader = false;
    static const bool HasSerOffset = false;
    static const bool HasFixedLengthLimit = false;
    static const bool HasFixedBitLengthLimit = false;
    static const bool HasVarLengthLimits = false;
    static const bool HasSequenceSizeForcing = false;
    static const bool HasSequenceFixedSize = false;
    static const bool HasSequenceSizeFieldPrefix = false;
    static const bool HasSequenceTrailingFieldSuffix = false;
    static const bool HasSequenceTerminationFieldSuffix = false;
    static const bool HasDefaultValueInitialiser = false;
    static const bool HasCustomValidator = false;
    static const bool HasFailOnInvalid = false;
    static const bool HasIgnoreInvalid = false;
    static const bool HasFixedSizeStorage = false;
    static const bool HasCustomStorageType = false;
    static const bool HasScalingRatio = false;
};

template <typename T, typename... TOptions>
class OptionsParser<
    comms::option::CustomValueReader<T>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef OptionsParser<TOptions...> Base;
    typedef comms::option::CustomValueReader<T> Option;

    static_assert(!Base::HasSerOffset,
        "Cannot mix NumValueSerOffset and CustomValueReader options.");

    static_assert(!Base::HasFixedLengthLimit,
        "Cannot mix FixedLength and CustomValueReader options.");

    static_assert(!Base::HasFixedBitLengthLimit,
        "Cannot mix FixedBitLength and CustomValueReader options.");

    static_assert(!Base::HasVarLengthLimits,
        "Cannot mix VarLength and CustomValueReader options.");

    static_assert(!Base::HasSequenceSizeForcing,
        "Cannot mix SequenceSizeForcing and CustomValueReader options.");

    static_assert(!Base::HasSequenceFixedSize,
        "Cannot mix SequenceFixedSize and CustomValueReader options.");

    static_assert(!Base::HasSequenceSizeFieldPrefix,
        "Cannot mix SequenceSizeFieldPrefix and CustomValueReader options.");

    static_assert(!Base::HasSequenceTrailingFieldSuffix,
        "Cannot mix SequenceTrailingFieldSuffix and CustomValueReader options.");

    static_assert(!Base::HasSequenceTerminationFieldSuffix,
        "Cannot mix SequenceTerminationFieldSuffix and CustomValueReader options.");
public:
    static const bool HasCustomValueReader = true;
    typedef typename Option::Type CustomValueReader;
};

template <std::intmax_t TOffset, typename... TOptions>
class OptionsParser<
    comms::option::NumValueSerOffset<TOffset>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef OptionsParser<TOptions...> Base;
    typedef comms::option::NumValueSerOffset<TOffset> Option;

    static_assert(!Base::HasCustomValueReader,
        "Cannot mix NumValueSerOffset and CustomValueReader options.");
public:
    static const bool HasSerOffset = true;
    static const auto SerOffset = Option::Value;
};

template <std::size_t TLen, typename... TOptions>
class OptionsParser<
    comms::option::FixedLength<TLen>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef OptionsParser<TOptions...> Base;
    typedef comms::option::FixedLength<TLen> Option;

    static_assert(!Base::HasVarLengthLimits,
        "Cannot mix FixedLength and VarLength options.");

    static_assert(!Base::HasCustomValueReader,
        "Cannot mix FixedLength and CustomValueReader options.");
public:
    static const bool HasFixedLengthLimit = true;
    static const std::size_t FixedLength = Option::Value;
};

template <std::size_t TLen, typename... TOptions>
class OptionsParser<
    comms::option::FixedBitLength<TLen>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef OptionsParser<TOptions...> Base;
    typedef comms::option::FixedBitLength<TLen> Option;

    static_assert(!Base::HasVarLengthLimits,
        "Cannot mix FixedBitLength and VarLength options.");

    static_assert(!Base::HasCustomValueReader,
        "Cannot mix FixedBitLength and CustomValueReader options.");
public:
    static const bool HasFixedBitLengthLimit = true;
    static const std::size_t FixedBitLength = Option::Value;
};


template <std::size_t TMinLen, std::size_t TMaxLen, typename... TOptions>
class OptionsParser<
    comms::option::VarLength<TMinLen, TMaxLen>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef OptionsParser<TOptions...> Base;
    typedef comms::option::VarLength<TMinLen, TMaxLen> Option;

    static_assert(!Base::HasFixedLengthLimit,
        "Cannot mix FixedLength and VarLength options.");
    static_assert(!Base::HasFixedBitLengthLimit,
        "Cannot mix FixedBitLength and VarLength options.");
    static_assert(!Base::HasCustomValueReader,
        "Cannot mix VarLength and CustomValueReader options.");

public:
    static const bool HasVarLengthLimits = true;
    static const std::size_t MinVarLength = Option::MinValue;
    static const std::size_t MaxVarLength = Option::MaxValue;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::SequenceSizeForcingEnabled,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef OptionsParser<TOptions...> Base;
    static_assert(!Base::HasSequenceSizeFieldPrefix,
        "SequenceSizeFieldPrefix and SequenceSizeForcingEnabled are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasSequenceFixedSize,
        "SequenceSizeForcingEnabled and SequenceFixedSize are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasSequenceTerminationFieldSuffix,
        "SequenceSizeForcingEnabled and SequenceTerminationFieldSuffix are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasCustomValueReader,
        "Cannot mix SequenceSizeForcingEnabled and CustomValueReader options.");

public:
    static const bool HasSequenceSizeForcing = true;
};

template <std::size_t TSize, typename... TOptions>
class OptionsParser<
    comms::option::SequenceFixedSize<TSize>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef comms::option::SequenceFixedSize<TSize> Option;
    typedef OptionsParser<TOptions...> Base;
    static_assert(!Base::HasSequenceSizeFieldPrefix,
        "SequenceFixedSize and SequenceSizeFieldPrefix are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasSequenceSizeForcing,
        "SequenceFixedSize and SequenceSizeForcing are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasSequenceTerminationFieldSuffix,
        "SequenceFixedSize and SequenceTerminationFieldSuffix are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasCustomValueReader,
        "Cannot mix SequenceFixedSize and CustomValueReader options.");
public:
    static const bool HasSequenceFixedSize = true;
    static const auto SequenceFixedSize = Option::Value;
};

template <typename TSizeField, typename... TOptions>
class OptionsParser<
    comms::option::SequenceSizeFieldPrefix<TSizeField>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef comms::option::SequenceSizeFieldPrefix<TSizeField> Option;
    typedef OptionsParser<TOptions...> Base;
    static_assert(!Base::HasSequenceSizeForcing,
        "SequenceSizeFieldPrefix and SequenceSizeForcingEnabled are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasSequenceFixedSize,
        "SequenceSizeFieldPrefix and SequenceFixedSize are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasSequenceTerminationFieldSuffix,
        "SequenceSizeFieldPrefix and SequenceTerminationFieldSuffix are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasCustomValueReader,
        "Cannot mix SequenceSizeFieldPrefix and CustomValueReader options.");
public:
    static const bool HasSequenceSizeFieldPrefix = true;
    typedef typename Option::Type SequenceSizeFieldPrefix;
};

template <typename TTrailField, typename... TOptions>
class OptionsParser<
    comms::option::SequenceTrailingFieldSuffix<TTrailField>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef comms::option::SequenceTrailingFieldSuffix<TTrailField> Option;
    typedef OptionsParser<TOptions...> Base;

    static_assert(!Base::HasSequenceTerminationFieldSuffix,
        "SequenceTerminationFieldSuffix and SequenceTrailingFieldSuffix are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasCustomValueReader,
        "Cannot mix SequenceTrailingFieldSuffix and CustomValueReader options.");

public:
    static const bool HasSequenceTrailingFieldSuffix = true;
    typedef typename Option::Type SequenceTrailingFieldSuffix;
};

template <typename TTermField, typename... TOptions>
class OptionsParser<
    comms::option::SequenceTerminationFieldSuffix<TTermField>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef comms::option::SequenceTerminationFieldSuffix<TTermField> Option;
    typedef OptionsParser<TOptions...> Base;

    static_assert(!Base::HasSequenceSizeForcing,
        "SequenceSizeForcingEnabled and SequenceTerminationFieldSuffix are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasSequenceFixedSize,
        "SequenceFixedSize and SequenceTerminationFieldSuffix are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasSequenceSizeFieldPrefix,
        "SequenceSizeFieldPrefix and SequenceTerminationFieldSuffix are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasSequenceTrailingFieldSuffix,
        "SequenceTerminationFieldSuffix and SequenceTrailingFieldSuffix are incompatible options, "
        "mustn't be used together");

    static_assert(!Base::HasCustomValueReader,
        "Cannot mix SequenceTerminationFieldSuffix and CustomValueReader options.");
public:
    static const bool HasSequenceTerminationFieldSuffix = true;
    typedef typename Option::Type SequenceTerminationFieldSuffix;
};

template <typename TInitialiser, typename... TOptions>
class OptionsParser<
    comms::option::DefaultValueInitialiser<TInitialiser>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef comms::option::DefaultValueInitialiser<TInitialiser> Option;
public:
    static const bool HasDefaultValueInitialiser = true;
    typedef typename Option::Type DefaultValueInitialiser;
};

template <typename TValidator, typename... TOptions>
class OptionsParser<
    comms::option::ContentsValidator<TValidator>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef comms::option::ContentsValidator<TValidator> Option;
public:
    static const bool HasCustomValidator = true;
    typedef typename Option::Type CustomValidator;
};

template <comms::ErrorStatus TStatus, typename... TOptions>
class OptionsParser<
    comms::option::FailOnInvalid<TStatus>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef OptionsParser<TOptions...> Base;
    static_assert(!Base::HasIgnoreInvalid,
        "Cannot mix FailOnInvalid and IgnoreInvalid options.");
public:
    static const bool HasFailOnInvalid = true;
    static const comms::ErrorStatus FailOnInvalidStatus = TStatus;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::IgnoreInvalid,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef OptionsParser<TOptions...> Base;
    static_assert(!Base::HasFailOnInvalid,
        "Cannot mix FailOnInvalid and IgnoreInvalid options.");
public:
    static const bool HasIgnoreInvalid = true;
};

template <std::size_t TSize, typename... TOptions>
class OptionsParser<
    comms::option::FixedSizeStorage<TSize>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef comms::option::FixedSizeStorage<TSize> Option;
public:
    static const bool HasFixedSizeStorage = true;
    static const std::size_t FixedSizeStorage = Option::Value;
};

template <typename TType, typename... TOptions>
class OptionsParser<
    comms::option::CustomStorageType<TType>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef comms::option::CustomStorageType<TType> Option;
public:
    static const bool HasCustomStorageType = true;
    typedef typename Option::Type CustomStorageType;
};

template <std::intmax_t TNum, std::intmax_t TDenom, typename... TOptions>
class OptionsParser<
    comms::option::ScalingRatio<TNum, TDenom>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef comms::option::ScalingRatio<TNum, TDenom> Option;
public:
    static const bool HasScalingRatio = true;
    typedef typename Option::Type ScalingRatio;
};

template <typename... TOptions>
class OptionsParser<
    comms::option::EmptyOption,
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


