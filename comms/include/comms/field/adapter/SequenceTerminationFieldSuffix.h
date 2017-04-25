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

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TTermField, typename TNext>
class SequenceTerminationFieldSuffix : public details::AdapterBaseT<TNext>
{
    using Base = details::AdapterBaseT<TNext>;
    using TermField = TTermField;

public:
    using ValueType = typename Base::ValueType;
    using ElementType = typename Base::ElementType;

    SequenceTerminationFieldSuffix() = default;

    explicit SequenceTerminationFieldSuffix(const ValueType& val)
      : Base(val)
    {
    }

    explicit SequenceTerminationFieldSuffix(ValueType&& val)
      : Base(std::move(val))
    {
    }

    SequenceTerminationFieldSuffix(const SequenceTerminationFieldSuffix&) = default;
    SequenceTerminationFieldSuffix(SequenceTerminationFieldSuffix&&) = default;
    SequenceTerminationFieldSuffix& operator=(const SequenceTerminationFieldSuffix&) = default;
    SequenceTerminationFieldSuffix& operator=(SequenceTerminationFieldSuffix&&) = default;

    constexpr std::size_t length() const
    {
        return TermField().length() + Base::length();
    }

    static constexpr std::size_t minLength()
    {
        return TermField::minLength() + Base::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return TermField::maxLength() + Base::maxLength();
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterTag = typename std::iterator_traits<IterType>::iterator_category;
        static_assert(std::is_base_of<std::random_access_iterator_tag, IterTag>::value,
            "Only random access iterator for reading is supported with comms::option::SequenceTerminationFieldSuffix option");

        Base::clear();
        TermField termField;
        while (true) {
            auto iterCpy = iter;
            auto es = termField.read(iterCpy, len);
            if ((es == comms::ErrorStatus::Success) &&
                (termField == TermField())){
                std::advance(iter, termField.length());
                return es;
            }

            ElementType elem;
            es = Base::readElement(elem, iter, len);
            if (es != comms::ErrorStatus::Success) {
                return es;
            }

            Base::pushBack(std::move(elem));
        }

        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        TermField termField;
        auto trailLen = termField.length();
        auto es = Base::write(iter, len - trailLen);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        return termField.write(iter, trailLen);
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




