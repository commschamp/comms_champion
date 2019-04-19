//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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
#include <utility>

namespace comms
{

namespace details
{

// VS2015 does NOT support expressions SFINAE, will use it for later versions.

//template <typename... TArgs>
//using VoidT = void;

//template <typename TVoid, template <class...> class TOp, typename... TArgs>
//struct PresenceDetector
//{
//    static const bool Value = false;
//};

//template <template <class...> class TOp, typename... TArgs>
//struct PresenceDetector<VoidT<TOp<TArgs...> >, TOp, TArgs...>
//{
//    static const bool Value = true;
//};

//template <template <class...> class TOp, typename... TArgs>
//constexpr bool isDetected()
//{
//    return PresenceDetector<void, TOp, TArgs...>::Value;
//}

//template <typename T>
//using HasClearOp = decltype(std::declval<T&>().clear());

//template <typename T>
//using HasReserveOp = decltype(std::declval<T&>().reserve(std::declval<typename T::size_type>()));

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
constexpr bool hasClearFunc()
{
    return HasClearFunc<T>::Value;
}

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
constexpr bool hasReserveFunc()
{
    return HasReserveFunc<T>::Value;
}

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
constexpr bool hasResizeFunc()
{
    return HasResizeFunc<T>::Value;
}

template <typename T>
class HasRemoveSuffixFunc
{
protected:
  typedef char Yes;
  typedef unsigned No;

  template <typename U, U>
  struct ReallyHas;

  template <typename C>
  static Yes test(ReallyHas<void (C::*)(typename C::size_type), &C::remove_suffix>*);
  template <typename>
  static No test(...);

public:
    static const bool Value = (sizeof(test<T>(nullptr)) == sizeof(Yes));
};

template <typename T>
constexpr bool hasRemoveSuffixFunc()
{
    return HasRemoveSuffixFunc<T>::Value;
}

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
