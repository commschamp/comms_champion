//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include <memory>
#include "comms/protocol/ProtocolLayerBase.h"
#include "comms/protocol/options.h"
#include "comms/util/Tuple.h"

namespace comms
{

namespace protocol
{

namespace details
{

template <typename TField,
          typename TAllMessages,
          typename TNextLayer,
          typename... TOptions>
class MsgIdLayerBase;

template <typename TField,
          typename TAllMessages,
          typename TNextLayer>
class MsgIdLayerBase<TField, TAllMessages, TNextLayer> :
        public comms::protocol::ProtocolLayerBase<TField, TNextLayer, MsgIdLayerBase<TField, TAllMessages, TNextLayer> >
{
    typedef comms::protocol::ProtocolLayerBase<
        TField,
        TNextLayer,
        MsgIdLayerBase<TField, TAllMessages, TNextLayer>
    > Base;
public:

    typedef std::unique_ptr<typename Base::Message> MsgPtr;
    typedef TAllMessages AllMessages;

protected:

    using Base::ProtocolLayerBase;

    template <typename TObj, typename... TArgs>
    MsgPtr allocMsg(TArgs&&... args)
    {
        static_assert(std::is_base_of<typename Base::Message, TObj>::value,
            "TObj is not a proper message type");
        return MsgPtr(new TObj(std::forward<TArgs>(args)...));
    }
};

template <typename TField,
          typename TAllMessages,
          typename TNextLayer>
class MsgIdLayerBase<
    TField, TAllMessages, TNextLayer, comms::protocol::option::InPlaceAllocation> :
    public comms::protocol::ProtocolLayerBase<TField, TNextLayer, MsgIdLayerBase<TField, TAllMessages, TNextLayer, comms::protocol::option::InPlaceAllocation> >
{
    typedef comms::protocol::ProtocolLayerBase<
        TField,
        TNextLayer,
        MsgIdLayerBase<TField, TAllMessages, TNextLayer, comms::protocol::option::InPlaceAllocation> > Base;
public:

    /// @cond DOCUMENT_ASSERT_MANAGER

    /// @brief Deleter class
    template <typename T>
    class Deleter
    {
        template<typename U>
        friend class Deleter;

    public:
        /// Constructor used by BasicInPlaceAllocator to create std::unique_ptr
        Deleter(bool* allocated = nullptr)
            : allocated_(allocated)
        {
        }

        /// Copy constructor is deleted
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

        /// Copy assignment is deleted
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

        /// @brief Deletion operator
        /// @details Executes destructor of the deleted object
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
    /// @endcond

    typedef std::unique_ptr<typename Base::Message, Deleter<typename Base::Message> > MsgPtr;
    typedef TAllMessages AllMessages;

protected:

    MsgIdLayerBase() = default;
    MsgIdLayerBase(const MsgIdLayerBase&) = delete;
    MsgIdLayerBase& operator=(const MsgIdLayerBase&) = delete;

    template <typename TObj, typename... TArgs>
    MsgPtr allocMsg(TArgs&&... args)
    {
        if (allocated_) {
            return MsgPtr();
        }

        static_assert(std::is_base_of<typename Base::Message, TObj>::value,
            "TObj is not a proper message type");

        static_assert(comms::util::IsInTuple<TObj, TAllMessages>::Value, ""
            "TObj must be in provided tuple of supported messages");

        static_assert(sizeof(TObj) <= sizeof(place_), "Object is too big");

        MsgPtr msg(
            new TObj(std::forward<TArgs>(args)...),
            Deleter<typename Base::Message>(&allocated_));
        allocated_ = true;
        return std::move(msg);
    }

private:
    typedef typename comms::util::TupleAsAlignedUnion<TAllMessages>::Type AlignedStorage;

    AlignedStorage place_;
    bool allocated_ = false;
};

}  // namespace details

}  // namespace protocol

}  // namespace comms


