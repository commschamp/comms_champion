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

#include <string>
#include <algorithm>
#include <iterator>

#include "comms/options.h"
#include "comms/Assert.h"
#include "comms/util/StaticQueue.h"

namespace comms
{

namespace field
{

namespace details
{

template <std::size_t TSize>
class StaticStringWrapper
{
    typedef comms::util::StaticQueue<char, TSize + 1> Queue;
public:
    typedef typename Queue::LinearisedIterator iterator;
    typedef typename Queue::ConstLinearisedIterator const_iterator;
    typedef typename std::iterator_traits<iterator>::iterator_category iterator_category;
    typedef typename std::iterator_traits<iterator>::value_type value_type;
    typedef typename std::iterator_traits<iterator>::difference_type difference_type;
    typedef typename std::iterator_traits<iterator>::pointer pointer;


    StaticStringWrapper()
    {
        endString();
    }

    StaticStringWrapper(const StaticStringWrapper&) = default;
    StaticStringWrapper(const char* str)
    {
        copyString(str);
    }

    ~StaticStringWrapper() = default;

    StaticStringWrapper& operator=(const StaticStringWrapper&) = default;

    StaticStringWrapper& operator=(const char* str)
    {
        str_.clear();
        copyString(str);
        return *this;
    }

    void clear()
    {
        str_.clear();
        endString();
    }

    bool empty() const
    {
        return size() == 0;
    }

    constexpr std::size_t max_size() const
    {
        return str_.capacity() - 1;
    }

    const char* c_str() const
    {
        return &str_[0];
    }

    const char* data() const
    {
        return c_str();
    }

    iterator begin()
    {
        return str_.lbegin();
    }

    const_iterator begin() const
    {
        return str_.lbegin();
    }

    const_iterator cbegin() const
    {
        return str_.clbegin();
    }

    iterator end()
    {
        return str_.lend() - 1;
    }

    const_iterator end() const
    {
        return str_.lend() - 1;
    }

    const_iterator cend() const
    {
        return str_.clend() - 1;
    }

    std::size_t size() const
    {
        GASSERT(0 < str_.size());
        return str_.size() - 1;
    }

    void push_back(char ch)
    {
        str_[str_.size() - 1] = ch;
        endString();
    }

    template <std::size_t TOtherSize>
    bool equals(const StaticStringWrapper<TOtherSize>& other) const
    {
        return str_ == other.str_;
    }

    bool equals(const char* str) const
    {
        for (auto iter = begin(); iter != end(); ++iter) {
            if (*iter != *str) {
                return false;
            }

            GASSERT(*str != '\0');
            ++str;
        }

        if (*str != '\0') {
            return false;
        }

        return true;
    }

private:
    void copyString(const char* str)
    {
        while ((*str != '\0') && (str_.size() < max_size())) {
            str_.pushBack(*str);
            ++str;
        }
        endString();
    }

    void endString()
    {
        str_.pushBack('\0');
    }
    Queue str_;
};

template <std::size_t TSize>
bool operator==(const StaticStringWrapper<TSize>& str1, const StaticStringWrapper<TSize>& str2)
{
    return str1.equals(str2);
}

template <std::size_t TSize>
bool operator==(const StaticStringWrapper<TSize>& str1, const char* str2)
{
    return str1.equals(str2);
}

template <std::size_t TSize>
bool operator==(const char* str1, const StaticStringWrapper<TSize>& str2)
{
    return str2.equals(str1);
}

template <std::size_t TSize>
bool operator!=(const StaticStringWrapper<TSize>& str1, const StaticStringWrapper<TSize>& str2)
{
    return !(str1 == str2);
}

template <std::size_t TSize>
bool operator!=(const StaticStringWrapper<TSize>& str1, const char* str2)
{
    return !(str1 == str2);
}

template <std::size_t TSize>
bool operator!=(const char* str1, const StaticStringWrapper<TSize>& str2)
{
    return !(str1 == str2);
}


template <typename TField, typename TSizeField, typename... TOptions>
class StringBase;

template <typename TField, typename TSizeField>
class StringBase<TField, TSizeField> : public TField
{
protected:
    typedef TSizeField SizeField;
    typedef std::string StorageType;
    static const bool HasCustomInitialiser = false;
    static const bool HasCustomValidator = false;
};

template <typename TField, typename TSizeField, std::size_t TSize, typename... TOptions>
class StringBase<TField, TSizeField, comms::option::FixedSizeStorage<TSize>, TOptions...> :
    public StringBase<TField, TSizeField, TOptions...>
{
    typedef comms::option::FixedSizeStorage<TSize> Option;
protected:
    typedef StaticStringWrapper<Option::Value> StorageType;
};

template <typename TField, typename TSizeField, typename TInit, typename... TOptions>
class StringBase<
    TField,
    TSizeField,
    comms::option::DefaultValueInitialiser<TInit>,
    TOptions...> : public StringBase<TField, TSizeField, TOptions...>
{
    typedef comms::option::DefaultValueInitialiser<TInit> Option;

protected:
    typedef typename Option::Type DefaultValueInitialiser;
    static const bool HasCustomInitialiser = true;
};

template <typename TField, typename TSizeField, typename TValidator, typename... TOptions>
class StringBase<
    TField,
    TSizeField,
    comms::option::ContentsValidator<TValidator>,
    TOptions...> : public StringBase<TField, TSizeField, TOptions...>
{
    typedef comms::option::ContentsValidator<TValidator> Option;

protected:
    typedef typename Option::Type ContentsValidator;
    static const bool HasCustomValidator = true;
};


}  // namespace details

}  // namespace field

}  // namespace comms
