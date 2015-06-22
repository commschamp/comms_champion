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
#include "details/AdapterBase.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TSizeField, typename TNext>
class SequenceSizeFieldPrefix : public details::AdapterBaseT<TNext>
{
    typedef details::AdapterBaseT<TNext> Base;
    typedef TSizeField SizeField;

public:
    typedef typename Base::ValueType ValueType;
    typedef typename Base::ElementType ElementType;

    SequenceSizeFieldPrefix() = default;

    explicit SequenceSizeFieldPrefix(const ValueType& value)
      : Base(value)
    {
    }

    explicit SequenceSizeFieldPrefix(ValueType&& value)
      : Base(std::move(value))
    {
    }

    SequenceSizeFieldPrefix(const SequenceSizeFieldPrefix&) = default;
    SequenceSizeFieldPrefix(SequenceSizeFieldPrefix&&) = default;
    SequenceSizeFieldPrefix& operator=(const SequenceSizeFieldPrefix&) = default;
    SequenceSizeFieldPrefix& operator=(SequenceSizeFieldPrefix&&) = default;

    constexpr std::size_t length() const
    {
        typedef typename SizeField::ValueType SizeValueType;
        return
            SizeField(static_cast<SizeValueType>(Base::value().size())).length() +
            Base::length();
    }

    static constexpr std::size_t minLength()
    {
        return SizeField::minLength() + Base::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return SizeField::maxLength() + Base::maxLength();
    }

    bool valid() const
    {
        typedef typename SizeField::ValueType SizeValueType;
        return
            SizeField(static_cast<SizeValueType>(Base::value().size())).valid() &&
            Base::valid();
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        SizeField sizeField;
        auto es = sizeField.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto count = static_cast<std::size_t>(sizeField.value());
        len -= sizeField.length();

        Base::clear();
        while (0 < count) {
            auto elem = ElementType();
            es = Base::readElement(elem, iter, len);

            if (es != ErrorStatus::Success) {
                return es;
            }

            Base::pushBack(std::move(elem));
            --count;
        }

        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        typedef typename SizeField::ValueType SizeValueType;
        SizeField sizeField(static_cast<SizeValueType>(Base::value().size()));
        auto es = sizeField.write(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        GASSERT(sizeField.length() <= len);
        return Base::write(iter, len - sizeField.length());
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




