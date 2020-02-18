//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
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

template <typename T>
class ValueAssignWrapper
{
    using ValueType = typename std::decay<T>::type;

public:
    explicit ValueAssignWrapper(T& value) : m_value(value) {}

    template <typename U>
    ValueAssignWrapper& operator=(U&& val)
    {
        m_value = static_cast<ValueType>(val);
        return *this;
    }

    operator ValueType&()
    {
        return m_value;
    }

    operator const ValueType&() const
    {
        return m_value;
    }

private:

    T& m_value;
};

} // namespace details

} // namespace comms
