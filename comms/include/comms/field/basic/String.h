//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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
#include <algorithm>
#include <limits>
#include <numeric>

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/util/access.h"
#include "comms/util/StaticVector.h"
#include "comms/util/StaticString.h"
#include "comms/details/detect.h"
#include "CommonFuncs.h"

namespace comms
{

namespace field
{

namespace basic
{

namespace details
{

template <typename TStorage>
struct StringMaxLengthRetrieveHelper
{
    static const std::size_t Value = CommonFuncs::maxSupportedLength();
};

template <std::size_t TSize>
struct StringMaxLengthRetrieveHelper<comms::util::StaticString<TSize> >
{
    static const std::size_t Value = TSize - 1;
};

template <typename T>
class StringHasAssign
{
protected:
  typedef char Yes;
  typedef unsigned No;

  template <typename U, U>
  struct ReallyHas;

  template <typename C>
  static Yes test(ReallyHas<C& (C::*)(typename C::const_pointer, typename C::size_type), &C::assign>*);

  template <typename C>
  static Yes test(ReallyHas<void (C::*)(typename C::const_pointer, typename C::size_type), &C::assign>*);

  template <typename>
  static No test(...);

public:
    static const bool Value = (sizeof(test<T>(0)) == sizeof(Yes));
};


template <typename T>
constexpr bool stringHasAssign()
{
    return StringHasAssign<T>::Value;
}

template <typename T>
class StringHasPushBack
{
protected:
  typedef char Yes;
  typedef unsigned No;

  template <typename U, U>
  struct ReallyHas;

  template <typename C>
  static Yes test(ReallyHas<void (C::*)(char), &C::push_back>*);
  template <typename>
  static No test(...);

public:
    static const bool Value = (sizeof(test<T>(0)) == sizeof(Yes));
};

template <typename T>
constexpr bool stringHasPushBack()
{
    return StringHasPushBack<T>::Value;
}

}  // namespace details

template <typename TFieldBase, typename TStorage>
class String : public TFieldBase
{
    using BaseImpl = TFieldBase;
public:
    using Endian = typename BaseImpl::Endian;

    using ValueType = TStorage;
    using ElementType = typename TStorage::value_type;

    static_assert(std::is_integral<ElementType>::value, "String of characters only supported");
    static_assert(sizeof(ElementType) == sizeof(char), "Single byte charactes only supported");

    String() = default;

    explicit String(const ValueType& val)
      : value_(val)
    {
    }

    explicit String(ValueType&& val)
      : value_(std::move(val))
    {
    }

    String(const String&) = default;
    String(String&&) = default;
    String& operator=(const String&) = default;
    String& operator=(String&&) = default;
    ~String() noexcept = default;

    const ValueType& value() const
    {
        return value_;
    }

    ValueType& value()
    {
        return value_;
    }

    template <typename U>
    void pushBack(U&& val)
    {
        static_assert(details::stringHasPushBack<ValueType>(),
                "The string type must have push_back() member function");
        value_.push_back(static_cast<typename ValueType::value_type>(val));
    }

    ValueType& createBack()
    {
        value_.push_back(ValueType());
        return value_.back();
    }

    void clear()
    {
        static_assert(comms::details::hasClearFunc<ValueType>(),
                "The string type must have clear() member function");
        value_.clear();
    }

    constexpr std::size_t length() const
    {
        return value_.size() * sizeof(ElementType);
    }

    static constexpr std::size_t minLength()
    {
        return 0U;
    }

    static constexpr std::size_t maxLength()
    {
        return
            details::StringMaxLengthRetrieveHelper<TStorage>::Value *
            sizeof(ElementType);
    }

    static constexpr bool valid()
    {
        return true;
    }

    static constexpr std::size_t minElementLength()
    {
        return sizeof(ElementType);
    }

    static constexpr std::size_t maxElementLength()
    {
        return minElementLength();
    }

    static constexpr std::size_t elementLength(const ElementType& elem)
    {
        return sizeof(elem);
    }

    template <typename TIter>
    static ErrorStatus readElement(ElementType& elem, TIter& iter, std::size_t& len)
    {
        if (len < sizeof(ElementType)) {
            return ErrorStatus::NotEnoughData;
        }

        elem = comms::util::readData<ElementType>(iter,  Endian());
        len -= sizeof(ElementType);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    static void readElementNoStatus(ElementType& elem, TIter& iter)
    {
        elem = comms::util::readData<ElementType>(iter,  Endian());
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterCategory =
            typename std::iterator_traits<IterType>::iterator_category;
        static_assert(std::is_base_of<std::random_access_iterator_tag, IterCategory>::value,
            "Iterator for reading is expected to be random access one");

        using ConstPointer = typename ValueType::const_pointer;
        auto* str = reinterpret_cast<ConstPointer>(&(*iter));
        doAdvance(iter, len);
        auto* endStr = reinterpret_cast<ConstPointer>(&(*iter));
        if (static_cast<std::size_t>(std::distance(str, endStr)) == len) {
            using Tag =
                typename std::conditional<
                    details::stringHasAssign<ValueType>(),
                    AssignExistsTag,
                    AssignMissingTag
                >::type;
            doAssign(str, len, Tag());
        }
        else {
            using Tag =
                typename std::conditional<
                    details::stringHasPushBack<ValueType>(),
                    PushBackExistsTag,
                    PushBackMissingTag
                >::type;

            doPushBack(str, len, Tag());
        }

        return ErrorStatus::Success;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    template <typename TIter>
    ErrorStatus readN(std::size_t count, TIter& iter, std::size_t& len)
    {
        if (len < count) {
            return comms::ErrorStatus::NotEnoughData;
        }

        return read(iter, count);
    }

    template <typename TIter>
    void readNoStatusN(std::size_t count, TIter& iter)
    {
        read(iter, count);
    }

    template <typename TIter>
    static ErrorStatus writeElement(const ElementType& elem, TIter& iter, std::size_t& len)
    {
        if (len < sizeof(ElementType)) {
            return ErrorStatus::BufferOverflow;
        }

        comms::util::writeData(elem, iter, Endian());
        len -= sizeof(ElementType);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    static void writeElementNoStatus(const ElementType& elem, TIter& iter)
    {
        comms::util::writeData(elem, iter, Endian());
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        if (len < length()) {
            return comms::ErrorStatus::BufferOverflow;
        }

        writeNoStatus(iter);
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        std::copy_n(value_.begin(), value_.size(), iter);
        doAdvance(iter, value_.size());
    }

    template <typename TIter>
    ErrorStatus writeN(std::size_t count, TIter& iter, std::size_t& len) const
    {
        count = std::min(count, value_.size());

        if (len < count) {
            return comms::ErrorStatus::BufferOverflow;
        }

        writeNoStatusN(count, iter);
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    void writeNoStatusN(std::size_t count, TIter& iter) const
    {
        count = std::min(count, value_.size());
        std::copy_n(value_.begin(), count, iter);
        doAdvance(iter, count);
    }

private:
    struct AssignExistsTag {};
    struct AssignMissingTag {};
    struct PushBackExistsTag {};
    struct PushBackMissingTag {};
    struct ReserveExistsTag {};
    struct ReserveMissingTag {};
    struct AdvancableTag {};
    struct NotAdvancableTag {};

    void doAssign(typename ValueType::const_pointer str, std::size_t len, AssignExistsTag)
    {
        value_.assign(str, len);
    }

    void doAssign(typename ValueType::const_pointer str, std::size_t len, AssignMissingTag)
    {
        value_ = ValueType(str, len);
    }

    void doPushBack(typename ValueType::const_pointer str, std::size_t len, PushBackExistsTag)
    {
        clear();
        doReserve(len);
        for (std::size_t idx = 0; idx < len; ++idx) {
            value_.push_back(str[idx]);
        }
    }

    void doPushBack(typename ValueType::const_pointer str, std::size_t len, PushBackMissingTag)
    {
        value_ = ValueType(str, len);
    }

    void doReserve(std::size_t len)
    {
        using Tag =
            typename std::conditional<
                comms::details::hasReserveFunc<ValueType>(),
                ReserveExistsTag,
                ReserveMissingTag
            >::type;
        doReserve(len, Tag());
    }

    void doReserve(std::size_t len, ReserveExistsTag)
    {
        value_.reserve(len);
    }

    static void doReserve(std::size_t, ReserveMissingTag)
    {
    }

    template <typename TIter>
    static void doAdvance(TIter& iter, std::size_t len)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterCategory = typename std::iterator_traits<IterType>::iterator_category;
        static const bool InputIter =
                std::is_base_of<std::input_iterator_tag, IterCategory>::value;
        using Tag =
            typename std::conditional<
                InputIter,
                AdvancableTag,
                NotAdvancableTag
            >::type;
        doAdvance(iter, len, Tag());
    }

    template <typename TIter>
    static void doAdvance(TIter& iter, std::size_t len, AdvancableTag)
    {
        std::advance(iter, len);
    }

    template <typename TIter>
    static void doAdvance(TIter&, std::size_t, NotAdvancableTag)
    {
    }

    ValueType value_;
};

}  // namespace basic

}  // namespace field

}  // namespace comms


