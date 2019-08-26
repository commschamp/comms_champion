//
// Copyright 2019 (C). Alex Robenko. All rights reserved.
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

namespace comms
{

namespace details
{

template <typename TField>
class FieldAssignWrapper
{
    using FieldType = typename std::decay<TField>::type;

public:
    explicit FieldAssignWrapper(TField& field) : m_field(field) {}

    template <typename TVal>
    FieldAssignWrapper& operator=(TVal&& val)
    {
        using ValueType = typename std::decay<decltype(val)>::type;
        using FieldValueType = typename FieldType::ValueType;

        static const bool IsValueNumeric =
            std::is_arithmetic<ValueType>::value ||
            std::is_enum<ValueType>::value;

        static const bool IsFieldValueNumeric =
            std::is_arithmetic<FieldValueType>::value ||
            std::is_enum<FieldValueType>::value;


        using Tag =
            typename std::conditional<
                (IsValueNumeric && IsFieldValueNumeric) ||
                (std::is_convertible<ValueType, FieldValueType>::value),
                CastAssignTag,
                ForwardAssignTag
            >::type;

        assignValue(std::forward<TVal>(val), Tag());
        return *this;
    }

    operator TField&()
    {
        return m_field;
    }

    operator const TField&() const
    {
        return m_field;
    }

private:
    struct CastAssignTag {};
    struct ForwardAssignTag {};

    template <typename TVal>
    void assignValue(TVal&& val, CastAssignTag)
    {
        m_field.value() = static_cast<typename FieldType::ValueType>(std::forward<TVal>(val));
    }

    template <typename TVal>
    void assignValue(TVal&& val, ForwardAssignTag)
    {
        m_field.value() = std::forward<TVal>(val);
    }

    TField& m_field;
};


} // namespace details

} // namespace comms
