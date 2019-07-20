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

template <typename TTermField, typename TBase>
class SequenceTerminationFieldSuffix : public TBase
{
    using BaseImpl = TBase;
    using TermField = TTermField;

    static_assert(!TermField::isVersionDependent(),
            "Suffix fields must not be version dependent");

public:
    using ValueType = typename BaseImpl::ValueType;
    using ElementType = typename BaseImpl::ElementType;

    SequenceTerminationFieldSuffix() = default;

    explicit SequenceTerminationFieldSuffix(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit SequenceTerminationFieldSuffix(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    SequenceTerminationFieldSuffix(const SequenceTerminationFieldSuffix&) = default;
    SequenceTerminationFieldSuffix(SequenceTerminationFieldSuffix&&) = default;
    SequenceTerminationFieldSuffix& operator=(const SequenceTerminationFieldSuffix&) = default;
    SequenceTerminationFieldSuffix& operator=(SequenceTerminationFieldSuffix&&) = default;

    constexpr std::size_t length() const
    {
        return TermField().length() + BaseImpl::length();
    }

    static constexpr std::size_t minLength()
    {
        return TermField::minLength() + BaseImpl::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return TermField::maxLength() + BaseImpl::maxLength();
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterTag = typename std::iterator_traits<IterType>::iterator_category;
        static_assert(std::is_base_of<std::random_access_iterator_tag, IterTag>::value,
            "Only random access iterator for reading is supported with comms::option::def::SequenceTerminationFieldSuffix option");

        using ElemTag =
            typename std::conditional<
                std::is_integral<ElementType>::value && (sizeof(ElementType) == sizeof(std::uint8_t)),
                RawDataTag,
                FieldTag
            >::type;

        return readInternal(iter, len, ElemTag());
    }


    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    template <typename TIter>
    comms::ErrorStatus write(TIter& iter, std::size_t len) const
    {
        TermField termField;
        auto trailLen = termField.length();
        auto es = BaseImpl::write(iter, len - trailLen);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        return termField.write(iter, trailLen);
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        TermField termField;
        BaseImpl::writeNoStatus(iter);
        termField.writeNoStatus(iter);
    }

private:
    struct RawDataTag {};
    struct FieldTag {};

    template <typename TIter>
    comms::ErrorStatus readInternal(TIter& iter, std::size_t len, FieldTag)
    {
        BaseImpl::clear();
        TermField termField;
        while (true) {
            auto iterCpy = iter;
            auto es = termField.read(iterCpy, len);
            if ((es == comms::ErrorStatus::Success) &&
                (termField == TermField())){
                std::advance(iter, std::distance(iter, iterCpy));
                return es;
            }

            auto& elem = BaseImpl::createBack();
            es = BaseImpl::readElement(elem, iter, len);
            if (es != comms::ErrorStatus::Success) {
                BaseImpl::value().pop_back();
                return es;
            }
        }

        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    comms::ErrorStatus readInternal(TIter& iter, std::size_t len, RawDataTag)
    {
        TermField termField;
        std::size_t consumed = 0U;
        std::size_t termFieldLen = 0U;
        while (consumed < len) {
            auto iterCpy = iter + consumed;
            auto es = termField.read(iterCpy, len - consumed);
            if ((es == comms::ErrorStatus::Success) &&
                (termField == TermField())){
                termFieldLen = static_cast<std::size_t>(std::distance(iter + consumed, iterCpy));
                break;
            }

            ++consumed;
        }

        if (len <= consumed) {
            return comms::ErrorStatus::NotEnoughData;
        }

        auto iterCpy = iter;
        auto es = BaseImpl::read(iterCpy, consumed);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto fullConsumeLen = consumed + termFieldLen;
        GASSERT(fullConsumeLen <= len);
        std::advance(iter, fullConsumeLen);
        return comms::ErrorStatus::Success;
    }

};

}  // namespace adapter

}  // namespace field

}  // namespace comms




