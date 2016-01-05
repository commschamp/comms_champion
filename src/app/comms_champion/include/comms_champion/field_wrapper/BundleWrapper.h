//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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
#include <vector>

#include "comms/field/Bundle.h"
#include "FieldWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class BundleWrapper : public FieldWrapper
{
    using Base = FieldWrapper;
public:
    typedef std::vector<FieldWrapperPtr> Members;

    using Base::FieldWrapper;
    typedef unsigned long long UnderlyingType;

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

private:
    Members m_members;
};

template <typename TField>
class BundleWrapperT : public FieldWrapperT<BundleWrapper, TField>
{
    using Base = FieldWrapperT<BundleWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isBundle<Field>(), "Must be of Bundle field type");

    using UnderlyingType = typename Base::UnderlyingType;
public:
    explicit BundleWrapperT(Field& fieldRef)
      : Base(fieldRef)
    {
    }

    BundleWrapperT(const BundleWrapperT&) = default;
    BundleWrapperT(BundleWrapperT&&) = default;
    virtual ~BundleWrapperT() = default;

    BundleWrapperT& operator=(const BundleWrapperT&) = delete;

};

using BundleWrapperPtr = std::unique_ptr<BundleWrapper>;

template <typename TField>
BundleWrapperPtr
makeBundleWrapper(TField& field)
{
    return
        BundleWrapperPtr(
            new BundleWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
