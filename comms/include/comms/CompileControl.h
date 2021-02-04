//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// Contains various compiler related definitions

#pragma once

#ifdef __GNUC__

#define GCC_DIAG_STR(s) #s
#define GCC_DIAG_JOINSTR(x,y) GCC_DIAG_STR(x ## y)
#define GCC_DIAG_DO_PRAGMA(x) _Pragma (#x)
#define GCC_DIAG_PRAGMA(x) GCC_DIAG_DO_PRAGMA(GCC diagnostic x)
#define CC_DISABLE_WARNINGS() \
    GCC_DIAG_PRAGMA(push) \
    GCC_DIAG_PRAGMA(ignored "-Wpedantic") \
    GCC_DIAG_PRAGMA(ignored "-Wctor-dtor-privacy")\
    GCC_DIAG_PRAGMA(ignored "-Wold-style-cast") \
    GCC_DIAG_PRAGMA(ignored "-Wconversion")

#define CC_ENABLE_WARNINGS() GCC_DIAG_PRAGMA(pop)

#define COMMS_GNU_WARNING_PUSH GCC_DIAG_PRAGMA(push)
#define COMMS_GNU_WARNING_DISABLE(x_) GCC_DIAG_PRAGMA(ignored x_)
#define COMMS_GNU_WARNING_POP GCC_DIAG_PRAGMA(pop)

#else

#define CC_DISABLE_WARNINGS()
#define CC_ENABLE_WARNINGS()
#define COMMS_GNU_WARNING_PUSH
#define COMMS_GNU_WARNING_DISABLE(x_)
#define COMMS_GNU_WARNING_POP

#endif


#define COMMS_IS_MSVC false
#define COMMS_IS_GCC  false
#define COMMS_IS_CLANG false
#define COMMS_IS_USING_GNUC false

#if defined(__GNUC__)
#undef COMMS_IS_USING_GNUC
#define COMMS_IS_USING_GNUC true
#endif // #if defined(__GNUC__) && !defined(__clang__)

#if defined(_MSC_VER) && !defined(__clang__)
#undef COMMS_IS_MSVC
#define COMMS_IS_MSVC true
#endif // #if defined(_MSC_VER) && !defined(__clang__)

#if defined(__GNUC__) && !defined(__clang__)
#undef COMMS_IS_GCC
#define COMMS_IS_GCC true
#endif // #if defined(__GNUC__) && !defined(__clang__)

#if defined(__clang__)

#undef COMMS_IS_CLANG
#define COMMS_IS_CLANG true
#endif // #if defined(__clang__)

#define COMMS_IS_GCC_47_OR_BELOW (COMMS_IS_GCC && (__GNUC__ == 4) && (__GNUC_MINOR__ < 8))
#define COMMS_IS_GCC_9 (COMMS_IS_GCC && (__GNUC__ == 9))
#define COMMS_IS_GCC_9_OR_BELOW (COMMS_IS_GCC_9 && (__GNUC__ <= 9))
#define COMMS_IS_GCC_9_OR_ABOVE (COMMS_IS_GCC_9 && (__GNUC__ >= 9))
#define COMMS_IS_GCC_10 (COMMS_IS_GCC && (__GNUC__ == 10))
#define COMMS_IS_GCC_10_OR_ABOVE (COMMS_IS_GCC && (__GNUC__ >= 10))
#define COMMS_IS_CLANG_7_OR_ABOVE (COMMS_IS_CLANG && (__clang_major__ >= 7))
#define COMMS_IS_CLANG_8 (COMMS_IS_CLANG && (__clang_major__ == 8))
#define COMMS_IS_CLANG_8_OR_BELOW (COMMS_IS_CLANG && (__clang_major__ <= 8))
#define COMMS_IS_CLANG_9_OR_BELOW (COMMS_IS_CLANG && (__clang_major__ <= 9))
#define COMMS_IS_CLANG_9_OR_ABOVE (COMMS_IS_CLANG && (__clang_major__ >= 9))
#define COMMS_IS_MSVC_2019 (COMMS_IS_MSVC && (_MSC_VER >= 1920) && (_MSC_VER < 1930))
#define COMMS_IS_MSVC_2019_OR_BELOW (COMMS_IS_MSVC && (_MSC_VER < 1930))
#define COMMS_IS_MSVC_2017_OR_BELOW (COMMS_IS_MSVC && (_MSC_VER < 1920))
#define COMMS_IS_MSVC_2015_OR_BELOW (COMMS_IS_MSVC && (_MSC_VER < 1910))

#if !defined(CC_COMPILER_GCC47) && COMMS_IS_GCC_47_OR_BELOW
#define CC_COMPILER_GCC47
#endif

#define COMMS_IS_CPP14 (__cplusplus >= 201402L)
#define COMMS_IS_CPP17 (__cplusplus >= 201703L)
#define COMMS_IS_CPP20 (__cplusplus >= 202002L)

#if COMMS_IS_MSVC_2019_OR_BELOW // Visual Studio 2019
#undef COMMS_IS_CPP20
#define COMMS_IS_CPP20 (__cplusplus >= 201704L)
#endif

#if COMMS_IS_CLANG_9_OR_BELOW
#undef COMMS_IS_CPP20
#define COMMS_IS_CPP20 (__cplusplus >= 201709L)
#endif

#if COMMS_IS_CLANG_8_OR_BELOW
#undef COMMS_IS_CPP20
#define COMMS_IS_CPP20 (__cplusplus >= 201707L)
#endif

#define COMMS_CLANG_HAS_STRING_VIEW false
#define COMMS_CLANG_HAS_VERSION_HEADER false

#if COMMS_IS_CLANG

// The defines below are seperate because VS2015 doesn't 
// behave well with angle brackets inside macro arguments.

#undef COMMS_CLANG_HAS_STRING_VIEW
#define COMMS_CLANG_HAS_STRING_VIEW (__has_include(<string_view>))

#undef COMMS_CLANG_HAS_VERSION_HEADER
#define COMMS_CLANG_HAS_VERSION_HEADER (__has_include(<version>))

#endif // #if COMMS_IS_CLANG


#define COMMS_HAS_CPP20_VERSION_HEADER \
    COMMS_IS_CPP20 && \
    (\
        (COMMS_IS_USING_GNUC && (__GNUC__ >= 9)) || \
        (COMMS_IS_CLANG && COMMS_CLANG_HAS_VERSION_HEADER) || \
        (COMMS_IS_MSVC && (_MSC_VER >= 1922)) \
    )

#if COMMS_HAS_CPP20_VERSION_HEADER
#include <version>
#endif

#define COMMS_HAS_CPP17_STRING_VIEW  false

#ifndef COMMS_NO_CPP17_STRING_VIEW 
#undef COMMS_HAS_CPP17_STRING_VIEW
#define COMMS_HAS_CPP17_STRING_VIEW \
    COMMS_IS_CPP17 && \
    (\
        (COMMS_IS_USING_GNUC && (__GNUC__ >= 7)) || \
        (COMMS_IS_CLANG && COMMS_CLANG_HAS_STRING_VIEW) || \
        (COMMS_IS_MSVC && (_MSC_VER >= 1910)) \
    )

#endif // #ifndef COMMS_NO_CPP17_STRING_VIEW 

#define COMMS_HAS_CPP20_SPAN false
#if !defined(COMMS_NO_CPP20_SPAN) && COMMS_IS_CPP20 && defined(__cpp_lib_span)
#undef COMMS_HAS_CPP20_SPAN
#define COMMS_HAS_CPP20_SPAN true
#endif // #if COMMS_IS_CPP20 && defined(__cpp_lib_span)

#if COMMS_IS_MSVC

#define COMMS_MSVC_WARNING_PRAGMA(s_) __pragma(s_)
#define COMMS_MSVC_WARNING_PUSH __pragma(warning(push))
#define COMMS_MSVC_WARNING_POP __pragma(warning(pop))
#define COMMS_MSVC_WARNING_DISABLE(w_) __pragma(warning(disable:w_))
#define COMMS_MSVC_WARNING_SUPPRESS(w_) __pragma(warning(suppress:w_))

#else // #if COMMS_IS_MSVC

#define COMMS_MSVC_WARNING_PRAGMA(s_)
#define COMMS_MSVC_WARNING_PUSH
#define COMMS_MSVC_WARNING_POP
#define COMMS_MSVC_WARNING_DISABLE(w_)
#define COMMS_MSVC_WARNING_SUPPRESS(w_)

#endif // #if COMMS_IS_MSVC




