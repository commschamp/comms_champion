//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include <cstdint>
#include <cassert>
#include <memory>
#include <type_traits>
#include <limits>
#include <algorithm>
#include <iterator>

#include "FieldWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

template <typename TUnderlyingType>
class NumericValueWrapper : public FieldWrapper
{
    typedef FieldWrapper Base;
public:
    typedef TUnderlyingType UnderlyingType;
    typedef Base::SerialisedSeq SerialisedSeq;

    virtual ~NumericValueWrapper() {}

    UnderlyingType getValue() const
    {
        return getValueImpl();
    }

    void setValue(UnderlyingType value)
    {
        setValueImpl(value);
    }

    std::size_t minLength() const
    {
        return minLengthImpl();
    }

    std::size_t maxLength() const
    {
        return maxLengthImpl();
    }

    int minWidth() const
    {
        return static_cast<int>(minLength()) * 2;
    }

    int maxWidth() const
    {
        return static_cast<int>(maxLength()) * 2;
    }

private:
    virtual UnderlyingType getValueImpl() const = 0;
    virtual void setValueImpl(UnderlyingType value) = 0;
    virtual std::size_t minLengthImpl() const = 0;
    virtual std::size_t maxLengthImpl() const = 0;
};

template <typename TBase, typename TField>
class NumericValueWrapperT : public FieldWrapperT<TBase, TField>
{
    using Base = FieldWrapperT<TBase, TField>;

public:
    using UnderlyingType = typename Base::UnderlyingType;
    using SerialisedSeq = typename Base::SerialisedSeq;

protected:
    using Field = TField;
    using ValueType = typename Field::ValueType;
    using SerialisedType = typename Field::SerialisedType;

    static_assert(sizeof(ValueType) <= sizeof(UnderlyingType), "This wrapper cannot handle provided field.");
    static_assert(std::is_signed<ValueType>::value || (sizeof(ValueType) < sizeof(UnderlyingType)),
        "This wrapper cannot handle provided field.");

public:
    NumericValueWrapperT(Field& field)
      : Base(field)
    {
        static_assert(std::is_base_of<NumericValueWrapper<UnderlyingType>, NumericValueWrapperT<TBase, TField> >::value,
            "Must inherit from NumericValueWrapper");
    }

    NumericValueWrapperT(const NumericValueWrapperT&) = default;
    NumericValueWrapperT(NumericValueWrapperT&&) = default;
    virtual ~NumericValueWrapperT() = default;

    NumericValueWrapperT& operator=(const NumericValueWrapperT&) = delete;

protected:

    virtual UnderlyingType getValueImpl() const override
    {
        return static_cast<UnderlyingType>(Base::field().getValue());
    }

    virtual void setValueImpl(UnderlyingType value) override
    {
        Base::field().setValue(static_cast<ValueType>(value));
    }

    virtual SerialisedSeq getSerialisedValueImpl() const override
    {
        SerialisedSeq seq;
        auto& field = Base::field();
        seq.reserve(field.length());
        auto iter = std::back_inserter(seq);
        auto es = field.write(iter, seq.max_size());
        static_cast<void>(es);
        assert(es == comms::ErrorStatus::Success);
        assert(seq.size() == field.length());
        return seq;
    }

    virtual bool setSerialisedValueImpl(const SerialisedSeq& value) override
    {
        auto iter = &value[0];
        auto& field = Base::field();
        auto es = field.read(iter, value.size());
        return es == comms::ErrorStatus::Success;
    }

    virtual std::size_t minLengthImpl() const override
    {
        return minLengthInternal(LengthTag());
    }

    virtual std::size_t maxLengthImpl() const override
    {
        return maxLengthInternal(LengthTag());
    }

private:
    struct FixedLengthTag {};
    struct VarLengthTag {};

    using LengthTag =
        typename std::conditional<
            Field::hasFixedLength(),
            FixedLengthTag,
            VarLengthTag
        >::type;

    struct SerialisedSignedTag {};
    struct SerialisedUnsignedTag {};

    using SerialisedTypeTag =
        typename std::conditional<
            std::is_signed<SerialisedType>::value,
            SerialisedSignedTag,
            SerialisedUnsignedTag
        >::type;


    std::size_t minLengthInternal(FixedLengthTag) const
    {
        return Base::field().length();
    }

    std::size_t minLengthInternal(VarLengthTag) const
    {
        return Base::field().minLength();
    }

    std::size_t maxLengthInternal(FixedLengthTag) const
    {
        return Base::field().length();
    }

    std::size_t maxLengthInternal(VarLengthTag) const
    {
        return Base::field().maxLength();
    }
};

}  // namespace field_wrapper

}  // namespace comms_champion
