//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
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

#ifdef __GNUC__

#define GCC_DIAG_STR(s) #s
#define GCC_DIAG_JOINSTR(x,y) GCC_DIAG_STR(x ## y)
#define GCC_DIAG_DO_PRAGMA(x) _Pragma (#x)
#define GCC_DIAG_PRAGMA(x) GCC_DIAG_DO_PRAGMA(GCC diagnostic x)
#define CC_DISABLE_WARNINGS() \
    GCC_DIAG_PRAGMA(push) \
    GCC_DIAG_PRAGMA(ignored "-Wpedantic") \
    GCC_DIAG_PRAGMA(ignored "-Wctor-dtor-privacy")\
    GCC_DIAG_PRAGMA(ignored "-Wold-style-cast")

#define CC_ENABLE_WARNINGS() GCC_DIAG_PRAGMA(pop)

#else

#define CC_DISABLE_WARNINGS()
#define CC_ENABLE_WARNINGS()
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
#define COMMS_IS_GCC_10_OR_ABOVE (COMMS_IS_GCC && (__GNUC__ >= 10))
#define COMMS_IS_CLANG_7_OR_ABOVE (COMMS_IS_CLANG && (__clang_major__ >= 7))
#define COMMS_IS_CLANG_8 (COMMS_IS_CLANG && (__clang_major__ == 8))
#define COMMS_IS_CLANG_8_OR_BELOW (COMMS_IS_CLANG && (__clang_major__ <= 8))
#define COMMS_IS_CLANG_9_OR_BELOW (COMMS_IS_CLANG && (__clang_major__ <= 9))
#define COMMS_IS_CLANG_9_OR_ABOVE (COMMS_IS_CLANG && (__clang_major__ >= 9))
#define COMMS_IS_MSVC_2019 (COMMS_IS_MSVC && (_MSC_VER >= 1920) && (_MSC_VER < 1930))
#define COMMS_IS_MSVC_2019_OR_BELOW (COMMS_IS_MSVC && (_MSC_VER < 1930))

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
#define COMMS_CLANG_HAS_SPAN false

#if COMMS_IS_CLANG

// The defines below are seperate because VS2015 doesn't 
// behave well with angle brackets inside macro arguments.

#undef COMMS_CLANG_HAS_STRING_VIEW
#define COMMS_CLANG_HAS_STRING_VIEW (__has_include(<string_view>))

#undef COMMS_CLANG_HAS_SPAN
#define COMMS_CLANG_HAS_SPAN (__has_include(<span>))

#endif // #if COMMS_IS_CLANG


#define COMMS_HAS_CPP17_STRING_VIEW \
    COMMS_IS_CPP17 && \
    (\
        (COMMS_IS_USING_GNUC && (__GNUC__ >= 7)) || \
        (COMMS_IS_CLANG && COMMS_CLANG_HAS_STRING_VIEW) || \
        (COMMS_IS_MSVC && (_MSC_VER >= 1910)) \
    )

#define COMMS_HAS_CPP20_SPAN \
    COMMS_IS_CPP20 && \
    (\
        (COMMS_IS_USING_GNUC && (__GNUC__ >= 10)) || \
        (COMMS_IS_CLANG && COMMS_CLANG_HAS_SPAN) || \
        (COMMS_IS_MSVC && (_MSC_VER >= 1926)) \
    )



