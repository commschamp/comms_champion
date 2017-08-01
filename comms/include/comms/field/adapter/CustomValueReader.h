//
// Copyright 2016 - 2017 (C). Alex Robenko. All rights reserved.
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

namespace comms
{

namespace field
{

namespace adapter
{

template <typename T, typename TBase>
class CustomValueReader : public TBase
{
    using Base = TBase;
public:

    using ValueType = typename Base::ValueType;

    CustomValueReader() = default;

    explicit CustomValueReader(const ValueType& val)
      : Base(val)
    {
    }

    explicit CustomValueReader(ValueType&& val)
      : Base(std::move(val))
    {
    }

    CustomValueReader(const CustomValueReader&) = default;
    CustomValueReader(CustomValueReader&&) = default;
    CustomValueReader& operator=(const CustomValueReader&) = default;
    CustomValueReader& operator=(CustomValueReader&&) = default;


    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t size)
    {
        return T()(static_cast<Base&>(*this), iter, size);
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

