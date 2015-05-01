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

namespace comms
{

namespace field
{

namespace category
{

struct NumericValueField {};

namespace details
{

template <typename T>
struct IsValidCategory
{
    static const bool Value =
        std::is_same<NumericValueField, T>::value;
};

template <class T, class R = void>
struct EnableIfHasCategory { typedef R Type; };

template <class T, class Enable = void>
struct HasCategory
{
    static const bool Value = false;
};

template <class T>
struct HasCategory<T, typename EnableIfHasCategory<typename T::Category>::Type>
{
    static const bool Value =
        IsValidCategory<typename T::Category>::Value;
};

}  // namespace details

template <typename T>
constexpr bool isCategorised()
{
    return details::HasCategory<T>::Value;
}

}  // namespace category

}  // namespace field

}  // namespace comms


