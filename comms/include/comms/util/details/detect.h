//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
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

namespace comms
{

namespace util
{

namespace detect
{

namespace details
{

template <typename T>
class HasClearFunc
{
    struct No {};

protected:
    template <typename C>
    static auto test(std::nullptr_t) -> decltype(std::declval<C>().clear());

    template <typename>
    static No test(...);

public:
    static const bool Value = !std::is_same<No, decltype(test<T>(nullptr))>::value;
};

template <typename T>
class HasReserveFunc
{
    struct No {};

protected:
    template <typename C>
    static auto test(std::nullptr_t) -> decltype(std::declval<C>().reserve(0U));

    template <typename>
    static No test(...);

public:
    static const bool Value = !std::is_same<No, decltype(test<T>(nullptr))>::value;
};

template <typename T>
class HasResizeFunc
{
    struct No {};

protected:
    template <typename C>
    static auto test(std::nullptr_t) -> decltype(std::declval<C>().resize(0U));

    template <typename>
    static No test(...);

public:
    static const bool Value = !std::is_same<No, decltype(test<T>(nullptr))>::value;
};

template <typename T>
class HasRemoveSuffixFunc
{
    struct No {};

protected:
    template <typename C>
    static auto test(std::nullptr_t) -> decltype(std::declval<C>().remove_suffix(0U));

    template <typename>
    static No test(...);

public:
    static const bool Value = !std::is_same<No, decltype(test<T>(nullptr))>::value;
};

template <typename T>
class HasAssignFunc
{
    struct No {};

protected:
    template <typename C>
    static auto test(std::nullptr_t) -> decltype(std::declval<C>().assign(static_cast<typename C::const_pointer>(nullptr), static_cast<typename C::const_pointer>(nullptr)));

    template <typename>
    static No test(...);

public:
    static const bool Value = !std::is_same<No, decltype(test<T>(nullptr))>::value;
};

template <typename T>
class HasPtrSizeConstructor
{
    struct No {};

protected:
    template <typename C>
    static auto test(std::nullptr_t) -> decltype(C(static_cast<typename C::const_pointer>(nullptr), static_cast<typename C::size_type>(0U)));

    template <typename>
    static No test(...);

public:
    static const bool Value = !std::is_same<No, decltype(test<T>(nullptr))>::value;
};

} // namespace details

} // namespace detect

} // namespace util

} // namespace comms