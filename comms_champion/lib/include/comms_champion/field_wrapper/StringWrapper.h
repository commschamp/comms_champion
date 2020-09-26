//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include <cstdint>
#include <cassert>
#include <memory>
#include <limits>

#include "comms/comms.h"

#include "FieldWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class CC_API StringWrapper : public FieldWrapper
{
public:

    typedef std::unique_ptr<StringWrapper> Ptr;

    StringWrapper();
    virtual ~StringWrapper() noexcept;

    QString getValue() const;

    void setValue(const QString& val);

    int maxSize() const;

    Ptr clone();

protected:
    virtual QString getValueImpl() const = 0;
    virtual void setValueImpl(const QString& val) = 0;
    virtual int maxSizeImpl() const = 0;
    virtual Ptr cloneImpl() = 0;

    void dispatchImpl(FieldWrapperHandler& handler);
};

template <typename TField>
class StringWrapperT : public FieldWrapperT<StringWrapper, TField>
{
    using Base = FieldWrapperT<StringWrapper, TField>;
    using Field = TField;

public:
    using SerialisedSeq = typename Base::SerialisedSeq;
    using Ptr = typename Base::Ptr;

    explicit StringWrapperT(Field& fieldRef)
      : Base(fieldRef)
    {
    }

    StringWrapperT(const StringWrapperT&) = default;
    StringWrapperT(StringWrapperT&&) = default;
    virtual ~StringWrapperT() noexcept = default;

    StringWrapperT& operator=(const StringWrapperT&) = delete;

protected:

    virtual QString getValueImpl() const override
    {
        auto& strField = Base::field();
        return QString::fromUtf8(strField.value().c_str(), static_cast<int>(strField.value().size()));
    }

    virtual void setValueImpl(const QString& val) override
    {
        Base::field().value() = val.toStdString().c_str();
    }

    virtual bool setSerialisedValueImpl(const SerialisedSeq& value) override
    {
        static_cast<void>(value);
        static constexpr bool Must_not_be_called = false;
        static_cast<void>(Must_not_be_called);
        assert(Must_not_be_called); 
        return false;
    }

    virtual int maxSizeImpl() const override
    {
        return maxSizeInternal(SizeExistanceTag());
    }

    virtual Ptr cloneImpl() override
    {
        return Ptr(new StringWrapperT<TField>(Base::field()));
    }

private:
    struct SizeFieldExistsTag {};
    struct SerLengthFieldExistsTag {};
    struct NoSizeFieldTag {};

    typedef typename Field::ParsedOptions FieldOptions;
    typedef typename std::conditional<
        FieldOptions::HasSequenceSizeFieldPrefix,
        SizeFieldExistsTag,
        typename std::conditional<
            FieldOptions::HasSequenceSerLengthFieldPrefix,
            SerLengthFieldExistsTag,
            NoSizeFieldTag
        >::type
    >::type SizeExistanceTag;

    template <typename TPrefixField>
    static int maxSizeByPrefix()
    {
        if (sizeof(int) <= TPrefixField::maxLength()) {
            return std::numeric_limits<int>::max();
        }

        auto shift =
            TPrefixField::maxLength() * std::numeric_limits<std::uint8_t>::digits;

        return static_cast<int>((1U << shift) - 1);
    }

    static int maxSizeInternal(SizeFieldExistsTag)
    {
        typedef typename FieldOptions::SequenceSizeFieldPrefix SizeField;
        return maxSizeByPrefix<SizeField>();
    }

    static int maxSizeInternal(SerLengthFieldExistsTag)
    {
        typedef typename FieldOptions::SequenceSerLengthFieldPrefix LengthField;
        return maxSizeByPrefix<LengthField>();
    }

    int maxSizeInternal(NoSizeFieldTag) const
    {
        return
            static_cast<int>(
                std::min(
                    static_cast<std::size_t>(std::numeric_limits<int>::max()),
                    Base::field().value().max_size()));
    }
};

using StringWrapperPtr = StringWrapper::Ptr;

template <typename TField>
StringWrapperPtr
makeStringWrapper(TField& field)
{
    return
        StringWrapperPtr(
            new StringWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion



