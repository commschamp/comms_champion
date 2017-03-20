//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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
#include "details/AdapterBase.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <comms::ErrorStatus TStatus, typename TNext>
class FailOnInvalid : public details::AdapterBaseT<TNext>
{
    typedef details::AdapterBaseT<TNext> Base;
public:

    typedef typename Base::Next Next;
    typedef typename Base::ValueType ValueType;

    FailOnInvalid() = default;

    explicit FailOnInvalid(const ValueType& val)
      : Base(val)
    {
    }

    explicit FailOnInvalid(ValueType&& val)
      : Base(std::move(val))
    {
    }

    FailOnInvalid(const FailOnInvalid&) = default;
    FailOnInvalid(FailOnInvalid&&) = default;
    FailOnInvalid& operator=(const FailOnInvalid&) = default;
    FailOnInvalid& operator=(FailOnInvalid&&) = default;

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        Next nextTmp;
        auto es = nextTmp.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        if (!nextTmp.valid()) {
            return TStatus;
        }

        Base::next() = std::move(nextTmp);
        return comms::ErrorStatus::Success;
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

