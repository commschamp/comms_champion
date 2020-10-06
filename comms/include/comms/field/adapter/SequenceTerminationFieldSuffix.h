//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/util/type_traits.h"
#include "comms/details/tag.h"

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
            typename comms::util::LazyShallowConditional<
                std::is_integral<ElementType>::value && (sizeof(ElementType) == sizeof(std::uint8_t))
            >::template Type<
                RawDataTag,
                FieldTag
            >;

        return readInternal(iter, len, ElemTag());
    }

    static constexpr bool hasReadNoStatus()
    {
        return false;
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
    template <typename... TParams>
    using RawDataTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using FieldTag = comms::details::tag::Tag2<>;

    template <typename TIter, typename... TParams>
    comms::ErrorStatus readInternal(TIter& iter, std::size_t len, FieldTag<TParams...>)
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

    template <typename TIter, typename... TParams>
    comms::ErrorStatus readInternal(TIter& iter, std::size_t len, RawDataTag<TParams...>)
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
        COMMS_ASSERT(fullConsumeLen <= len);
        std::advance(iter, fullConsumeLen);
        return comms::ErrorStatus::Success;
    }

};

}  // namespace adapter

}  // namespace field

}  // namespace comms




