//
// Copyright 2016 - 2019 (C). Alex Robenko. All rights reserved.
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
    using BaseImpl = TBase;
public:

    using ValueType = typename BaseImpl::ValueType;

    CustomValueReader() = default;

    explicit CustomValueReader(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit CustomValueReader(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    CustomValueReader(const CustomValueReader&) = default;
    CustomValueReader(CustomValueReader&&) = default;
    CustomValueReader& operator=(const CustomValueReader&) = default;
    CustomValueReader& operator=(CustomValueReader&&) = default;


    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t size)
    {
        return T()(static_cast<BaseImpl&>(*this), iter, size);
    }

    template <std::size_t TFromIdx, typename TIter>
    comms::ErrorStatus readFrom(TIter& iter, std::size_t size) = delete;

    template <std::size_t TUntilIdx, typename TIter>
    comms::ErrorStatus readUntil(TIter& iter, std::size_t size) = delete;

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    comms::ErrorStatus readFromUntil(TIter& iter, std::size_t size) = delete;

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    template <std::size_t TFromIdx, typename TIter>
    void readFromNoStatus(TIter& iter) = delete;

    template <std::size_t TUntilIdx, typename TIter>
    void readUntilNoStatus(TIter& iter) = delete;

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    void readFromUntilNoStatus(TIter& iter) = delete;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

