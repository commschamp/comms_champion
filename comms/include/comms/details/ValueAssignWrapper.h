//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
