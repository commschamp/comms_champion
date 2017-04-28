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
class MsgFactoryGeneric : public MsgFactoryBinSearchBase<TMsgBase, TAllMessages, TOptions...>
{
    using Base = MsgFactoryBinSearchBase<TMsgBase, TAllMessages, TOptions...>;

public:
    using AllMessages = typename Base::AllMessages;
    using MsgPtr = typename Base::MsgPtr;
    using MsgIdParamType = typename Base::MsgIdParamType;
    using MsgIdType = typename Base::MsgIdType;

    MsgPtr createMsg(MsgIdParamType id, unsigned idx = 0) const
    {
        auto range =
            std::equal_range(
                Base::registry().begin(), Base::registry().end(), id,
                [](const CompWrapper& idWrapper1, const CompWrapper& idWrapper2) -> bool
                {
                    return idWrapper1.getId() < idWrapper2.getId();
                });

        auto dist = static_cast<unsigned>(std::distance(range.first, range.second));
        if (dist <= idx) {
            return MsgPtr();
        }

        auto iter = range.first + idx;
        GASSERT(*iter);
        return (*iter)->create(*this);
    }

    std::size_t msgCount(MsgIdParamType id) const
    {
        auto range =
            std::equal_range(
                Base::registry().begin(), Base::registry().end(), id,
                [](const CompWrapper& idWrapper1, const CompWrapper& idWrapper2) -> bool
                {
                    return idWrapper1.getId() < idWrapper2.getId();
                });

        return static_cast<std::size_t>(std::distance(range.first, range.second));
    }

private:

    using FactoryMethod = typename Base::FactoryMethod;

    class CompWrapper
    {
    public:

        CompWrapper(MsgIdParamType id)
          : m_id(id)
        {
        }

        CompWrapper(const FactoryMethod* method)
          : m_id(method->getId())
        {
        }


        MsgIdParamType getId() const
        {
            return m_id;
        }

    private:
        MsgIdType m_id;
    };
};

} // namespace details

} // namespace comms
