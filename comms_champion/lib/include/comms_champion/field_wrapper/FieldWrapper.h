//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <memory>
#include <vector>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

#include "comms/comms.h"
#include "comms_champion/Api.h"

namespace comms_champion
{

namespace field_wrapper
{

class FieldWrapperHandler;
class CC_API FieldWrapper
{
public:
    typedef std::vector<std::uint8_t> SerialisedSeq;

    typedef std::unique_ptr<FieldWrapper> BasePtr;

    FieldWrapper();
    virtual ~FieldWrapper();

    std::size_t length() const;

    int width() const;

    bool valid() const;

    SerialisedSeq getSerialisedValue() const;

    bool setSerialisedValue(const SerialisedSeq& value);

    QString getSerialisedString() const;

    bool setSerialisedString(const QString& str);

    void dispatch(FieldWrapperHandler& handler);

    BasePtr upClone();

protected:
    virtual std::size_t lengthImpl() const = 0;
    virtual bool validImpl() const = 0;
    virtual SerialisedSeq getSerialisedValueImpl() const = 0;
    virtual bool setSerialisedValueImpl(const SerialisedSeq& value) = 0;
    virtual void dispatchImpl(FieldWrapperHandler& handler) = 0;
    virtual BasePtr upCloneImpl() = 0;
};

template <typename TBase, typename TField>
class FieldWrapperT : public TBase
{
    using Base = TBase;

    struct HasPrefixSuffixTag {};
    struct NoPrefixSuffixTag {};

public:
    typedef typename Base::SerialisedSeq SerialisedSeq;
    typedef typename Base::BasePtr BasePtr;

    virtual ~FieldWrapperT() = default;

protected:

    using Field = TField;

    explicit FieldWrapperT(Field& fieldRef)
      : m_field(fieldRef)
    {
    }

    virtual std::size_t lengthImpl() const override
    {
        return m_field.length();
    }

    virtual bool validImpl() const override
    {
        return m_field.valid();
    }

    Field& field()
    {
        return m_field;
    }

    const Field& field() const
    {
        return m_field;
    }

    virtual SerialisedSeq getSerialisedValueImpl() const override
    {
        SerialisedSeq seq;
        seq.reserve(m_field.length());
        auto iter = std::back_inserter(seq);
        auto es = m_field.write(iter, seq.max_size());
        static_cast<void>(es);
        assert(es == comms::ErrorStatus::Success);
        assert(seq.size() == m_field.length());
        return seq;
    }

    virtual bool setSerialisedValueImpl(const SerialisedSeq& value) override
    {
        if (value.empty()) {
            return false;
        }

        if ((!Field::ParsedOptions::HasSequenceSizeFieldPrefix) &&
            (!Field::ParsedOptions::HasSequenceTrailingFieldSuffix) &&
            (!Field::ParsedOptions::HasSequenceTerminationFieldSuffix)){
            auto iter = &value[0];
            auto es = m_field.read(iter, value.size());
            return es == comms::ErrorStatus::Success;
        }

        SerialisedSeq newVal;
        if (!writeSerialisedSize(newVal, value.size(), SerialisedSizePrefixTag())) {
            return false;
        }

        auto writeIter = std::back_inserter(newVal);
        std::copy(value.begin(), value.end(), writeIter);

        if (!writeTrailSuffix(newVal, SerialisedTrailSuffixTag())) {
            return false;
        }

        if (!writeTermSuffix(newVal, SerialisedTermSuffixTag())) {
            return false;
        }

        auto iter = &newVal[0];
        auto es = m_field.read(iter, newVal.size());
        return es == comms::ErrorStatus::Success;
    }

    virtual BasePtr upCloneImpl() override
    {
        return static_cast<Base*>(this)->clone();
    }

private:
    typedef typename std::conditional<
        Field::ParsedOptions::HasSequenceSizeFieldPrefix,
        HasPrefixSuffixTag,
        NoPrefixSuffixTag
    >::type SerialisedSizePrefixTag;

    typedef typename std::conditional<
        Field::ParsedOptions::HasSequenceTrailingFieldSuffix,
        HasPrefixSuffixTag,
        NoPrefixSuffixTag
    >::type SerialisedTrailSuffixTag;

    typedef typename std::conditional<
        Field::ParsedOptions::HasSequenceTerminationFieldSuffix,
        HasPrefixSuffixTag,
        NoPrefixSuffixTag
    >::type SerialisedTermSuffixTag;

    bool writeSerialisedSize(SerialisedSeq& seq, std::size_t sizeVal, HasPrefixSuffixTag)
    {
        typedef typename Field::ParsedOptions::SequenceSizeFieldPrefix SizePrefixField;

        SizePrefixField sizePrefixField(sizeVal);
        auto writeIter = std::back_inserter(seq);
        auto es = sizePrefixField.write(writeIter, seq.max_size() - seq.size());
        return es == comms::ErrorStatus::Success;
    }

    bool writeSerialisedSize(SerialisedSeq& seq, std::size_t sizeVal, NoPrefixSuffixTag)
    {
        static_cast<void>(seq);
        static_cast<void>(sizeVal);
        return true;
    }

    bool writeTrailSuffix(SerialisedSeq& seq, HasPrefixSuffixTag)
    {
        typedef typename Field::ParsedOptions::SequenceTrailingFieldSuffix TrailingSuffixField;
        TrailingSuffixField trailingSuffixField;
        auto writeIter = std::back_inserter(seq);
        auto es = trailingSuffixField.write(writeIter, seq.max_size() - seq.size());
        return es == comms::ErrorStatus::Success;
    }

    bool writeTrailSuffix(SerialisedSeq& seq, NoPrefixSuffixTag)
    {
        static_cast<void>(seq);
        return true;
    }

    bool writeTermSuffix(SerialisedSeq& seq, HasPrefixSuffixTag)
    {
        typedef typename Field::ParsedOptions::SequenceTerminationFieldSuffix TermSuffixField;
        TermSuffixField termSuffixField;
        auto writeIter = std::back_inserter(seq);
        auto es = termSuffixField.write(writeIter, seq.max_size() - seq.size());
        return es == comms::ErrorStatus::Success;
    }

    bool writeTermSuffix(SerialisedSeq& seq, NoPrefixSuffixTag)
    {
        static_cast<void>(seq);
        return true;
    }


    Field& m_field;
};

typedef FieldWrapper::BasePtr FieldWrapperPtr;

}  // namespace field_wrapper

}  // namespace comms_champion
