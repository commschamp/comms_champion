//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TSizeField, typename TBase>
class SequenceSizeFieldPrefix : public TBase
{
    using BaseImpl = TBase;
    using SizeField = TSizeField;

    static const std::size_t MaxAllowedSize =
            static_cast<std::size_t>(
                std::numeric_limits<typename SizeField::ValueType>::max());

    static_assert(!SizeField::isVersionDependent(),
            "Prefix fields must not be version dependent");

public:
    using ValueType = typename BaseImpl::ValueType;
    using ElementType = typename BaseImpl::ElementType;

    SequenceSizeFieldPrefix() = default;

    explicit SequenceSizeFieldPrefix(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit SequenceSizeFieldPrefix(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    SequenceSizeFieldPrefix(const SequenceSizeFieldPrefix&) = default;
    SequenceSizeFieldPrefix(SequenceSizeFieldPrefix&&) = default;
    SequenceSizeFieldPrefix& operator=(const SequenceSizeFieldPrefix&) = default;
    SequenceSizeFieldPrefix& operator=(SequenceSizeFieldPrefix&&) = default;

    std::size_t length() const
    {
        using SizeValueType = typename SizeField::ValueType;
        SizeField sizeField;
        sizeField.value() = static_cast<SizeValueType>(BaseImpl::value().size());
        return sizeField.length() + BaseImpl::length();
    }

    static constexpr std::size_t minLength()
    {
        return SizeField::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return SizeField::maxLength() + BaseImpl::maxLength();
    }

    bool valid() const
    {
        if ((!BaseImpl::valid()) || (!canWrite())) {
            return false;
        }
        using SizeValueType = typename SizeField::ValueType;
        SizeField sizeField;
        sizeField.value() = static_cast<SizeValueType>(BaseImpl::value().size());
        return sizeField.valid() && BaseImpl::valid();
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        auto fromIter = iter;
        SizeField sizeField;
        auto es = sizeField.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto diff = static_cast<std::size_t>(std::distance(fromIter, iter));
        COMMS_ASSERT(diff <= len);
        len -= diff;

        auto count = static_cast<std::size_t>(sizeField.value());
        return BaseImpl::readN(count, iter, len);
    }

    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        SizeField sizeField;
        sizeField.readNoStatus(iter);
        auto count = static_cast<std::size_t>(sizeField.value());
        BaseImpl::readNoStatusN(count, iter);
    }

    bool canWrite() const
    {
        if (!BaseImpl::canWrite()) {
            return false;
        }

        if (MaxAllowedSize < BaseImpl::value().size()) {
            return false;
        }

        SizeField sizeField;
        sizeField.value() = static_cast<typename SizeField::ValueType>(BaseImpl::value().size());
        return sizeField.canWrite();
    }

    template <typename TIter>
    comms::ErrorStatus write(TIter& iter, std::size_t len) const
    {
        if (!canWrite()) {
            return comms::ErrorStatus::InvalidMsgData;
        }

        using SizeValueType = typename SizeField::ValueType;
        SizeField sizeField;
        sizeField.value() = static_cast<SizeValueType>(BaseImpl::value().size());
        auto es = sizeField.write(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        COMMS_ASSERT(sizeField.length() <= len);
        return BaseImpl::write(iter, len - sizeField.length());
    }

    static constexpr bool hasWriteNoStatus()
    {
        return false;
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const = delete;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




