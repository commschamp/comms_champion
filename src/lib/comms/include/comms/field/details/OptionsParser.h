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
    static const bool HasSerOffset = false;
    static const bool HasFixedLengthLimit = false;
    static const bool HasFixedBitLengthLimit = false;
    static const bool HasVarLengthLimits = false;
    static const bool HasSequenceSizeFieldPrefix=false;
    static const bool HasDefaultValueInitialiser = false;
    static const bool HasCustomValidator = false;
    static const bool HasFailOnInvalid = false;
    static const bool HasIgnoreInvalid = false;
    static const bool HasFixedSizeStorage = false;
};

template <long long int TOffset, typename... TOptions>
class OptionsParser<
    comms::option::NumValueSerOffset<TOffset>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef comms::option::NumValueSerOffset<TOffset> Option;

public:
    static const bool HasSerOffset = true;
    static const std::size_t SerOffset = Option::Value;
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
        "Cannot mix FixedLength and VarLength options.");
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
public:
    static const bool HasVarLengthLimits = true;
    static const std::size_t MinVarLength = Option::MinValue;
    static const std::size_t MaxVarLength = Option::MaxValue;
};

template <typename TSizeField, typename... TOptions>
class OptionsParser<
    comms::option::SequenceSizeFieldPrefix<TSizeField>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef comms::option::SequenceSizeFieldPrefix<TSizeField> Option;
public:
    static const bool HasSequenceSizeFieldPrefix = true;
    typedef typename Option::Type SequenceSizeFieldPrefix;
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

template <typename... TOptions>
class OptionsParser<
    comms::option::FailOnInvalid,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef OptionsParser<TOptions...> Base;
    static_assert(!Base::HasIgnoreInvalid,
        "Cannot mix FailOnInvalid and IgnoreInvalid options.");
public:
    static const bool HasFailOnInvalid = true;
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

}  // namespace details

}  // namespace field

}  // namespace comms


