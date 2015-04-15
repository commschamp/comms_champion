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
public:
    typedef TUnderlyingType UnderlyingType;
    typedef std::vector<std::uint8_t> SerialisedSeq;

    virtual ~NumericValueWrapper() {}

    UnderlyingType value() const
    {
        return valueImpl();
    }

    void setValue(UnderlyingType value)
    {
        setValueImpl(value);
    }

    SerialisedSeq serialisedValue() const
    {
        return serialisedValueImpl();
    }

    void setSerialisedValue(const SerialisedSeq& value)
    {
        setSerialisedValueImpl(value);
    }

    QString serialisedString() const
    {
        auto seq = serialisedValue();
        QString str;
        for (auto& byte : seq) {
            str.append(QString("%1").arg((unsigned)byte, 2, 16, QChar('0')));
        }
        return str;
    }

    void setSerialisedString(const QString& str)
    {
        assert((str.size() & 0x1) == 0U);
        SerialisedSeq seq;
        seq.reserve(str.size() / 2);
        QString byteStr;
        for (auto ch : str) {
            byteStr.append(ch);
            if (byteStr.size() < 2) {
                continue;
            }

            bool ok = false;
            auto val = byteStr.toUInt(&ok, 16);
            if (ok) {
                seq.push_back(static_cast<SerialisedSeq::value_type>(val));
            }
            byteStr.clear();
        }

        setSerialisedValue(seq);
    }

    UnderlyingType minValue() const
    {
        return minValueImpl();
    }

    UnderlyingType maxValue() const
    {
        return maxValueImpl();
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
    virtual UnderlyingType valueImpl() const = 0;
    virtual SerialisedSeq serialisedValueImpl() const = 0;
    virtual void setValueImpl(UnderlyingType value) = 0;
    virtual void setSerialisedValueImpl(const SerialisedSeq& value) = 0;
    virtual UnderlyingType minValueImpl() const = 0;
    virtual UnderlyingType maxValueImpl() const = 0;
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

    virtual UnderlyingType valueImpl() const override
    {
        return static_cast<UnderlyingType>(Base::field().getValue());
    }

    virtual SerialisedSeq serialisedValueImpl() const override
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

    virtual void setValueImpl(UnderlyingType value) override
    {
        setValueImplInternal(value, UpdateTag());
    }

    virtual void setSerialisedValueImpl(const SerialisedSeq& value) override
    {
        auto iter = &value[0];
        auto& field = Base::field();
        auto es = field.read(iter, value.size());
        static_cast<void>(es);
        assert(es == comms::ErrorStatus::Success);
        assert(value.size() == field.length());
    }

    virtual UnderlyingType minValueImpl() const override
    {
        return minValueImplInternal(SerialisedTypeTag());
    }

    virtual UnderlyingType maxValueImpl() const override
    {
        return maxValueImplInternal(SerialisedTypeTag());
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
    struct Writable {};
    struct ReadOnly {};
    struct FixedLengthTag {};
    struct VarLengthTag {};


    using UpdateTag =
        typename std::conditional<
            std::is_const<TField>::value,
            ReadOnly,
            Writable
        >::type;

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


    void setValueImplInternal(UnderlyingType value, Writable)
    {
        Base::field().setValue(static_cast<ValueType>(value));
    }

    void setValueImplInternal(UnderlyingType value, ReadOnly)
    {
        static_cast<void>(value);
        assert(!"Attempt to update readonly field");
    }

    void setSerialisedValueImplInternal(UnderlyingType value, Writable)
    {
        Base::field().setSerialisedValue(static_cast<ValueType>(value));
    }

    void setSerialisedValueImplInternal(UnderlyingType value, ReadOnly)
    {
        static_cast<void>(value);
        assert(!"Attempt to update readonly field");
    }

    UnderlyingType minValueImplInternal(SerialisedUnsignedTag) const
    {
        Field fieldTmp;
        fieldTmp.setSerialisedValue(0);
        return fieldTmp.getValue();
    }

    UnderlyingType minValueImplInternal(SerialisedSignedTag) const
    {
        auto minSerialised = std::numeric_limits<SerialisedType>::min();
        if (Field::SerialisedLen < sizeof(SerialisedType)) {
            typedef typename std::make_unsigned<SerialisedType>::type UnsignedSerialisedType;
            static const auto numOfBits =
                Field::SerialisedLen * std::numeric_limits<std::uint8_t>::digits;
            minSerialised =
                static_cast<UnderlyingType>(
                    static_cast<UnsignedSerialisedType>(1) << (numOfBits - 1));
        }

        Field fieldTmp;
        fieldTmp.setSerialisedValue(0);
        return static_cast<UnderlyingType>(fieldTmp.getValue());
    }

    UnderlyingType maxValueImplInternal(SerialisedUnsignedTag) const
    {
        auto maxSerialised = std::numeric_limits<SerialisedType>::max();
        if (Base::field().length() < sizeof(SerialisedType)) {
            static const auto numOfBits =
                Base::field().length() * std::numeric_limits<std::uint8_t>::digits;
            maxSerialised = (static_cast<SerialisedType>(1) << numOfBits) - 1;
        }

        Field fieldTmp;
        fieldTmp.setSerialisedValue(maxSerialised);
        return static_cast<UnderlyingType>(fieldTmp.getValue());
    }

    UnderlyingType maxValueImplInternal(SerialisedSignedTag) const
    {
        auto maxSerialised = std::numeric_limits<SerialisedType>::max();
        if (Field::SerialisedLen < sizeof(SerialisedType)) {
            static const auto numOfBits =
                Field::SerialisedLen * std::numeric_limits<std::uint8_t>::digits;

            maxSerialised = (static_cast<SerialisedType>(1) << (Field::numOfBits - 1)) - 1;
        }

        Field fieldTmp;
        fieldTmp.setSerialisedValue(maxSerialised);
        return static_cast<UnderlyingType>(fieldTmp.getValue());
    }

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
