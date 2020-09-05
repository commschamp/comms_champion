//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <utility>

namespace comms
{

namespace details
{

template <class T, class R = void>
struct EnableIfHasInterfaceOptions { using Type = R; };

template <class T, class Enable = void>
struct HasInterfaceOptions
{
    static const bool Value = false;
};

template <class T>
struct HasInterfaceOptions<T, typename EnableIfHasInterfaceOptions<typename T::InterfaceOptions>::Type>
{
    static const bool Value = true;
};

template <class T>
constexpr bool hasInterfaceOptions()
{
    return HasInterfaceOptions<T>::Value;
}

template <class T, class R = void>
struct EnableIfHasImplOptions { using Type = R; };

template <class T, class Enable = void>
struct HasImplOptions
{
    static const bool Value = false;
};

template <class T>
struct HasImplOptions<T, typename EnableIfHasImplOptions<typename T::ImplOptions>::Type>
{
    static const bool Value = true;
};

template <class T>
constexpr bool hasImplOptions()
{
    return HasImplOptions<T>::Value;
}

template <class T, class R = void>
struct EnableIfHasElementType { using Type = R; };

template <class T, class Enable = void>
struct HasElementType
{
    static const bool Value = false;
};

template <class T>
struct HasElementType<T, typename EnableIfHasElementType<typename T::element_type>::Type>
{
    static const bool Value = true;
};

template <class T>
constexpr bool hasElementType()
{
    return HasElementType<T>::Value;
}

} // namespace details

} // namespace comms
