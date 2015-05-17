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

#include "comms/Assert.h"

namespace comms
{

namespace field
{

template <typename TField>
class Optional
{
public:
    typedef TField Field;
    typedef typename Field::ParamValueType ParamValueType;

    enum class Mode
    {
        Tentative,
        Exists,
        Missing,
        NumOfModes // Must be last
    };

    Optional() = default;

    explicit Optional(const Field& field, Mode mode = Mode::Tentative)
      : field_(field),
        mode_(mode)
    {
    }

    explicit Optional(Field&& field, Mode mode = Mode::Tentative)
      : field_(std::move(field)),
        mode_(mode)
    {
    }

    Optional(const Optional&) = default;
    Optional(Optional&&) = default;
    ~Optional() = default;

    Optional& operator=(const Optional&) = default;
    Optional& operator=(Optional&&) = default;

    Field& field()
    {
        return field_;
    }

    const Field& field() const
    {
        return field_;
    }

    Mode getMode() const
    {
        return mode_;
    }

    void setMode(Mode value)
    {
        GASSERT(value < Mode::NumOfModes);
        mode_ = value;
    }

    ParamValueType getValue() const
    {
        return field_.getValue();
    }

    void setValue(ParamValueType value)
    {
        field_.setValue(value);
    }

    std::size_t length() const
    {
        if (mode_ == Mode::Missing) {
            return 0U;
        }

        return field_.length();
    }

    static constexpr std::size_t minLength()
    {
        return Field::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return Field::maxLength();
    }

    bool valid() const
    {
        return field_.valid();
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        if (mode_ == Mode::Missing) {
            return comms::ErrorStatus::Success;
        }

        if ((mode_ == Mode::Tentative) && (0U == len)) {
            mode_ = Mode::Missing;
            return comms::ErrorStatus::Success;
        }

        auto es = field_.read(iter, len);
        if (es == comms::ErrorStatus::Success) {
            mode_ = Mode::Exists;
        }
        return es;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        if (mode_ == Mode::Missing) {
            return comms::ErrorStatus::Success;
        }

        if ((mode_ == Mode::Tentative) && (0U == len)) {
            return comms::ErrorStatus::Success;
        }

        return field_.write(iter, len);
    }

private:
    Field field_;
    Mode mode_ = Mode::Tentative;
};

/// @brief Equality comparison operator.
/// @related Optional
template <typename... TArgs>
bool operator==(
    const Optional<TArgs...>& field1,
    const Optional<TArgs...>& field2)
{
    return field1.getValue() == field2.getValue();
}

/// @brief Non-equality comparison operator.
/// @related Optional
template <typename... TArgs>
bool operator!=(
    const Optional<TArgs...>& field1,
    const Optional<TArgs...>& field2)
{
    return field1.getValue() != field2.getValue();
}

/// @brief Equivalence comparison operator.
/// @related Optional
template <typename... TArgs>
bool operator<(
    const Optional<TArgs...>& field1,
    const Optional<TArgs...>& field2)
{
    return field1.getValue() < field2.getValue();
}

namespace details
{

template <typename T>
struct IsOptional
{
    static const bool Value = false;
};

template <typename... TArgs>
struct IsOptional<comms::field::Optional<TArgs...> >
{
    static const bool Value = true;
};

}  // namespace details

template <typename T>
constexpr bool isOptional()
{
    return details::IsOptional<T>::Value;
}


}  // namespace field

}  // namespace comms


