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

template <comms::ErrorStatus TStatus, typename TBase>
class FailOnInvalid : public TBase
{
    using BaseImpl = TBase;
public:

    using ValueType = typename BaseImpl::ValueType;

    FailOnInvalid() = default;

    explicit FailOnInvalid(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit FailOnInvalid(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    FailOnInvalid(const FailOnInvalid&) = default;
    FailOnInvalid(FailOnInvalid&&) = default;
    FailOnInvalid& operator=(const FailOnInvalid&) = default;
    FailOnInvalid& operator=(FailOnInvalid&&) = default;

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        BaseImpl tmp;
        auto es = tmp.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        if (!tmp.valid()) {
            return TStatus;
        }

        static_cast<BaseImpl&>(*this) = std::move(tmp);
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

