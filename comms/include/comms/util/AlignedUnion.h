//
// Copyright 2013 - 2019 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
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

namespace comms
{

namespace util
{

/// @cond SKIP_DOC
template <typename TType, typename... TTypes>
class AlignedUnion
{
    using OtherStorage = typename AlignedUnion<TTypes...>::Type;
    static const std::size_t OtherSize = sizeof(OtherStorage);
    static const std::size_t OtherAlignment = std::alignment_of<OtherStorage>::value;
    using FirstStorage = typename AlignedUnion<TType>::Type;
    static const std::size_t FirstSize = sizeof(FirstStorage);
    static const std::size_t FirstAlignment = std::alignment_of<FirstStorage>::value;
    static const std::size_t MaxSize = FirstSize > OtherSize ? FirstSize : OtherSize;
    static const std::size_t MaxAlignment = FirstAlignment > OtherAlignment ? FirstAlignment : OtherAlignment;
public:
    /// Type that has proper size and proper alignment to keep any of the
    /// specified types
    using Type = typename std::aligned_storage<MaxSize, MaxAlignment>::type;
};

template <typename TType>
class AlignedUnion<TType>
{
public:
    using Type = typename std::aligned_storage<sizeof(TType), std::alignment_of<TType>::value>::type;
};

/// @endcond

}  // namespace util

}  // namespace comms
