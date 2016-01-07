//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include <cstdint>
#include <cassert>
#include <memory>
#include <type_traits>

#include "comms/comms.h"

#include "FieldWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class ArrayListWrapper : public FieldWrapper
{
public:

    typedef std::vector<FieldWrapperPtr> Members;
    typedef std::unique_ptr<ArrayListWrapper> Ptr;

    virtual ~ArrayListWrapper() {}

    void addField()
    {
        addFieldImpl();
    }

    void removeField(int idx)
    {
        removeFieldImpl(idx);
    }

    unsigned size() const
    {
        return sizeImpl();
    }

    bool hasFixedSize() const
    {
        return hasFixedSizeImpl();
    }

    Members& getMembers()
    {
        return m_members;
    }

    const Members& getMembers() const
    {
        return m_members;
    }

    void setMembers(Members&& members)
    {
        m_members = std::move(members);
    }

    Ptr clone()
    {
        Members clonedMembers;
        clonedMembers.reserve(m_members.size());
        for (auto& mem : m_members) {
            clonedMembers.push_back(mem->upClone());
        }

        auto ptr = cloneImpl();
        ptr->setMembers(std::move(clonedMembers));
        assert(size() == ptr->size());
        assert(getMembers().size() == ptr->getMembers().size());
        return std::move(ptr);
    }

protected:
    virtual void addFieldImpl() = 0;
    virtual void removeFieldImpl(int idx) = 0;
    virtual unsigned sizeImpl() const = 0;
    virtual bool hasFixedSizeImpl() const = 0;
    virtual Ptr cloneImpl() = 0;

    void dispatchImpl(FieldWrapperHandler& handler);

private:
    Members m_members;
};

template <typename TField>
class ArrayListWrapperT : public FieldWrapperT<ArrayListWrapper, TField>
{
    using Base = FieldWrapperT<ArrayListWrapper, TField>;
    using Field = TField;
    using ValueType = typename Field::ValueType;
    using ElementType = typename ValueType::value_type;

public:
    using SerialisedSeq = typename Base::SerialisedSeq;
    using Ptr = typename Base::Ptr;

    typedef std::function<FieldWrapperPtr (ElementType&)> WrapFieldCallbackFunc;

    explicit ArrayListWrapperT(Field& fieldRef)
      : Base(fieldRef)
    {
    }

    ArrayListWrapperT(const ArrayListWrapperT&) = default;
    ArrayListWrapperT(ArrayListWrapperT&&) = default;
    virtual ~ArrayListWrapperT() = default;

    ArrayListWrapperT& operator=(const ArrayListWrapperT&) = delete;

    void setWrapFieldCallback(WrapFieldCallbackFunc&& func)
    {
        assert(func);
        m_wrapFieldFunc = std::move(func);
    }

protected:

    virtual void addFieldImpl() override
    {
        auto& col = Base::field().value();

        auto& mems = Base::getMembers();

        decltype(&col[0]) firstElemPtr = nullptr;
        if (!col.empty()) {
            firstElemPtr = &col[0];
        }

        col.push_back(ElementType());
        if (!m_wrapFieldFunc) {
            assert(!"The callback is expected to be set");
            mems.clear();
            return;
        }

        if (firstElemPtr == &col[0]) {
            mems.push_back(m_wrapFieldFunc(col.back()));
            assert(col.size() == mems.size());
            return;
        }

        mems.clear();
        mems.reserve(col.size());
        for (auto& f : col) {
            mems.push_back(m_wrapFieldFunc(f));
        }

        assert(col.size() == mems.size());
    }

    virtual void removeFieldImpl(int idx) override
    {
        auto& storage = Base::field().value();
        if (static_cast<decltype(idx)>(storage.size()) <= idx) {
            return;
        }

        storage.erase(storage.begin() + idx);
        auto& mems = Base::getMembers();
        mems.clear();
        mems.reserve(storage.size());
        for (auto& f : storage) {
            mems.push_back(m_wrapFieldFunc(f));
        }
    }

    virtual bool setSerialisedValueImpl(const SerialisedSeq& value) override
    {
        static_cast<void>(value);
        assert(!"Mustn't be called");
        return false;
    }

    virtual unsigned sizeImpl() const override
    {
        auto val = Base::field().value().size();
        assert(val == Base::getMembers().size());
        return val;
    }

    virtual bool hasFixedSizeImpl() const override
    {
        return Field::ParsedOptions::HasSequenceFixedSize;
    }

    virtual Ptr cloneImpl() override
    {
        std::unique_ptr<ArrayListWrapperT<TField> > ptr(new ArrayListWrapperT(Base::field()));
        ptr->m_wrapFieldFunc = m_wrapFieldFunc;
        return std::move(ptr);
    }

private:
    WrapFieldCallbackFunc m_wrapFieldFunc;
};

using ArrayListWrapperPtr = ArrayListWrapper::Ptr;

template <typename TField>
ArrayListWrapperPtr
makeArrayListWrapper(TField& field)
{
    return
        ArrayListWrapperPtr(
            new ArrayListWrapperT<TField>(field));
}

template <typename TField>
std::unique_ptr<ArrayListWrapperT<TField> >
makeDowncastedArrayListWrapper(TField& field)
{
    return
        std::unique_ptr<ArrayListWrapperT<TField> >(
            new ArrayListWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion



