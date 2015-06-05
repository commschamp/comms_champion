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
#include "basic/Bundle.h"
#include "details/AdaptBasicField.h"


namespace comms
{

namespace field
{

template <typename TMembers, typename... TOptions>
class Bundle
{
    static_assert(comms::util::IsTuple<TMembers>::Value,
        "TMembers is expected to be a tuple of BundleMember<...>");

    static_assert(
        1U < std::tuple_size<TMembers>::value,
        "Number of members is expected to be at least 2.");

    typedef basic::Bundle<TMembers> BasicField;
    typedef details::AdaptBasicFieldT<BasicField, TOptions...> ThisField;

public:
    typedef details::OptionsParser<TOptions...> ParsedOptions;
    typedef typename ThisField::Members Members;
    typedef typename ThisField::ValueType ValueType;
    typedef typename ThisField::ParamValueType ParamValueType;

    Bundle() = default;

    explicit Bundle(ParamValueType value)
      : field_(value)
    {
    }

    explicit Bundle(ValueType&& value)
      : field_(std::move(value))
    {
    }

    Members& members()
    {
        return field_.members();
    }

    const Members& members() const
    {
        return field_.members();
    }

    ParamValueType getValue() const
    {
        return field_.getValue();
    }

    void setValue(ParamValueType value)
    {
        field_.setValue(value);
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

    constexpr bool valid() const {
        return field_.valid();
    }

private:
    ThisField field_;
};

/// @brief Equality comparison operator.
/// @related Bundle
template <typename... TArgs>
bool operator==(
    const Bundle<TArgs...>& field1,
    const Bundle<TArgs...>& field2)
{
    return field1.getValue() == field2.getValue();
}

/// @brief Non-equality comparison operator.
/// @related Bundle
template <typename... TArgs>
bool operator!=(
    const Bundle<TArgs...>& field1,
    const Bundle<TArgs...>& field2)
{
    return field1.getValue() != field2.getValue();
}

namespace details
{

template <typename T>
struct IsBundle
{
    static const bool Value = false;
};

template <typename... TArgs>
struct IsBundle<comms::field::Bundle<TArgs...> >
{
    static const bool Value = true;
};

}  // namespace details

template <typename T>
constexpr bool isBundle()
{
    return details::IsBundle<T>::Value;
}


}  // namespace field

}  // namespace comms


