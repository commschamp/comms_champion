//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
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

#include "MsgFactoryBinSearchBase.h"

namespace comms
{

namespace details
{

template <typename TMsgBase, typename TAllMessages, typename... TOptions>
class MsgFactoryUniq : public MsgFactoryBinSearchBase<TMsgBase, TAllMessages, TOptions...>
{
    using BaseImpl = MsgFactoryBinSearchBase<TMsgBase, TAllMessages, TOptions...>;

public:
    using AllMessages = typename BaseImpl::AllMessages;
    using MsgPtr = typename BaseImpl::MsgPtr;
    using MsgIdParamType = typename BaseImpl::MsgIdParamType;
    using MsgIdType = typename BaseImpl::MsgIdType;

    MsgPtr createMsg(MsgIdParamType id, unsigned idx = 0) const
    {
        if (0 < idx) {
            return MsgPtr();
        }

        auto iter = findMethod(id);
        if (iter == BaseImpl::registry().end()) {
            return MsgPtr();
        }

        COMMS_ASSERT(*iter != nullptr);
        if((*iter)->getId() != id) {
            return MsgPtr();
        }

        return (*iter)->create(*this);
    }

    std::size_t msgCount(MsgIdParamType id) const
    {
        auto iter = findMethod(id);

        if (iter == BaseImpl::registry().end()) {
            return 0U;
        }

        COMMS_ASSERT(*iter != nullptr);
        if((*iter)->getId() != id) {
            return 0U;
        }

        return 1U;
    }

    static constexpr bool hasUniqueIds()
    {
        return true;
    }

private:

    using FactoryMethod = typename BaseImpl::FactoryMethod;
    using MethodsRegistry = typename BaseImpl::MethodsRegistry;

    typename MethodsRegistry::const_iterator findMethod(MsgIdParamType id) const
    {
        return std::lower_bound(
            BaseImpl::registry().begin(), BaseImpl::registry().end(), id,
            [](const FactoryMethod* method, MsgIdParamType idParam) -> bool
            {
                return method->getId() < idParam;
            });
    }
};

} // namespace details

} // namespace comms
