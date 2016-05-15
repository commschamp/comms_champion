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

namespace comms
{

namespace details
{

template <typename TMsgBase, typename TAllMessages, typename... TOptions>
class MsgFactoryBase;

template <typename TMsgBase, typename TAllMessages>
class MsgFactoryBase<TMsgBase, TAllMessages>
{
public:
    typedef TMsgBase Message;
    typedef std::unique_ptr<Message> MsgPtr;
    typedef TAllMessages AllMessages;

protected:
    MsgFactoryBase() = default;

    template <typename TObj, typename... TArgs>
    MsgPtr allocMsg(TArgs&&... args) const
    {
        static_assert(std::is_base_of<Message, TObj>::value,
            "TObj is not a proper message type");
        return MsgPtr(new TObj(std::forward<TArgs>(args)...));
    }
};

template <typename TMsgBase, typename TAllMessages>
class MsgFactoryBase<TMsgBase, TAllMessages, comms::option::InPlaceAllocation>
{
public:

    template <typename T>
    class Deleter
    {
        template<typename U>
        friend class Deleter;

    public:
        Deleter(bool* allocated = nullptr)
            : allocated_(allocated)
        {
        }

        Deleter(const Deleter& other) = delete;

        template <typename U>
        Deleter(Deleter<U>&& other)
            : allocated_(other.allocated_)
        {
            static_assert(std::is_base_of<T, U>::value ||
                          std::is_base_of<U, T>::value ||
                          std::is_convertible<U, T>::value ||
                          std::is_convertible<T, U>::value ,
                "To make Deleter convertible, their template parameters "
                "must be convertible.");

            other.allocated_ = nullptr;
        }

        ~Deleter()
        {
            GASSERT(allocated_ == nullptr);
        }

        Deleter& operator=(const Deleter& other) = delete;

        template <typename U>
        Deleter& operator=(Deleter<U>&& other)
        {
            static_assert(std::is_base_of<T, U>::value ||
                          std::is_base_of<U, T>::value ||
                          std::is_convertible<U, T>::value ||
                          std::is_convertible<T, U>::value ,
                "To make Deleter convertible, their template parameters "
                "must be convertible.");

            if (reinterpret_cast<void*>(this) == reinterpret_cast<const void*>(&other)) {
                return *this;
            }

            GASSERT(allocated_ == nullptr);
            allocated_ = other.allocated_;
            other.allocated_ = nullptr;
            return *this;
        }

        void operator()(T* obj) {
            GASSERT(allocated_ != nullptr);
            GASSERT(*allocated_);
            obj->~T();
            *allocated_ = false;
            allocated_ = nullptr;
        }

    private:
        bool* allocated_;
    };

    typedef TMsgBase Message;
    typedef std::unique_ptr<Message, Deleter<Message> > MsgPtr;
    typedef TAllMessages AllMessages;

    MsgFactoryBase(const MsgFactoryBase&) = delete;
    MsgFactoryBase& operator=(const MsgFactoryBase&) = delete;

protected:
    MsgFactoryBase() = default;

    template <typename TObj, typename... TArgs>
    MsgPtr allocMsg(TArgs&&... args) const
    {
        if (allocated_) {
            return MsgPtr();
        }

        static_assert(std::is_base_of<Message, TObj>::value,
            "TObj is not a proper message type");

        static_assert(comms::util::IsInTuple<TObj, TAllMessages>::Value, ""
            "TObj must be in provided tuple of supported messages");

        static_assert(sizeof(TObj) <= sizeof(place_), "Object is too big");

        new (&place_) TObj(std::forward<TArgs>(args)...);
        MsgPtr msg(
            reinterpret_cast<Message*>(&place_),
            Deleter<Message>(&allocated_));
        allocated_ = true;
        return std::move(msg);
    }

private:
    typedef typename comms::util::TupleAsAlignedUnion<TAllMessages>::Type AlignedStorage;

    mutable AlignedStorage place_;
    mutable bool allocated_ = false;
};

template <typename TMsgBase, typename TAllMessages, typename... TBundledOptions, typename... TOtherOptions>
class MsgFactoryBase<TMsgBase, TAllMessages, std::tuple<TBundledOptions...>, TOtherOptions...> :
    public MsgFactoryBase<TMsgBase, TAllMessages, TBundledOptions..., TOtherOptions...>
{
};

}  // namespace details

}  // namespace comms


