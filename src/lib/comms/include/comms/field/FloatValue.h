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

#include "comms/ErrorStatus.h"
#include "comms/field/category.h"
#include "comms/options.h"
#include "basic/FloatValue.h"
#include "details/AdaptBasicField.h"

namespace comms
{

namespace field
{

template <typename TFieldBase, typename T, typename... TOptions>
class FloatValue : public TFieldBase
{
    typedef TFieldBase Base;

    typedef basic::FloatValue<TFieldBase, T> BasicField;
    typedef details::AdaptBasicFieldT<BasicField, TOptions...> ThisField;

    static_assert(std::is_base_of<comms::field::category::NumericValueField, typename ThisField::Category>::value,
        "ThisField is expected to be of NumericFieldCategory");
public:

    typedef details::OptionsParser<TOptions...> ParsedOptions;
    typedef typename ThisField::ValueType ValueType;

    FloatValue() = default;

    explicit FloatValue(const ValueType& value)
      : field_(value)
    {
    }

    explicit FloatValue(ValueType&& value)
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

private:
    ThisField field_;
};


/// @brief Equality comparison operator.
/// @related FloatValue
template <typename... TArgs>
bool operator==(
    const FloatValue<TArgs...>& field1,
    const FloatValue<TArgs...>& field2)
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @related FloatValue
template <typename... TArgs>
bool operator!=(
    const FloatValue<TArgs...>& field1,
    const FloatValue<TArgs...>& field2)
{
    return field1.value() != field2.value();
}

/// @brief Equivalence comparison operator.
/// @related FloatValue
template <typename... TArgs>
bool operator<(
    const FloatValue<TArgs...>& field1,
    const FloatValue<TArgs...>& field2)
{
    return field1.value() < field2.value();
}

namespace details
{

template <typename T>
struct IsFloatValue
{
    static const bool Value = false;
};

template <typename... TArgs>
struct IsFloatValue<comms::field::FloatValue<TArgs...> >
{
    static const bool Value = true;
};

}  // namespace details

template <typename T>
constexpr bool isFloatValue()
{
    return details::IsFloatValue<T>::Value;
}


}  // namespace field

}  // namespace comms



