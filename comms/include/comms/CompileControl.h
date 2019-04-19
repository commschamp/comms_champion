//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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

#if !defined(CC_COMPILER_GCC47) && !defined(__clang__) && defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ < 8)
#define CC_COMPILER_GCC47
#endif
