//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <algorithm>
#include <limits>
#include <numeric>

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/util/access.h"
#include "comms/util/assign.h"
#include "comms/util/StaticVector.h"
#include "comms/util/StaticString.h"
#include "comms/util/detect.h"
#include "comms/util/type_traits.h"
#include "comms/details/tag.h"
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

    ValueType& createBack()
    {
        value_.push_back(ValueType());
        return value_.back();
    }

    void clear()
    {
        static_assert(comms::util::detect::hasClearFunc<ValueType>(),
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
        std::advance(iter, len);
        auto* endStr = reinterpret_cast<ConstPointer>(&(*iter));
        comms::util::assign(value_, str, endStr);
        return ErrorStatus::Success;
    }

    static constexpr bool hasReadNoStatus()
    {
        return false;
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
    template<typename... TParams>
    using AdvancableTag = comms::details::tag::Tag1<TParams...>;

    template<typename... TParams>
    using NotAdvancableTag = comms::details::tag::Tag2<TParams...>;

    template <typename TIter>
    static void doAdvance(TIter& iter, std::size_t len)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterCategory = typename std::iterator_traits<IterType>::iterator_category;
        static const bool InputIter =
                std::is_base_of<std::input_iterator_tag, IterCategory>::value;
        using Tag =
            typename comms::util::LazyShallowConditional<
                InputIter
            >::template Type<
                AdvancableTag,
                NotAdvancableTag
            >;
        doAdvance(iter, len, Tag());
    }

    template <typename TIter>
    static void doAdvance(TIter& iter, std::size_t len, AdvancableTag<>)
    {
        std::advance(iter, len);
    }

    template <typename TIter>
    static void doAdvance(TIter&, std::size_t, NotAdvancableTag<>)
    {
    }

    ValueType value_;
};

}  // namespace basic

}  // namespace field

}  // namespace comms


