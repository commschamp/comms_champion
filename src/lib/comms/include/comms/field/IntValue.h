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

#include <ratio>
#include <limits>
#include <type_traits>

#include "comms/ErrorStatus.h"
#include "comms/field/category.h"
#include "comms/options.h"
#include "basic/IntValue.h"
#include "details/AdaptBasicField.h"

namespace comms
{

namespace field
{

template <typename TFieldBase, typename T, typename... TOptions>
class IntValue : public TFieldBase
{
    typedef TFieldBase Base;

    typedef basic::IntValue<TFieldBase, T> BasicField;
    typedef details::AdaptBasicFieldT<BasicField, TOptions...> ThisField;

    static_assert(std::is_base_of<comms::field::category::NumericValueField, typename ThisField::Category>::value,
        "ThisField is expected to be of NumericFieldCategory");
public:

    typedef details::OptionsParser<TOptions...> ParsedOptions;
    typedef typename ThisField::ValueType ValueType;

    IntValue() = default;

    explicit IntValue(const ValueType& value)
      : field_(value)
    {
    }

    explicit IntValue(ValueType&& value)
      : field_(value)
    {
    }

    const ValueType& value() const
    {
        return field_.value();
    }

    ValueType& value()
    {
        return field_.value();
    }

    constexpr std::size_t length() const
    {
        return field_.length();
    }

    static constexpr std::size_t minLength()
    {
        return ThisField::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return ThisField::maxLength();
    }

    constexpr bool valid() const
    {
        return field_.valid();
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        return field_.read(iter, size);
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        return field_.write(iter, size);
    }

    template <typename TRet>
    constexpr TRet scaleAs() const
    {
        static_assert(std::is_floating_point<TRet>::value,
            "TRet is expected to be floating point type");
        return static_cast<TRet>(value()) * (static_cast<TRet>(ThisField::ScalingRatio::num) / static_cast<TRet>(ThisField::ScalingRatio::den));
    }

    template <typename TScaled>
    void setScaled(TScaled val)
    {
        typedef typename std::decay<decltype(val)>::type DecayedType;
        auto mul = 10;
        if ((ThisField::ScalingRatio::den & (ThisField::ScalingRatio::den - 1)) == 0) {
            mul = 2;
        }

        auto epsilon = static_cast<TScaled>(ThisField::ScalingRatio::num) / static_cast<TScaled>(ThisField::ScalingRatio::den * mul);
        if (val < static_cast<DecayedType>(0)) {
            epsilon = -epsilon;
        }

        value() =
            static_cast<ValueType>(
                ((val + epsilon) * static_cast<TScaled>(ThisField::ScalingRatio::den)) / static_cast<TScaled>(ThisField::ScalingRatio::num));
    }

private:
    ThisField field_;
};


/// @brief Equality comparison operator.
/// @related IntValue
template <typename... TArgs>
bool operator==(
    const IntValue<TArgs...>& field1,
    const IntValue<TArgs...>& field2)
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @related IntValue
template <typename... TArgs>
bool operator!=(
    const IntValue<TArgs...>& field1,
    const IntValue<TArgs...>& field2)
{
    return field1.value() != field2.value();
}

/// @brief Equivalence comparison operator.
/// @related IntValue
template <typename... TArgs>
bool operator<(
    const IntValue<TArgs...>& field1,
    const IntValue<TArgs...>& field2)
{
    return field1.value() < field2.value();
}

namespace details
{

template <typename T>
struct IsIntValue
{
    static const bool Value = false;
};

template <typename... TArgs>
struct IsIntValue<comms::field::IntValue<TArgs...> >
{
    static const bool Value = true;
};

}  // namespace details

template <typename T>
constexpr bool isIntValue()
{
    return details::IsIntValue<T>::Value;
}


}  // namespace field

}  // namespace comms



