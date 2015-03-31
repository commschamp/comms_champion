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

#include <type_traits>
#include <cstdint>
#include <cstddef>
#include <limits>

#include "comms/field/options.h"

namespace comms
{

namespace field
{

namespace details
{

struct DefaultStaticStringInitialiser
{
    typedef const char* Iterator;
    static Iterator begin()
    {
        return nullptr;
    }

    static constexpr std::size_t size()
    {
        return 0;
    }
};

struct DefaultStaticStringSizeValidator
{
    constexpr bool operator()(std::size_t) const
    {
        return true;
    }
};

template <std::size_t TMaxSize>
struct StaticStringSizeValidator
{
    constexpr bool operator()(std::size_t size) const
    {
        return size <= TMaxSize;
    }
};

struct DefaultStaticStringContentValidator
{
    template <typename TIter>
    bool operator()(TIter, TIter) const
    {
        return true;
    }
};



template <typename TField, typename... TOptions>
class StaticStringBase;

template <typename TField>
class StaticStringBase<TField> : public TField
{
protected:
    typedef DefaultStaticStringInitialiser DefaultInitialiser;
    typedef DefaultStaticStringSizeValidator SizeValidator;
    typedef DefaultStaticStringContentValidator ContentValidator;

    static const std::size_t SizeLength = sizeof(std::uint8_t);
    static const std::size_t StorageSize =
        static_cast<std::size_t>(std::numeric_limits<std::uint8_t>::max());
};

template <typename TField, std::size_t TLen, typename... TOptions>
class StaticStringBase<TField, comms::field::option::SetStringSizeLengthLimit<TLen>, TOptions...> :
    public StaticStringBase<TField, TOptions...>
{
    typedef StaticStringBase<TField, TOptions...> Base;
    typedef comms::field::option::SetStringSizeLengthLimit<TLen> Option;

protected:

    static const std::size_t SizeLength = Option::Value;
};

template <typename TField, std::size_t TSize, typename... TOptions>
class StaticStringBase<TField, comms::field::option::FixedSizeStorage<TSize>, TOptions...> :
    public StaticStringBase<TField, TOptions...>
{
    typedef StaticStringBase<TField, TOptions...> Base;
    typedef comms::field::option::FixedSizeStorage<TSize> Option;
    static const std::size_t MaxAllowedSize =
        static_cast<std::size_t>(std::numeric_limits<std::uint16_t>::max()) + 1;

protected:

    static const std::size_t StorageSize = Option::Value;

    static_assert(
        StorageSize <= MaxAllowedSize,
        "Current implementation doesn't allow strings longer than 64KB.");
};

template <typename TField, typename T, typename... TOptions>
class StaticStringBase<TField, comms::field::option::SetDefaultValueInitialiser<T>, TOptions...> :
    public StaticStringBase<TField, TOptions...>
{
    typedef StaticStringBase<TField, TOptions...> Base;
    typedef comms::field::option::SetDefaultValueInitialiser<T> Option;

protected:

    typedef typename Option::Type DefaultInitialiser;
};

template <typename TField, std::size_t TSize, typename... TOptions>
class StaticStringBase<TField, comms::field::option::SetValidStringMaxSize<TSize>, TOptions...> :
    public StaticStringBase<TField, TOptions...>
{
    typedef StaticStringBase<TField, TOptions...> Base;
    typedef comms::field::option::SetValidStringMaxSize<TSize> Option;

protected:

    typedef StaticStringSizeValidator<Option::Value> SizeValidator;
};

template <typename TField, typename T, typename... TOptions>
class StaticStringBase<TField, comms::field::option::SetStringContentValidator<T>, TOptions...> :
    public StaticStringBase<TField, TOptions...>
{
    typedef StaticStringBase<TField, TOptions...> Base;
    typedef comms::field::option::SetStringContentValidator<T> Option;

protected:

    typedef typename Option::Type ContentValidator;
};


}  // namespace details

}  // namespace field

}  // namespace comms

