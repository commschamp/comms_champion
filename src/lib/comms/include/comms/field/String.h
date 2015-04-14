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

#include "details/StringBase.h"

namespace comms
{

namespace field
{

template <typename TField, typename TSizeField, typename... TOptions>
class String : public details::StringBase<TField, TSizeField, TOptions...>
{
    typedef details::StringBase<TField, TSizeField, TOptions...> Base;
public:

    typedef typename Base::SizeField SizeField;
    typedef typename Base::StorageType StorageType;
    typedef typename StorageType::iterator Iterator;
    typedef Iterator iterator;
    typedef typename StorageType::const_iterator ConstIterator;
    typedef ConstIterator const_iterator;

    typedef typename std::iterator_traits<iterator>::iterator_category iterator_category;
    typedef typename std::iterator_traits<iterator>::value_type value_type;
    typedef typename std::iterator_traits<iterator>::difference_type difference_type;
    typedef typename std::iterator_traits<iterator>::pointer pointer;

    String()
    {
        typedef typename std::conditional<
            Base::HasCustomInitialiser,
            CustomInitialisationTag,
            DefaultInitialisationTag
        >::type Tag;
        completeDefaultInitialisation(Tag());
    }

    explicit String(const char* value)
      : str_(value)
    {
    }

    explicit String(const StorageType& value)
      : str_(value)
    {
    }

    String(const String&) = default;

    String(String&&) = default;

    ~String() = default;

    String& operator=(const String&) = default;

    String& operator=(String&&) = default;

    String& operator=(const char* str)
    {
        str_ = str;
        return *this;
    }

    const StorageType& getValue() const
    {
        return str_;
    }

    void setValue(const char* value)
    {
        str_ = value;
    }

    void setValue(const StorageType& value)
    {
        str_ = value;
    }

    iterator begin()
    {
        return str_.begin();
    }

    const_iterator begin() const
    {
        return str_.begin();
    }

    const_iterator cbegin() const
    {
        return str_.cbegin();
    }

    iterator end()
    {
        return str_.end();
    }

    const_iterator end() const
    {
        return str_.end();
    }

    const_iterator cend() const
    {
        return str_.cend();
    }

    void clear()
    {
        str_.clear();
    }

    constexpr std::size_t length() const
    {
        return sizeField().length() + size();
    }

    constexpr std::size_t size() const
    {
        return str_.size();
    }

    bool empty() const
    {
        return str_.empty();
    }

    constexpr bool valid() const
    {
        return
            SizeField(static_cast<typename SizeField::ValueType>(size())).valid() &&
            validInternal(
            typename std::conditional<
                Base::HasCustomValidator,
                CustomValidatorTag,
                DefaultValidatorTag
            >::type());
    }

    template <typename U>
    void pushBack(U&& ch)
    {
        str_.push_back(std::forward<U>(ch));
    }

    template <typename U>
    void push_back(U&& ch)
    {
        pushBack(std::forward<U>(ch));
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t bufSize)
    {
        SizeField sizeField;
        auto es = sizeField.read(iter, bufSize);
        if (es != ErrorStatus::Success) {
            return es;
        }

        auto len = sizeField.getValue();

        if (str_.max_size() <= len) {
            return ErrorStatus::InvalidMsgData;
        }

        auto remSize = bufSize - sizeField.length();
        if (remSize < len) {
            return ErrorStatus::NotEnoughData;
        }

        clear();
        std::copy_n(iter, len, std::back_inserter(str_));
        std::advance(iter, len);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t bufSize) const
    {
        if (bufSize < length()) {
            return ErrorStatus::BufferOverflow;
        }

        SizeField sizeField(static_cast<typename SizeField::ValueType>(size()));
        auto es = sizeField.write(iter, bufSize);
        static_cast<void>(es);
        GASSERT(es == ErrorStatus::Success);

        std::copy_n(begin(), size(), iter);

        typedef typename std::decay<decltype(iter)>::type IterType;
        typedef typename std::conditional<
            std::is_same<typename std::iterator_traits<IterType>::difference_type, void>::value,
            UseIncTag,
            UseStdAdvanceTag
        >::type Tag;

        advanceIter(iter, size(), Tag());
        return ErrorStatus::Success;
    }

    SizeField sizeField() const
    {
        return SizeField(static_cast<typename SizeField::ValueType>(size()));
    }

private:

    struct DefaultInitialisationTag {};
    struct CustomInitialisationTag {};
    struct DefaultValidatorTag {};
    struct CustomValidatorTag {};
    struct UseStdAdvanceTag {};
    struct UseIncTag {};

    void completeDefaultInitialisation(DefaultInitialisationTag)
    {
    }

    void completeDefaultInitialisation(CustomInitialisationTag)
    {
        typedef typename Base::DefaultValueInitialiser DefaultValueInitialiser;
        DefaultValueInitialiser()(*this);
    }

    static constexpr bool validInternal(DefaultValidatorTag)
    {
        return true;
    }

    constexpr bool validInternal(CustomValidatorTag) const
    {
        typedef typename Base::ContentsValidator ContentsValidator;
        return ContentsValidator()(*this);
    }

    template <typename TIter>
    static void advanceIter(TIter& iter, std::size_t sizeParam, UseStdAdvanceTag)
    {
        std::advance(iter, sizeParam);
    }

    template <typename TIter>
    static void advanceIter(TIter& iter, std::size_t sizeParam, UseIncTag)
    {
        for (auto i = 0U; i < sizeParam; ++i) {
            ++iter;
        }
    }

    StorageType str_;
};

/// @brief Equality comparison operator.
/// @related String
template <typename... TArgs>
bool operator==(
    const String<TArgs...>& field1,
    const String<TArgs...>& field2)
{
    return field1.getValue() == field2.getValue();
}

/// @brief Non-equality comparison operator.
/// @related String
template <typename... TArgs>
bool operator!=(
    const String<TArgs...>& field1,
    const String<TArgs...>& field2)
{
    return field1.getValue() != field2.getValue();
}

/// @brief Equivalence comparison operator.
/// @related String
template <typename... TArgs>
bool operator<(
    const String<TArgs...>& field1,
    const String<TArgs...>& field2)
{
    return field1.getValue() < field2.getValue();
}

namespace details
{

template <typename T>
struct IsString
{
    static const bool Value = false;
};

template <typename... TArgs>
struct IsString<comms::field::String<TArgs...> >
{
    static const bool Value = true;
};

}  // namespace details

template <typename T>
constexpr bool isString()
{
    return details::IsString<T>::Value;
}


}  // namespace field

}  // namespace comms


