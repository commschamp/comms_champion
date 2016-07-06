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


#pragma once

#include <memory>
#include <type_traits>
#include <array>
#include <algorithm>

#include "comms/Assert.h"
#include "Tuple.h"

namespace comms
{

namespace util
{

namespace alloc
{

namespace details
{

template <typename T>
class InPlaceDeleter
{
    template<typename U>
    friend class InPlaceDeleter;

public:
    InPlaceDeleter(bool* allocated = nullptr)
        : allocated_(allocated)
    {
    }

    InPlaceDeleter(const InPlaceDeleter& other) = delete;

    template <typename U>
    InPlaceDeleter(InPlaceDeleter<U>&& other)
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

    ~InPlaceDeleter()
    {
        GASSERT(allocated_ == nullptr);
    }

    InPlaceDeleter& operator=(const InPlaceDeleter& other) = delete;

    template <typename U>
    InPlaceDeleter& operator=(InPlaceDeleter<U>&& other)
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


}  // namespace details

template <typename TInterface>
class DynMemory
{
public:
    typedef std::unique_ptr<TInterface> Ptr;

    template <typename TObj, typename... TArgs>
    static Ptr alloc(TArgs&&... args)
    {
        static_assert(std::is_base_of<TInterface, TObj>::value,
            "TObj does not inherit from TInterface");
        return Ptr(new TObj(std::forward<TArgs>(args)...));
    }
};

template <typename TInterface, typename TAllTypes>
class InPlaceSingle
{
public:

    typedef std::unique_ptr<TInterface, details::InPlaceDeleter<TInterface> > Ptr;

    template <typename TObj, typename... TArgs>
    Ptr alloc(TArgs&&... args)
    {
        if (allocated_) {
            return Ptr();
        }

        static_assert(std::is_base_of<TInterface, TObj>::value,
            "TObj does not inherit from TInterface");

        static_assert(comms::util::IsInTuple<TObj, TAllTypes>::Value, ""
            "TObj must be in provided tuple of supported types");

        static_assert(
            std::has_virtual_destructor<TInterface>::value ||
            std::is_same<TInterface, TObj>::value,
            "TInterface is expected to have virtual destructor");

        static_assert(sizeof(TObj) <= sizeof(place_), "Object is too big");

        new (&place_) TObj(std::forward<TArgs>(args)...);
        Ptr obj(
            reinterpret_cast<TInterface*>(&place_),
            details::InPlaceDeleter<TInterface>(&allocated_));
        allocated_ = true;
        return std::move(obj);
    }

    bool allocated() const
    {
        return allocated_;
    }

private:
    typedef typename TupleAsAlignedUnion<TAllTypes>::Type AlignedStorage;

    AlignedStorage place_;
    bool allocated_ = false;

};

template <typename TInterface, std::size_t TSize, typename TAllTypes = std::tuple<TInterface> >
class InPlacePool
{
    typedef InPlaceSingle<TInterface, TAllTypes> PoolElem;
    typedef std::array<PoolElem, TSize> Pool;
public:

    typedef typename PoolElem::Ptr Ptr;

    template <typename TObj, typename... TArgs>
    Ptr alloc(TArgs&&... args)
    {
        auto iter = std::find_if(
            pool_.begin(), pool_.end(),
            [](const PoolElem& elem) -> bool
            {
                return !elem.allocated();
            });

        if (iter == pool_.end()) {
            return Ptr();
        }

        return iter->alloc<TObj>(std::forward<TArgs>(args)...);
    }

private:
    Pool pool_;
};



}  // namespace alloc

}  // namespace util

}  // namespace comms


