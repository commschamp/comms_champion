//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

#include <type_traits>
#include <memory>

#include "comms/options.h"
#include "comms/Assert.h"
#include "comms/util/Tuple.h"
#include "comms/util/alloc.h"

namespace comms
{

namespace details
{

template <typename TMsgBase, typename TAllMessages, typename... TOptions>
class MsgFactoryBase;

template <typename TMsgBase, typename TAllMessages>
class MsgFactoryBase<TMsgBase, TAllMessages>
{
    typedef util::alloc::DynMemory<TMsgBase> Alloc;
public:
    typedef TMsgBase Message;
    typedef typename Alloc::Ptr MsgPtr;
    typedef TAllMessages AllMessages;

protected:
    MsgFactoryBase() = default;

    template <typename TObj, typename... TArgs>
    MsgPtr allocMsg(TArgs&&... args) const
    {
        static_assert(std::is_base_of<Message, TObj>::value,
            "TObj is not a proper message type");
        return alloc_.template alloc<TObj>(std::forward<TArgs>(args)...);
    }

private:
    mutable util::alloc::DynMemory<TMsgBase> alloc_;
};

template <typename TMsgBase, typename TAllMessages>
class MsgFactoryBase<TMsgBase, TAllMessages, comms::option::InPlaceAllocation>
{
    typedef util::alloc::InPlaceSingle<TMsgBase, TAllMessages> Alloc;
public:
    typedef TMsgBase Message;
    typedef typename Alloc::Ptr MsgPtr;
    typedef TAllMessages AllMessages;

    MsgFactoryBase(const MsgFactoryBase&) = delete;
    MsgFactoryBase& operator=(const MsgFactoryBase&) = delete;

protected:
    MsgFactoryBase() = default;

    template <typename TObj, typename... TArgs>
    MsgPtr allocMsg(TArgs&&... args) const
    {
        static_assert(std::is_base_of<Message, TObj>::value,
            "TObj is not a proper message type");

        static_assert(comms::util::IsInTuple<TObj, TAllMessages>::Value, ""
            "TObj must be in provided tuple of supported messages");

        return alloc_.template alloc<TObj>(std::forward<TArgs>(args)...);
    }

private:
    mutable Alloc alloc_;
};

template <typename TMsgBase, typename TAllMessages, typename... TBundledOptions, typename... TOtherOptions>
class MsgFactoryBase<TMsgBase, TAllMessages, std::tuple<TBundledOptions...>, TOtherOptions...> :
    public MsgFactoryBase<TMsgBase, TAllMessages, TBundledOptions..., TOtherOptions...>
{
};

}  // namespace details

}  // namespace comms


