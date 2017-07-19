//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
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

#if !defined(CC_MOVE_UNIQUE_PTR_ON_RETURN) && defined(__clang__)
#if (__clang_major__ < 4) && (__clang_minor__ <= 8)
#define CC_MOVE_UNIQUE_PTR_ON_RETURN
#endif // #if (__clang_major__ < 4) && (__clang_minor__ <= 8)
#endif // #if !defined(CC_MOVE_UNIQUE_PTR_ON_RETURN) && defined(__clang__)


#if !defined(CC_MOVE_UNIQUE_PTR_ON_RETURN) && !defined(__clang__) && defined(__GNUC__)
#if __GNUC__ < 5
#define CC_MOVE_UNIQUE_PTR_ON_RETURN
#endif // #if __GNUC__ < 5
#endif // #if !defined(CC_MOVE_UNIQUE_PTR_ON_RETURN) && !defined(__clang__) && defined(__GNUC__)

#ifdef CC_MOVE_UNIQUE_PTR_ON_RETURN
#define CC_RET_UNIQUE_PTR(p_) std::move(p_)
#else // #ifdef CC_MOVE_UNIQUE_PTR_ON_RETURN
#define CC_RET_UNIQUE_PTR(p_) p_
#endif // #ifdef CC_MOVE_UNIQUE_PTR_ON_RETURN


