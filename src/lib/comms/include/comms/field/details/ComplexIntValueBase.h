//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include <type_traits>
#include <limits>

#include "comms/options.h"
#include "comms/traits.h"
#include "comms/util/SizeToType.h"
#include "comms/util/IntegralPromotion.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename TField, typename T, typename... TOptions>
class ComplexIntValueBase;

template <typename TField, typename T>
class ComplexIntValueBase<TField, T> : public TField
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef TField Base;

protected:

    typedef T ValueType;
    typedef typename comms::util::IntegralPromotion<ValueType>::Type OffsetType;
    typedef typename util::SizeToType<sizeof(T), std::is_signed<T>::value>::Type SerialisedType;

    using Base::Base;

    static const std::size_t MinLength = sizeof(T);
    static const std::size_t MaxLength = sizeof(T);
    static const auto Offset = static_cast<OffsetType>(0);
    static const bool HasCustomInitialiser = false;
    static const bool HasCustomValidator = false;

    typedef comms::traits::behaviour::UseValue InvalidValueBehaviourTag;
};

namespace basic_int_value_details
{

template <long long int TVal1, long long int TVal2, bool TLess>
struct MinValueHelper;

template <long long int TVal1, long long int TVal2>
struct MinValueHelper<TVal1, TVal2, true>
{
    static const long long int Value = TVal1;
};

template <long long int TVal1, long long int TVal2>
struct MinValueHelper<TVal1, TVal2, false>
{
    static const long long int Value = TVal2;
};

template <long long int TVal1, long long int TVal2>
struct MinValue
{
    static const long long int Value =
        MinValueHelper<TVal1, TVal2, (TVal1 < TVal2)>::Value;
};

template <long long int TVal1, long long int TVal2, bool TLess>
struct MaxValueHelper;

template <long long int TVal1, long long int TVal2>
struct MaxValueHelper<TVal1, TVal2, true>
{
    static const long long int Value = TVal2;
};

template <long long int TVal1, long long int TVal2>
struct MaxValueHelper<TVal1, TVal2, false>
{
    static const long long int Value = TVal1;
};

template <long long int TVal1, long long int TVal2>
struct MaxValue
{
    static const long long int Value =
        MaxValueHelper<TVal1, TVal2, (TVal1 < TVal2)>::Value;
};

}  // namespace basic_int_value_details

template <typename TField, typename T, std::size_t TLen, typename... TOptions>
class ComplexIntValueBase<
    TField,
    T,
    comms::option::FixedLength<TLen>,
    TOptions...> : public ComplexIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef ComplexIntValueBase<TField, T, TOptions...> Base;
    typedef comms::option::FixedLength<TLen> Option;

protected:
    using Base::ComplexIntValueBase;

    typedef typename util::SizeToType<TLen, std::is_signed<T>::value>::Type SerialisedType;

    static const std::size_t MinLength = Option::Value;
    static const std::size_t MaxLength = Option::Value;

    static_assert(
        0 < MinLength,
        "Fixed length cannot be 0.");

};

template <typename TField, typename T, std::size_t TMin, std::size_t TMax, typename... TOptions>
class ComplexIntValueBase<
    TField,
    T,
    comms::option::VarLength<TMin, TMax>,
    TOptions...> : public ComplexIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef ComplexIntValueBase<TField, T, TOptions...> Base;
    typedef comms::option::VarLength<TMin, TMax> Option;

protected:
    using Base::ComplexIntValueBase;

    static const std::size_t MinLength = Option::MinValue;
    static const std::size_t MaxLength = Option::MaxValue;

    typedef typename util::SizeToType<MaxLength, std::is_signed<T>::value>::Type SerialisedType;

    static_assert(
        0 < MinLength,
        "MinLength cannot be 0.");

    static_assert(
        MinLength <= MaxLength,
        "MinLength must be non-greater than MaxLength");
};

template <typename TField, typename T, long long int TOff, typename... TOptions>
class ComplexIntValueBase<
    TField,
    T,
    comms::option::NumValueSerOffset<TOff>,
    TOptions...> : public ComplexIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef ComplexIntValueBase<TField, T, TOptions...> Base;
    typedef comms::option::NumValueSerOffset<TOff> Option;
protected:
    using Base::ComplexIntValueBase;

    static const auto Offset = static_cast<decltype(Base::Offset)>(Option::Value);
};

template <typename TField, typename T, typename TInit, typename... TOptions>
class ComplexIntValueBase<
    TField,
    T,
    comms::option::DefaultValueInitialiser<TInit>,
    TOptions...> : public ComplexIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef ComplexIntValueBase<TField, T, TOptions...> Base;
    typedef comms::option::DefaultValueInitialiser<TInit> Option;

protected:
    using Base::ComplexIntValueBase;

    typedef typename Option::Type DefaultValueInitialiser;
    static const bool HasCustomInitialiser = true;
};

template <typename TField, typename T, typename TValidator, typename... TOptions>
class ComplexIntValueBase<
    TField,
    T,
    comms::option::ContentsValidator<TValidator>,
    TOptions...> : public ComplexIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef ComplexIntValueBase<TField, T, TOptions...> Base;
    typedef comms::option::ContentsValidator<TValidator> Option;

protected:
    using Base::ComplexIntValueBase;

    typedef typename Option::Type ContentsValidator;
    static const bool HasCustomValidator = true;
};

template <typename TField, typename T, typename TBehaviour, typename... TOptions>
class ComplexIntValueBase<
    TField,
    T,
    comms::option::InvalidValueBehaviour<TBehaviour>,
    TOptions...> : public ComplexIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef ComplexIntValueBase<TField, T, TOptions...> Base;
    typedef comms::option::InvalidValueBehaviour<TBehaviour> Option;

protected:
    using Base::ComplexIntValueBase;

    typedef typename Option::Type InvalidValueBehaviourTag;
};



}  // namespace details

}  // namespace field

}  // namespace comms


