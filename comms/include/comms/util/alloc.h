//
// Copyright 2016 - 2020 (C). Alex Robenko. All rights reserved.
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

/// @file comms/util/alloc.h
/// This file contains various generic allocator classes that may be used
/// to allocate objects using dynamic memory or "in-place" allocations.


#pragma once

#include <memory>
#include <type_traits>
#include <array>
#include <algorithm>
#include <limits>

#include "comms/Assert.h"
#include "comms/dispatch.h"
#include "Tuple.h"

namespace comms
{

namespace util
{

namespace alloc
{

namespace details
{

template <typename TInterfaceType, typename TDefaultType>
struct DynMemoryDeleteHandler
{
    template <typename TObj>
    void handle(TObj& obj) const
    {
        handleInternal(obj, HandleTag());
    }

private:
    struct NoDefaultCastTag {};
    struct DefaultCastCheckTag {};
    struct ForcedDefaultCastTag {};

    using HandleTag =
        typename std::conditional<
            std::is_void<TDefaultType>::value,
            NoDefaultCastTag,
            DefaultCastCheckTag
        >::type;

    template <typename TObj>
    void handleInternal(TObj& obj, NoDefaultCastTag) const
    {
        delete (&obj);
    }

    template <typename TObj>
    void handleInternal(TObj& obj, ForcedDefaultCastTag) const
    {
        delete static_cast<TDefaultType*>(&obj);
    }

    template <typename TObj>
    void handleInternal(TObj& obj, DefaultCastCheckTag) const
    {
        using ObjType = typename std::decay<decltype(obj)>::type;
        using Tag =
            typename std::conditional<
                std::is_same<ObjType, TInterfaceType>::value,
                ForcedDefaultCastTag,
                NoDefaultCastTag
            >::type;
        handleInternal(obj, Tag());
    }

};

template <typename TInterfaceType, typename TDefaultType>
struct InPlaceDeleteHandler
{
    template <typename TObj>
    void handle(TObj& obj) const
    {
        handleInternal(obj, HandleTag());
    }

private:
    struct NoDefaultCastTag {};
    struct DefaultCastCheckTag {};
    struct ForcedDefaultCastTag {};

    using HandleTag =
        typename std::conditional<
            std::is_void<TDefaultType>::value,
            NoDefaultCastTag,
            DefaultCastCheckTag
        >::type;

    template <typename TObj>
    void handleInternal(TObj& obj, NoDefaultCastTag) const
    {
        obj.~TObj();
    }

    template <typename TObj>
    void handleInternal(TObj& obj, ForcedDefaultCastTag) const
    {
        static_cast<TDefaultType&>(obj).~TDefaultType();
    }

    template <typename TObj>
    void handleInternal(TObj& obj, DefaultCastCheckTag) const
    {
        using ObjType = typename std::decay<decltype(obj)>::type;
        using Tag =
            typename std::conditional<
                std::is_same<ObjType, TInterfaceType>::value,
                ForcedDefaultCastTag,
                NoDefaultCastTag
            >::type;
        handleInternal(obj, Tag());
    }
};

template <
    typename TInterface,
    typename TAllMessages,
    typename TDeleteHandler,
    typename TId>
class NoVirtualDestructorDeleter
{
    static const unsigned InvalidIdx = std::numeric_limits<unsigned>::max();
public:
    NoVirtualDestructorDeleter() : id_(TId()), idx_(InvalidIdx) {}
    NoVirtualDestructorDeleter(TId id, unsigned idx) : id_(id), idx_(idx) {}

    void operator()(TInterface* obj)
    {
        COMMS_ASSERT(obj != nullptr);
        COMMS_ASSERT(idx_ != InvalidIdx);
        TDeleteHandler handler;
        comms::dispatchMsgStaticBinSearch<TAllMessages>(id_, idx_, *obj, handler);
    }
private:
    TId id_;
    unsigned idx_ = 0;
};

template <
    typename TInterface,
    typename TAllMessages,
    typename TDeleteHandler,
    typename TId>
class NoVirtualDestructorInPlaceDeleter : public
        NoVirtualDestructorDeleter<TInterface, TAllMessages, TDeleteHandler, TId>
{
    using Base = NoVirtualDestructorDeleter<TInterface, TAllMessages, TDeleteHandler, TId>;
    static const unsigned InvalidIdx = std::numeric_limits<unsigned>::max();
public:
    NoVirtualDestructorInPlaceDeleter() = default;
    NoVirtualDestructorInPlaceDeleter(TId id, unsigned idx, bool& allocated) : Base(id, idx), allocated_(&allocated) {}

    NoVirtualDestructorInPlaceDeleter(const NoVirtualDestructorInPlaceDeleter&) = delete;
    NoVirtualDestructorInPlaceDeleter(NoVirtualDestructorInPlaceDeleter&& other) :
        Base(std::move(other)),
        allocated_(other.allocated_)
    {
        other.allocated_ = nullptr;
    }

    ~NoVirtualDestructorInPlaceDeleter()
    {
    }

    NoVirtualDestructorInPlaceDeleter& operator=(const NoVirtualDestructorInPlaceDeleter&) = delete;
    NoVirtualDestructorInPlaceDeleter& operator=(NoVirtualDestructorInPlaceDeleter&& other)
    {
        if (reinterpret_cast<void*>(this) == reinterpret_cast<const void*>(&other)) {
            return *this;
        }

        Base::operator=(std::move(other));
        allocated_ = other.allocated_;
        other.allocated_ = nullptr;
        return *this;
    }

    void operator()(TInterface* obj)
    {
        COMMS_ASSERT(allocated_ != nullptr);
        COMMS_ASSERT(*allocated_);
        Base::operator()(obj);
        *allocated_ = false;
        allocated_ = nullptr;
    }
private:
    bool* allocated_ = nullptr;
};

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

    ~InPlaceDeleter() noexcept
    {
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

        COMMS_ASSERT(allocated_ == nullptr);
        allocated_ = other.allocated_;
        other.allocated_ = nullptr;
        return *this;
    }

    void operator()(T* obj) {
        COMMS_ASSERT(allocated_ != nullptr);
        COMMS_ASSERT(*allocated_);
        obj->~T();
        *allocated_ = false;
        allocated_ = nullptr;
    }

private:
    bool* allocated_ = nullptr;
};


}  // namespace details

/// @brief Dynamic memory allocator
/// @details Uses standard operator "new" to allocate and initialise requested
///     object.
/// @tparam TInterface Common interface class for all objects being allocated
///     with this allocator.
template <typename TInterface>
class DynMemory
{
public:
    /// @brief Smart pointer (std::unique_ptr) to the allocated object
    using Ptr = std::unique_ptr<TInterface>;

    /// @brief Allocation function
    /// @tparam TObj Type of the object being allocated, expected to be the
    ///     same as or derived from TInterface.
    /// @tparam TArgs types of arguments to be passed to the constructor.
    /// @param[in] args Extra arguments to be passed to allocated object's constructor.
    /// @return Smart pointer to the allocated object.
    template <typename TObj, typename... TArgs>
    static Ptr alloc(TArgs&&... args)
    {
        static_assert(std::is_base_of<TInterface, TObj>::value,
            "TObj does not inherit from TInterface");
        return Ptr(new TObj(std::forward<TArgs>(args)...));
    }

    /// @brief Function used to wrap raw pointer into a smart one
    /// @tparam Type of the object, expected to be the
    ///     same as or derived from TInterface.
    /// @param[in] obj Pointer to previously allocated object.
    /// @return Smart pointer to the wrapped object.
    template <typename TObj>
    static Ptr wrap(TObj* obj)
    {
        static_assert(std::is_base_of<TInterface, TObj>::value,
            "TObj does not inherit from TInterface");
        return Ptr(obj);
    }

    /// @brief Inquiry whether allocation is possible
    /// @return Always @b true.
    static constexpr bool canAllocate()
    {
        return true;
    }
};

/// @brief Dynamic memory allocator for message types without virtual destructor
/// @details Uses standard operator "new" to allocate and initialise requested
///     object while using custom deleter.
/// @tparam TInterface Common interface class for all objects being allocated
///     with this allocator.
/// @tparam TAllMessages Tuple of all messages types, object of which could be allocated.
/// @tparam TId Type of message ID
/// @tparam TDefaultType Message type to cast to when correct type cannot be recognised.
///     @b void means does not exist.
template <typename TInterface, typename TAllMessages, typename TId, typename TDefaultType = void>
class DynMemoryNoVirtualDestructor
{
    using Deleter =
        details::NoVirtualDestructorDeleter<
            TInterface,
            TAllMessages,
            details::DynMemoryDeleteHandler<TInterface, TDefaultType>,
            TId>;
public:
    /// @brief Smart pointer (std::unique_ptr) to the allocated object
    using Ptr = std::unique_ptr<TInterface, Deleter>;

    /// @brief Allocation function
    /// @tparam TObj Type of the object being allocated, expected to be the
    ///     same as or derived from TInterface.
    /// @tparam TArgs types of arguments to be passed to the constructor.
    /// @param[in] id Numeric ID of the message
    /// @param[in] idx Index of the message type among types with same ID
    ///     provided in @b TAllMessages tuple.
    /// @param[in] args Extra arguments to be passed to allocated object's constructor.
    /// @return Smart pointer to the allocated object.
    template <typename TObj, typename... TArgs>
    static Ptr alloc(TId id, unsigned idx, TArgs&&... args)
    {
        static_assert(std::is_base_of<TInterface, TObj>::value,
            "TObj does not inherit from TInterface");
        return Ptr(new TObj(std::forward<TArgs>(args)...), Deleter(id, idx));
    }

    /// @brief Inquiry whether allocation is possible
    /// @return Always @b true.
    static constexpr bool canAllocate()
    {
        return true;
    }
};

/// @brief In-place single object allocator.
/// @details May allocate only single object at a time. In order to be able
///     to allocate new object, previous one must be destructed first. The
///     allocator contains uninitialised storage area in its private data,
///     which is used to contain allocated object.
/// @tparam TInterface Common interface class for all objects being allocated
///     with this allocator.
/// @tparam TAllTypes All the possible types that can be allocated with this
///     allocator bundled in @b std::tuple. They are used to identify the
///     size required to allocate any of the provided objects.
template <typename TInterface, typename TAllTypes>
class InPlaceSingle
{
public:
    /// @brief Smart pointer (std::unique_ptr) to the allocated object.
    /// @details The custom deleter makes sure the destructor of the
    ///     allocated object is called.
    using Ptr = std::unique_ptr<TInterface, details::InPlaceDeleter<TInterface> >;

    /// @brief Destructor
    ~InPlaceSingle()
    {
        // Not supposed to be destructed while elemenent is still allocated
        COMMS_ASSERT(!allocated_);
    }

    /// @brief Allocation function
    /// @tparam TObj Type of the object being allocated, expected to be the
    ///     same as or derived from @b TInterface.
    /// @tparam TArgs types of arguments to be passed to the constructor.
    /// @param[in] args Extra arguments to be passed to allocated object's constructor.
    /// @return Smart pointer to the allocated object.
    /// @pre If @b TObj is NOT the same as @b TInterface, i.e. @b TInterface is a base
    ///     class of @b TObj, then @b TInterface must have virtual destructor.
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
        return obj;
    }

    /// @brief Inquire whether the object is already allocated.
    bool allocated() const
    {
        return allocated_;
    }

    /// @brief Get address of the objects being allocated using this allocator
    const void* allocAddr() const
    {
        return &place_;
    }

    /// @brief Function used to wrap raw pointer into a smart one
    /// @tparam Type of the object, expected to be the
    ///     same as or derived from TInterface.
    /// @param[in] obj Pointer to previously allocated object.
    /// @return Smart pointer to the wrapped object.
    template <typename TObj>
    Ptr wrap(TObj* obj)
    {
        if (obj == nullptr) {
            return Ptr();
        }

        static_assert(std::is_base_of<TInterface, TObj>::value,
            "TObj does not inherit from TInterface");
        COMMS_ASSERT(obj == reinterpret_cast<TInterface*>(&place_)); // Wrong object if fails
        COMMS_ASSERT(allocated_); // Error if not set
        return Ptr(
            reinterpret_cast<TInterface*>(&place_),
            details::InPlaceDeleter<TInterface>(&allocated_));
    }

    /// @brief Inquiry whether allocation is possible.
    bool canAllocate() const
    {
        return !allocated_;
    }

private:
    using AlignedStorage = typename TupleAsAlignedUnion<TAllTypes>::Type;

    AlignedStorage place_;
    bool allocated_ = false;

};

/// @brief In-place single object allocator for message objects
///     without virtual destructor.
/// @details May allocate only single object at a time. In order to be able
///     to allocate new object, previous one must be destructed first. The
///     allocator contains uninitialised storage area in its private data,
///     which is used to contain allocated object.
/// @tparam TInterface Common interface class for all objects being allocated
///     with this allocator.
/// @tparam TAllocMessages All the possible message types that can be allocated with this
///     allocator bundled in @b std::tuple. They are used to identify the
///     size required to allocate any of the provided objects.
/// @tparam TOrigMessages All the original message types (without @ref comms::GenericMessage added)
///     ids of which are known at compile time.
/// @tparam TDefaultType Message type to cast to when correct type cannot be recognised.
///     @b void means does not exist.
template <
    typename TInterface,
    typename TAllocMessages,
    typename TOrigMessages,
    typename TId,
    typename TDefaultType = void>
class InPlaceSingleNoVirtualDestructor
{
    using Deleter =
        details::NoVirtualDestructorInPlaceDeleter<
            TInterface,
            TOrigMessages,
            details::InPlaceDeleteHandler<TInterface, TDefaultType>,
            TId>;

public:
    /// @brief Smart pointer (std::unique_ptr) to the allocated object.
    /// @details The custom deleter makes sure the destructor of the
    ///     allocated object is called.
    using Ptr = std::unique_ptr<TInterface, Deleter>;

    /// @brief Allocation function
    /// @tparam TObj Type of the object being allocated, expected to be the
    ///     same as or derived from @b TInterface.
    /// @tparam TArgs types of arguments to be passed to the constructor.
    /// @param[in] id Numeric ID of the message
    /// @param[in] idx Index of the message type among types with same ID
    ///     provided in @b TOrigMessages tuple.
    /// @param[in] args Extra arguments to be passed to allocated object's constructor.
    /// @return Smart pointer to the allocated object.
    template <typename TObj, typename... TArgs>
    Ptr alloc(TId id, unsigned idx, TArgs&&... args)
    {
        if (allocated_) {
            return Ptr();
        }

        static_assert(std::is_base_of<TInterface, TObj>::value,
            "TObj does not inherit from TInterface");

        static_assert(comms::util::IsInTuple<TObj, TAllocMessages>::Value, ""
            "TObj must be in provided tuple of supported types");

        static_assert(sizeof(TObj) <= sizeof(place_), "Object is too big");

        new (&place_) TObj(std::forward<TArgs>(args)...);
        Ptr obj(
            reinterpret_cast<TInterface*>(&place_),
            Deleter(id, idx, allocated_));
        allocated_ = true;
        return obj;
    }

    /// @brief Inquire whether the object is already allocated.
    bool allocated() const
    {
        return allocated_;
    }

    /// @brief Get address of the objects being allocated using this allocator
    const void* allocAddr() const
    {
        return &place_;
    }

    /// @brief Inquiry whether allocation is possible.
    bool canAllocate() const
    {
        return !allocated_;
    }

private:
    using AlignedStorage = typename TupleAsAlignedUnion<TAllocMessages>::Type;

    AlignedStorage place_;
    bool allocated_ = false;

};

/// @brief In-place object pool allocator.
/// @details Similar to @ref InPlaceSingle allocator, but allows multiple
///     allocations at the same time, limited by TSize template parameter.
/// @tparam TInterface Common interface class for all objects being allocated
///     with this allocator.
/// @tparam TSize Number of objects this allocator is allowed to allocate.
/// @tparam TAllTypes All the possible types that can be allocated with this
///     allocator bundled in @b std::tuple.
template <typename TInterface, std::size_t TSize, typename TAllTypes = std::tuple<TInterface> >
class InPlacePool
{
    using PoolElem = InPlaceSingle<TInterface, TAllTypes>;
    using Pool = std::array<PoolElem, TSize>;
public:

    /// @brief Smart pointer (std::unique_ptr) to the allocated object.
    /// @details Same as InPlaceSingle::Ptr;
    using Ptr = typename PoolElem::Ptr;

    /// @copydoc InPlaceSingle::alloc
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

        return iter->template alloc<TObj>(std::forward<TArgs>(args)...);
    }

    /// @brief Function used to wrap raw pointer into a smart one
    /// @tparam Type of the object, expected to be the
    ///     same as or derived from TInterface.
    /// @param[in] obj Pointer to previously allocated object.
    /// @return Smart pointer to the wrapped object.
    template <typename TObj>
    Ptr wrap(TObj* obj)
    {
        auto iter =
            std::find_if(
                pool_.begin(), pool_.end(),
                [obj](const PoolElem& elem) -> bool
                {
                    return elem.allocated() && (elem.allocAddr() == obj);
                });

        if (iter == pool_.end()) {
            return Ptr();
        }

        return iter->wrap(obj);
    }

private:
    Pool pool_;
};



}  // namespace alloc

}  // namespace util

}  // namespace comms


