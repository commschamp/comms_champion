//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
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
#include <cassert>
#include <memory>

#include "comms/field/Variant.h"
#include "FieldWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class CC_API VariantWrapper : public FieldWrapper
{
    using Base = FieldWrapper;
public:
    using Base::FieldWrapper;
    typedef std::unique_ptr<VariantWrapper> Ptr;

    using MemberCreateCallbackFunc = std::function<FieldWrapperPtr ()>;

    VariantWrapper();
    VariantWrapper(const VariantWrapper&) =delete;
    VariantWrapper& operator=(const VariantWrapper&) =delete;

    virtual ~VariantWrapper();

    FieldWrapperPtr& getCurrent();

    const FieldWrapperPtr& getCurrent() const;

    void setCurrent(FieldWrapperPtr current);

    void updateCurrent();

    Ptr clone();

    int getCurrentIndex() const;

    void setCurrentIndex(int index);

    int getMembersCount() const;

    template <typename TFunc>
    void setMemberCreateCallback(TFunc&& func)
    {
        m_createMemberCb = std::forward<TFunc>(func);
    }

protected:
    virtual Ptr cloneImpl() = 0;

    virtual void dispatchImpl(FieldWrapperHandler& handler);
    virtual int getCurrentIndexImpl() const = 0;
    virtual void setCurrentIndexImpl(int index) = 0;
    virtual int getMembersCountImpl() const = 0;

private:
    FieldWrapperPtr m_current;
    MemberCreateCallbackFunc m_createMemberCb;
};

template <typename TField>
class VariantWrapperT : public FieldWrapperT<VariantWrapper, TField>
{
    using Base = FieldWrapperT<VariantWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isVariant<Field>(), "Must be of Variant field type");

public:
    typedef typename Base::Ptr Ptr;

    explicit VariantWrapperT(Field& fieldRef)
      : Base(fieldRef)
    {
    }

    VariantWrapperT(const VariantWrapperT&) = default;
    VariantWrapperT(VariantWrapperT&&) = default;
    virtual ~VariantWrapperT() = default;

    VariantWrapperT& operator=(const VariantWrapperT&) = delete;

protected:
    virtual Ptr cloneImpl() override
    {
        return Ptr(new VariantWrapperT(Base::field()));
    }

    virtual int getCurrentIndexImpl() const
    {
        if (!Base::field().currentFieldValid()) {
            return -1;
        }

        return static_cast<int>(Base::field().currentField());
    }

    virtual void setCurrentIndexImpl(int index) override
    {
        if (index < 0) {
            Base::field().reset();
            return;
        }

        Base::field().selectField(static_cast<std::size_t>(index));
    }

    virtual int getMembersCountImpl() const override
    {
        return
            static_cast<int>(
                std::tuple_size<typename Base::Field::Members>::value);
    }

};

using VariantWrapperPtr = VariantWrapper::Ptr;

template <typename TField>
VariantWrapperPtr
makeVariantWrapper(TField& field)
{
    return
        VariantWrapperPtr(
            new VariantWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
