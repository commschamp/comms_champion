//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/field_wrapper/BundleWrapper.h"

#include "comms_champion/field_wrapper/FieldWrapperHandler.h"

namespace comms_champion
{

namespace field_wrapper
{

BundleWrapper::BundleWrapper() {}

BundleWrapper::~BundleWrapper() {}

BundleWrapper::Members& BundleWrapper::getMembers()
{
    return m_members;
}

const BundleWrapper::Members& BundleWrapper::getMembers() const
{
    return m_members;
}

void BundleWrapper::setMembers(Members&& members)
{
    m_members = std::move(members);
} 

BundleWrapper::Ptr BundleWrapper::clone()
{
    Members clonedMembers;
    clonedMembers.reserve(m_members.size());
    for (auto& mem : m_members) {
        clonedMembers.push_back(mem->upClone());
    }

    auto ptr = cloneImpl();
    ptr->setMembers(std::move(clonedMembers));
    return ptr;
}

void BundleWrapper::dispatchImpl(FieldWrapperHandler& handler)
{
    handler.handle(*this);
}

}  // namespace field_wrapper

}  // namespace comms_champion


