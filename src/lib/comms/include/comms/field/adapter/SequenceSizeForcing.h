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
#include "details/AdapterBase.h"

#include <limits>

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TNext>
class SequenceSizeForcing : public details::AdapterBaseT<TNext>
{
    typedef details::AdapterBaseT<TNext> Base;
public:
    typedef typename Base::ValueType ValueType;
    typedef typename Base::ElementType ElementType;

    SequenceSizeForcing() = default;

    explicit SequenceSizeForcing(const ValueType& val)
      : Base(val)
    {
    }

    explicit SequenceSizeForcing(ValueType&& val)
      : Base(std::move(val))
    {
    }

    SequenceSizeForcing(const SequenceSizeForcing&) = default;
    SequenceSizeForcing(SequenceSizeForcing&&) = default;
    SequenceSizeForcing& operator=(const SequenceSizeForcing&) = default;
    SequenceSizeForcing& operator=(SequenceSizeForcing&&) = default;

    void forceReadElemCount(std::size_t val)
    {
        GASSERT(val != Cleared);
        forced_ = val;
    }

    void clearReadElemCount()
    {
        forced_ = Cleared;
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        if (forced_ == Cleared) {
            return Base::read(iter, len);
        }

        return Base::readN(forced_, iter, len);
    }

private:
    static const auto Cleared = std::numeric_limits<std::size_t>::max();
    std::size_t forced_ = Cleared;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


