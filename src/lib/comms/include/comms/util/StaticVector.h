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

#include <cstddef>
#include <array>
#include <algorithm>
#include <iterator>
#include <initializer_list>

#include "comms/Assert.h"

namespace comms
{

namespace util
{

namespace details
{

template <typename T>
class StaticVectorBase
{
public:
    typedef T value_type;
    typedef std::size_t size_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef pointer iterator;
    typedef const_pointer const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef
        typename std::aligned_storage<
            sizeof(T),
            std::alignment_of<T>::value
        >::type CellType;

    static_assert(sizeof(CellType) == sizeof(T), "Type T must be padded");

    StaticVectorBase(CellType* data, std::size_t cap)
      : data_(data),
        capacity_(cap)
    {
    }

    ~StaticVectorBase()
    {
        clear();
    }

    StaticVectorBase(const StaticVectorBase&) = delete;
    StaticVectorBase& operator=(const StaticVectorBase&) = delete;


    std::size_t size() const
    {
        return size_;
    }

    std::size_t capacity() const
    {
        return capacity_;
    }

    bool empty() const
    {
        return (size() == 0);
    }

    void pop_back()
    {
        auto& lastElem = back();
        lastElem.~T();
        --size_;
    }

    T& back()
    {
        GASSERT(!empty());
        return elem(size() - 1);
    }

    const T& back() const
    {
        GASSERT(!empty());
        return elem(size() - 1);
    }

    T& front()
    {
        GASSERT(!empty());
        return elem(0);
    }

    const T& front() const
    {
        GASSERT(!empty());
        return elem(0);
    }

    template <typename TIter>
    void assign(TIter from, TIter to)
    {
        clear();
        for (auto iter = from; iter != to; ++iter) {
            if (capacity() <= size()) {
                GASSERT(!"Not all elements are copied");
                return;
            }

            new (cellPtr(size())) T(*iter);
            ++size_;
        }
    }

    void fill(std::size_t count, const T& value)
    {
        clear();
        GASSERT(count <= capacity());
        for (auto idx = 0U; idx < count; ++idx) {
            new (cellPtr(idx)) T(value);
        }
        size_ = count;
    }

    void clear() {
        for (auto idx = 0U; idx < size(); ++idx) {
            elem(idx).~T();
        }
        size_ = 0;
    }


    T* begin()
    {
        return &(elem(0));
    }

    const T* begin() const
    {
        return cbegin();
    }

    const T* cbegin() const
    {
        return &(elem(0));
    }

    T* end()
    {
        return begin() + size();
    }

    const T* end() const
    {
        return cend();
    }

    const T* cend() const
    {
        return cbegin() + size();
    }

    T& at(std::size_t pos)
    {
        GASSERT(pos < size());
        return elem(pos);
    }

    const T& at(std::size_t pos) const
    {
        GASSERT(pos < size());
        return elem(pos);
    }

    T& operator[](std::size_t pos)
    {
        return elem(pos);
    }

    const T& operator[](std::size_t pos) const
    {
        return elem(pos);
    }

    T* data()
    {
        return &(elem(0));
    }

    const T* data() const
    {
        return &(elem(0));
    }

    template <typename U>
    T* insert(const T* pos, U&& value)
    {
        GASSERT(pos <= end());
        GASSERT(size() < capacity());
        if (end() <= pos) {
            push_back(std::forward<U>(value));
            return &(back());
        }

        GASSERT(!empty());
        push_back(std::move(back()));
        auto* insertIter = begin() + std::distance(cbegin(), pos);
        std::move_backward(insertIter, end() - 2, end() - 1);
        *insertIter = std::forward<U>(value);
        return insertIter;
    }

    T* insert(const T* pos, std::size_t count, const T& value)
    {
        GASSERT(pos <= end());
        GASSERT((size() + count) < capacity());
        auto* posIter = begin() + std::distance(cbegin(), pos);
        if (end() <= posIter) {
            while (0 < count) {
                push_back(value);
                --count;
            }
            return posIter;
        }

        GASSERT(!empty());
        auto tailCount = static_cast<std::size_t>(std::distance(posIter, end()));
        if (count <= tailCount) {
            auto pushBegIter = end() - count;
            auto pushEndIter = end();
            for (auto iter = pushBegIter; iter != pushEndIter; ++iter) {
                push_back(std::move(*iter));
            }

            auto moveBegIter = posIter;
            auto moveEndIter = moveBegIter + (tailCount - count);
            GASSERT(moveEndIter < pushEndIter);
            std::move_backward(moveBegIter, moveEndIter, pushEndIter);

            auto* assignBegIter = posIter;
            auto* assignEndIter = assignBegIter + count;
            for (auto iter = assignBegIter; iter != assignEndIter; ++iter) {
                *iter = value;
            }
            return posIter;
        }

        auto pushValueCount = count - tailCount;
        for (auto idx = 0U; idx < pushValueCount; ++idx) {
            push_back(value);
        }

        auto* pushBegIter = posIter;
        auto* pushEndIter = pushBegIter + tailCount;
        for (auto iter = pushBegIter; iter != pushEndIter; ++iter) {
            push_back(std::move(*iter));
        }

        auto assignBegIter = posIter;
        auto assignEndIter = assignBegIter + tailCount;
        for (auto iter = assignBegIter; iter != assignEndIter; ++iter) {
            *iter = value;
        }
        return posIter;
    }

    template <typename TIter>
    T* insert(const T* pos, TIter from, TIter to)
    {
        typedef typename std::iterator_traits<TIter>::iterator_category Tag;
        return insert_internal(pos, from, to, Tag());
    }

    template <typename... TArgs>
    T* emplace(const T* iter, TArgs&&... args)
    {
        auto* insertIter = begin() + std::distance(cbegin(), iter);
        if (iter == cend()) {
            emplace_back(std::forward<TArgs>(args)...);
            return insertIter;
        }

        GASSERT(!empty());
        push_back(std::move(back()));
        std::move_backward(insertIter, end() - 2, end() - 1);
        insertIter->~T();
        new (insertIter) T(std::forward<TArgs>(args)...);
        return insertIter;
    }

    T* erase(const T* from, const T* to)
    {
        GASSERT(from <= cend());
        GASSERT(to <= cend());
        GASSERT(from <= to);

        auto tailCount = static_cast<std::size_t>(std::distance(to, cend()));
        auto eraseCount = static_cast<std::size_t>(std::distance(from, to));

        auto* moveSrc = begin() + std::distance(cbegin(), to);
        auto* moveDest = begin() + std::distance(cbegin(), from);
        std::move(moveSrc, end(), moveDest);

        auto* eraseFrom = moveDest + tailCount;
        auto* eraseTo = end();
        GASSERT(eraseFrom <= end());
        GASSERT(eraseCount <= size());
        GASSERT(static_cast<std::size_t>(std::distance(eraseFrom, eraseTo)) == eraseCount);
        for (auto iter = eraseFrom; iter != eraseTo; ++iter) {
            iter->~T();
        }
        size_ -= eraseCount;
        return moveDest;
    }

    template <typename U>
    void push_back(U&& value)
    {
        GASSERT(size() < capacity());
        new (cellPtr(size())) T(std::forward<U>(value));
        ++size_;
    }

    template <typename... TArgs>
    void emplace_back(TArgs&&... args)
    {
        GASSERT(size() < capacity());
        new (cellPtr(size())) T(std::forward<TArgs>(args)...);
        ++size_;
    }

    void resize(std::size_t count, const T& value)
    {
        if (count < size()) {
            erase(begin() + count, end());
            GASSERT(count == size());
            return;
        }

        while (size() < count) {
            push_back(value);
        }
    }

    void swap(StaticVectorBase<T>& other)
    {
        auto swapSize = std::min(other.size(), size());
        for (auto idx = 0U; idx < swapSize; ++idx) {
            std::swap(data_[idx], other.data_[idx]);
        }

        auto otherSize = other.size();
        auto thisSize = size();

        if (otherSize == thisSize) {
            return;
        }

        if (otherSize < thisSize) {
            auto limit = std::min(thisSize, other.capacity());
            for (auto idx = swapSize; idx < limit; ++idx) {
                new (other.cellPtr(idx)) T(std::move(elem(idx)));
            }

            other.size_ = thisSize;
            erase(begin() + otherSize, end());
            return;
        }

        auto limit = std::min(otherSize, capacity());
        for (auto idx = swapSize; idx < limit; ++idx) {
            new (cellPtr(idx)) T(std::move(other.elem(idx)));
        }
        size_ = otherSize;
        other.erase(other.begin() + thisSize, other.end());
    }

private:
    CellType& cell(std::size_t idx)
    {
        GASSERT(idx < capacity());
        return data_[idx];
    }

    const CellType& cell(std::size_t idx) const
    {
        GASSERT(idx < capacity());
        return data_[idx];
    }

    CellType* cellPtr(std::size_t idx)
    {
        GASSERT(idx < capacity());
        return &data_[idx];
    }

    T& elem(std::size_t idx)
    {
        return reinterpret_cast<T&>(cell(idx));
    }

    const T& elem(std::size_t idx) const
    {
        return reinterpret_cast<const T&>(cell(idx));
    }

    template <typename TIter>
    T* insert_random_access(const T* pos, TIter from, TIter to)
    {
        GASSERT(pos <= end());
        auto* posIter = begin() + std::distance(cbegin(), pos);
        if (end() <= posIter) {
            for (; from != to; ++from) {
                push_back(*from);
            }

            return posIter;
        }

        auto count = static_cast<std::size_t>(std::distance(from, to));
        GASSERT(!empty());
        auto tailCount = static_cast<std::size_t>(std::distance(posIter, end()));
        if (count <= tailCount) {
            auto pushBegIter = end() - count;
            auto pushEndIter = end();
            for (auto iter = pushBegIter; iter != pushEndIter; ++iter) {
                push_back(std::move(*iter));
            }

            auto moveBegIter = posIter;
            auto moveEndIter = moveBegIter + (tailCount - count);
            GASSERT(moveEndIter < pushEndIter);
            std::move_backward(moveBegIter, moveEndIter, pushEndIter);

            auto* assignBegIter = posIter;
            auto* assignEndIter = assignBegIter + count;
            for (auto iter = assignBegIter; iter != assignEndIter; ++iter) {
                *iter = *from;
                ++from;
            }
            return posIter;
        }

        auto pushValueCount = count - tailCount;
        auto pushInsertedBegIter = to - pushValueCount;
        for (auto idx = 0U; idx < pushValueCount; ++idx) {
            push_back(*pushInsertedBegIter);
            ++pushInsertedBegIter;
        }

        auto* pushBegIter = posIter;
        auto* pushEndIter = pushBegIter + tailCount;
        for (auto iter = pushBegIter; iter != pushEndIter; ++iter) {
            push_back(std::move(*iter));
        }

        auto assignBegIter = posIter;
        auto assignEndIter = assignBegIter + tailCount;
        for (auto iter = assignBegIter; iter != assignEndIter; ++iter) {
            *iter = *from;
            ++from;
        }

        return posIter;
    }

    template <typename TIter>
    T* insert_input(const T* pos, TIter from, TIter to)
    {
        T* ret = nullptr;
        for (; from != to; ++from) {
            if (ret == nullptr) {
                ret = begin() + std::distance(cbegin(), pos);
            }
            insert(pos, *from);
            ++pos;
        }
        return ret;
    }

    template <typename TIter>
    T* insert_internal(const T* pos, TIter from, TIter to, std::random_access_iterator_tag)
    {
        return insert_random_access(pos, from, to);
    }

    template <typename TIter>
    T* insert_internal(const T* pos, TIter from, TIter to, std::input_iterator_tag)
    {
        return insert_input(pos, from, to);
    }


    CellType* data_ = nullptr;
    std::size_t capacity_ = 0;
    std::size_t size_ = 0;
};

template <typename T, std::size_t TSize>
struct StaticVectorStorageBase
{
    typedef typename std::aligned_storage<
        sizeof(T),
        std::alignment_of<T>::value
    >::type ElementType;

    typedef std::array<ElementType, TSize> StorageType;
    StorageType data_;
};

}  // namespace details

template <typename T, std::size_t TSize>
class StaticVector :
    public details::StaticVectorStorageBase<T, TSize>,
    public details::StaticVectorBase<T>
{
    typedef details::StaticVectorStorageBase<T, TSize> StorageBase;
    typedef details::StaticVectorBase<T> Base;

    typedef typename StorageBase::ElementType ElementType;

    static_assert(sizeof(T) == sizeof(ElementType),
        "Sizes are not equal as expected.");

    template <typename U, std::size_t TOtherSize>
    friend class StaticVector;

public:
    typedef typename Base::value_type value_type;
    typedef typename Base::size_type size_type;
    typedef typename StorageBase::StorageType::difference_type difference_type;
    typedef typename Base::reference reference;
    typedef typename Base::const_reference const_reference;
    typedef typename Base::pointer pointer;
    typedef typename Base::const_pointer const_pointer;
    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;
    typedef typename Base::reverse_iterator reverse_iterator;
    typedef typename Base::const_reverse_iterator const_reverse_iterator;

    StaticVector()
      : Base(&StorageBase::data_[0], StorageBase::data_.size())
    {
    }

    StaticVector(size_type count, const T& value)
      : Base(&StorageBase::data_[0], StorageBase::data_.size())
    {
        assign(count, value);
    }

    explicit StaticVector(size_type count)
      : Base(&StorageBase::data_[0], StorageBase::data_.size())
    {
        GASSERT(count < capacity());
        while (0 < count) {
            emplace_back();
            --count;
        }
    }

    template <typename TIter>
    StaticVector(TIter from, TIter to)
      : Base(&StorageBase::data_[0], StorageBase::data_.size())
    {
        assign(from, to);
    }

    template <std::size_t TOtherSize>
    StaticVector(const StaticVector<T, TOtherSize>& other)
      : Base(&StorageBase::data_[0], StorageBase::data_.size())
    {
        assign(other.begin(), other.end());
    }

    StaticVector(const StaticVector& other)
      : Base(&StorageBase::data_[0], StorageBase::data_.size())
    {
        assign(other.begin(), other.end());
    }

    StaticVector(std::initializer_list<value_type> init)
      : Base(&StorageBase::data_[0], StorageBase::data_.size())
    {
        assign(init.begin(), init.end());
    }

    ~StaticVector() = default;

    StaticVector& operator=(const StaticVector& other)
    {
        if (&other == this) {
            return *this;
        }

        assign(other.begin(), other.end());
        return *this;
    }

    template <std::size_t TOtherSize>
    StaticVector& operator=(const StaticVector<T, TOtherSize>& other)
    {
        assign(other.begin(), other.end());
        return *this;
    }

    StaticVector& operator=(std::initializer_list<value_type> init)
    {
        assign(init);
        return *this;
    }

    void assign(size_type count, const T& value)
    {
        GASSERT(count <= TSize);
        Base::fill(count, value);
    }

    template <typename TIter>
    void assign(TIter from, TIter to)
    {
        Base::assign(from, to);
    }

    void assign(std::initializer_list<value_type> init)
    {
        assign(init.begin(), init.end());
    }

    reference at(size_type pos)
    {
        return Base::at(pos);
    }

    const_reference at(size_type pos) const
    {
        return Base::at(pos);
    }

    reference operator[](size_type pos)
    {
        return Base::operator[](pos);
    }

    const_reference operator[](size_type pos) const
    {
        return Base::operator[](pos);
    }

    reference front()
    {
        return Base::front();
    }

    const_reference front() const
    {
        return Base::front();
    }

    reference back()
    {
        return Base::back();
    }

    const_reference back() const
    {
        return Base::back();
    }

    pointer data()
    {
        return Base::data();
    }

    const_pointer data() const
    {
        return Base::data();
    }

    iterator begin()
    {
        return Base::begin();
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator cbegin() const
    {
        return Base::cbegin();
    }

    iterator end()
    {
        return Base::end();
    }

    const_iterator end() const
    {
        return cend();
    }

    const_iterator cend() const
    {
        return Base::cend();
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const
    {
        return crbegin();
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(cend());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const
    {
        return crend();
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(cbegin());
    }

    bool empty() const
    {
        return Base::empty();
    }

    size_type size() const
    {
        return Base::size();
    }

    size_type max_size() const
    {
        return capacity();
    }

    void reserve(size_type new_cap)
    {
        static_cast<void>(new_cap);
        GASSERT(new_cap <= capacity());
    }

    size_type capacity() const
    {
        return Base::capacity();
    }

    void shrink_to_fit()
    {
    }

    void clear()
    {
        Base::clear();
    }

    iterator insert(const_iterator iter, const T& value)
    {
        return Base::insert(iter, value);
    }

    iterator insert(const_iterator iter, T&& value)
    {
        return Base::insert(iter, std::move(value));
    }

    iterator insert(const_iterator iter, size_type count, const T& value)
    {
        return Base::insert(iter, count, value);
    }

    template <typename TIter>
    iterator insert(const_iterator iter, TIter from, TIter to)
    {
        return Base::insert(iter, from, to);
    }

    iterator insert(const_iterator iter, std::initializer_list<value_type> init)
    {
        return Base::insert(iter, init.begin(), init.end());
    }

    template <typename... TArgs>
    iterator emplace(const_iterator iter, TArgs&&... args)
    {
        return Base::emplace(iter, std::forward<TArgs>(args)...);
    }

    iterator erase(const_iterator iter)
    {
        return erase(iter, iter + 1);
    }

    iterator erase(const_iterator from, const_iterator to)
    {
        return Base::erase(from, to);
    }

    void push_back(const T& value)
    {
        Base::push_back(value);
    }

    void push_back(T&& value)
    {
        Base::push_back(std::move(value));
    }

    template <typename... TArgs>
    void emplace_back(TArgs&&... args)
    {
        Base::emplace_back(std::forward<TArgs>(args)...);
    }

    void pop_back()
    {
        Base::pop_back();
    }

    void resize(size_type count)
    {
        resize(count, T());
    }

    void resize(size_type count, const value_type& value)
    {
        Base::resize(count, value);
    }

    template <std::size_t TOtherSize>
    void swap(StaticVector<T, TOtherSize>& other)
    {
        Base::swap(other);
    }
};

template <typename T, std::size_t TSize1, std::size_t TSize2>
bool operator<(const StaticVector<T, TSize1>& v1, const StaticVector<T, TSize2>& v2)
{
    return std::lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end());
}

template <typename T, std::size_t TSize1, std::size_t TSize2>
bool operator<=(const StaticVector<T, TSize1>& v1, const StaticVector<T, TSize2>& v2)
{
    return !(v2 < v1);
}

template <typename T, std::size_t TSize1, std::size_t TSize2>
bool operator>(const StaticVector<T, TSize1>& v1, const StaticVector<T, TSize2>& v2)
{
    return v2 < v1;
}

template <typename T, std::size_t TSize1, std::size_t TSize2>
bool operator>=(const StaticVector<T, TSize1>& v1, const StaticVector<T, TSize2>& v2)
{
    return !(v1 < v2);
}

template <typename T, std::size_t TSize1, std::size_t TSize2>
bool operator==(const StaticVector<T, TSize1>& v1, const StaticVector<T, TSize2>& v2)
{
    return (v1.size() == v2.size()) &&
           (!(v1 < v2)) &&
           (!(v2 < v1));
}

template <typename T, std::size_t TSize1, std::size_t TSize2>
bool operator!=(const StaticVector<T, TSize1>& v1, const StaticVector<T, TSize2>& v2)
{
    return !(v1 == v2);
}

}  // namespace util

}  // namespace comms

namespace std
{

template <typename T, std::size_t TSize1, std::size_t TSize2>
void swap(comms::util::StaticVector<T, TSize1>& v1, comms::util::StaticVector<T, TSize2>& v2)
{
    v1.swap(v2);
}

}

