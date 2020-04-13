//
// Copyright 2020 (C). Alex Robenko. All rights reserved.
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

/// @file
/// Various compile-time detection functions of whether specific member functions and/or types exist

#pragma once

#include <type_traits>
#include <iterator>

#include "comms/util/detect.h"
#include "comms/Assert.h"

namespace comms
{

namespace util
{

namespace details
{

class AssignHelper
{
public:
    template <typename T, typename TIter>
    static void assign(T& obj, TIter from, TIter to)
    {
        using ObjType = typename std::decay<decltype(obj)>::type;

        using Tag = 
            typename std::conditional<
                comms::util::detect::hasAssignFunc<ObjType>(),
                UseAssignTag,
                typename std::conditional<
                    comms::util::detect::hasPtrSizeConstructor<ObjType>(),
                    UsePtrSizeConstructorTag,
                    UnknownTag
                >::type
            >::type;

        static_assert(!std::is_same<Tag, UnknownTag>::value, "Assignment to provided type is not supported");

        assignInternal(obj, from, to, Tag());
    }

private:
    struct UseAssignTag{};
    struct UsePtrSizeConstructorTag{};
    struct UnknownTag{};

    template <typename T, typename TIter>
    static void assignInternal(T& obj, TIter from, TIter to, UseAssignTag)
    {
        obj.assign(from, to);
    }

    template <typename T, typename TIter>
    static void assignInternal(T& obj, TIter from, TIter to, UsePtrSizeConstructorTag)
    {
        using IterType = typename std::decay<TIter>::type;
        using IterTag = typename std::iterator_traits<IterType>::iterator_category;
        static_assert(std::is_base_of<std::random_access_iterator_tag, IterTag>::value,    
            "Only random access iterator is supported for provided type assignments");         

        auto diff = std::distance(from, to);
        if (diff < 0) {
            COMMS_ASSERT(!"Invalid iterators used for assignment");
            return;
        }

        using ObjType = typename std::decay<decltype(obj)>::type;
        obj = ObjType(&(*from), static_cast<std::size_t>(diff));
    }    
};

} // namespace details

} // namespace util

} // namespace comms