//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "comms/comms.h"

namespace comms_champion
{

namespace details
{

struct FieldWrapperIntValueTag {};
struct FieldWrapperBitmaskValueTag {};
struct FieldWrapperEnumValueTag {};
struct FieldWrapperStringTag {};
struct FieldWrapperBitfieldTag {};
struct FieldWrapperOptionalTag {};
struct FieldWrapperBundleTag {};
struct FieldWrapperRawDataArrayListTag {};
struct FieldWrapperFieldsArrayListTag {};
struct FieldWrapperFloatValueTag {};
struct FieldWrapperUnknownValueTag {};

template <typename TField>
struct FieldWrapperTagOf
{
    static_assert(!comms::field::isIntValue<TField>(),
        "IntValue is perceived as unknown type");
    static_assert(!comms::field::isBitmaskValue<TField>(),
        "BitmaskValue is perceived as unknown type");
    static_assert(!comms::field::isEnumValue<TField>(),
        "EnumValue is perceived as unknown type");
    static_assert(!comms::field::isString<TField>(),
        "String is perceived as unknown type");
    static_assert(!comms::field::isBitfield<TField>(),
        "Bitfield is perceived as unknown type");
    static_assert(!comms::field::isOptional<TField>(),
        "Optional is perceived as unknown type");
    static_assert(!comms::field::isArrayList<TField>(),
        "ArrayList is perceived as unknown type");
    static_assert(!comms::field::isFloatValue<TField>(),
        "FloatValue is perceived as unknown type");

    typedef FieldWrapperUnknownValueTag Type;
};

template <typename... TArgs>
struct FieldWrapperTagOf<comms::field::IntValue<TArgs...> >
{
    static_assert(
        comms::field::isIntValue<comms::field::IntValue<TArgs...> >(),
        "isIntValue is supposed to return true");

    typedef FieldWrapperIntValueTag Type;
};

template <typename... TArgs>
struct FieldWrapperTagOf<comms::field::BitmaskValue<TArgs...> >
{
    static_assert(
        comms::field::isBitmaskValue<comms::field::BitmaskValue<TArgs...> >(),
        "isBitmaskValue is supposed to return true");

    typedef FieldWrapperBitmaskValueTag Type;
};

template <typename... TArgs>
struct FieldWrapperTagOf<comms::field::EnumValue<TArgs...> >
{
    static_assert(
        comms::field::isEnumValue<comms::field::EnumValue<TArgs...> >(),
        "isEnumValue is supposed to return true");

    typedef FieldWrapperEnumValueTag Type;
};

template <typename... TArgs>
struct FieldWrapperTagOf<comms::field::String<TArgs...> >
{
    static_assert(
        comms::field::isString<comms::field::String<TArgs...> >(),
        "isString is supposed to return true");

    typedef FieldWrapperStringTag Type;
};

template <typename... TArgs>
struct FieldWrapperTagOf<comms::field::Bitfield<TArgs...> >
{
    static_assert(
        comms::field::isBitfield<comms::field::Bitfield<TArgs...> >(),
        "isBitfield is supposed to return true");

    typedef FieldWrapperBitfieldTag Type;
};

template <typename... TArgs>
struct FieldWrapperTagOf<comms::field::Optional<TArgs...> >
{
    static_assert(
        comms::field::isOptional<comms::field::Optional<TArgs...> >(),
        "isOptional is supposed to return true");

    typedef FieldWrapperOptionalTag Type;
};

template <typename... TArgs>
struct FieldWrapperTagOf<comms::field::Bundle<TArgs...> >
{
    static_assert(
        comms::field::isBundle<comms::field::Bundle<TArgs...> >(),
        "isBundle is supposed to return true");

    typedef FieldWrapperBundleTag Type;
};

template <typename... TArgs>
struct FieldWrapperTagOf<comms::field::ArrayList<TArgs...> >
{
    static_assert(
        comms::field::isArrayList<comms::field::ArrayList<TArgs...> >(),
        "isArrayList is supposed to return true");

    typedef typename comms::field::ArrayList<TArgs...> FieldType;
    typedef typename FieldType::ValueType::value_type ElementType;

    typedef typename std::conditional<
        std::is_integral<ElementType>::value,
        FieldWrapperRawDataArrayListTag,
        FieldWrapperFieldsArrayListTag
    >::type Type;
};

template <typename... TArgs>
struct FieldWrapperTagOf<comms::field::FloatValue<TArgs...> >
{
    static_assert(
        comms::field::isFloatValue<comms::field::FloatValue<TArgs...> >(),
        "isFloatValue is supposed to return true");

    typedef FieldWrapperFloatValueTag Type;
};


template <typename TField>
using FieldWrapperTagOfT = typename FieldWrapperTagOf<TField>::Type;

}  // namespace details

}  // namespace comms_champion



