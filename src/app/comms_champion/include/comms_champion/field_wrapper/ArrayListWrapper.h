//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

protected:
    virtual void addFieldImpl() = 0;
    virtual void removeFieldImpl(int idx) = 0;
    virtual unsigned sizeImpl() const = 0;
};

template <typename TField>
class ArrayListWrapperT : public FieldWrapperT<ArrayListWrapper, TField>
{
    using Base = FieldWrapperT<ArrayListWrapper, TField>;
    using Field = TField;

public:
    using SerialisedSeq = typename Base::SerialisedSeq;

    ArrayListWrapperT(Field& field)
      : Base(field)
    {
    }

    ArrayListWrapperT(const ArrayListWrapperT&) = default;
    ArrayListWrapperT(ArrayListWrapperT&&) = default;
    virtual ~ArrayListWrapperT() = default;

    ArrayListWrapperT& operator=(const ArrayListWrapperT&) = delete;

protected:

    virtual void addFieldImpl() override
    {
        auto& col = Base::field().value();

        typedef typename std::decay<decltype(col)>::type CollectionType;
        typedef typename CollectionType::value_type ElementType;

        col.push_back(ElementType());
    }

    virtual void removeFieldImpl(int idx) override
    {
        auto& storage = Base::field().value();
        if (static_cast<decltype(idx)>(storage.size()) <= idx) {
            return;
        }

        storage.erase(storage.begin() + idx);
    }

    virtual bool setSerialisedValueImpl(const SerialisedSeq& value) override
    {
        static_cast<void>(value);
        assert(!"Mustn't be called");
        return false;
    }

    virtual unsigned sizeImpl() const
    {
        return Base::field().value().size();
    }
};

using ArrayListWrapperPtr = std::unique_ptr<ArrayListWrapper>;

template <typename TField>
ArrayListWrapperPtr
makeArrayListWrapper(TField& field)
{
    return
        ArrayListWrapperPtr(
            new ArrayListWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion



