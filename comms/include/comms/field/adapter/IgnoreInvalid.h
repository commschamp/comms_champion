//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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
#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TBase>
class IgnoreInvalid : public TBase
{
    using BaseImpl = TBase;
public:

    using ValueType = typename BaseImpl::ValueType;

    IgnoreInvalid() = default;

    explicit IgnoreInvalid(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit IgnoreInvalid(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    IgnoreInvalid(const IgnoreInvalid&) = default;
    IgnoreInvalid(IgnoreInvalid&&) = default;
    IgnoreInvalid& operator=(const IgnoreInvalid&) = default;
    IgnoreInvalid& operator=(IgnoreInvalid&&) = default;

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        BaseImpl tmp;
        auto es = tmp.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        if (tmp.valid()) {
            static_cast<BaseImpl&>(*this) = std::move(tmp);
        }

        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        BaseImpl tmp;
        tmp.readNoStatus(iter);

        if (tmp.valid()) {
            static_cast<BaseImpl&>(*this) = std::move(tmp);
        }
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

