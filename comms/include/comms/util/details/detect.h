//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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