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

#define COMMS_EXPAND(x_) x_
#define COMMS_CONCATENATE_(x_, y_) x_##y_
#define COMMS_CONCATENATE(x_, y_) COMMS_CONCATENATE_(x_, y_)

#if __cplusplus < 201402L
#define FUNC_AUTO_REF_RETURN(name_, ret_) auto name_ () -> ret_
#define FUNC_AUTO_REF_RETURN_CONST(name_, ret_) auto name_ () const -> ret_
#define FUNC_ARGS_AUTO_REF_RETURN(name_, args_, ret_) auto name_(args_) -> ret_
#define FUNC_ARGS_AUTO_REF_RETURN_CONST(name_, args_, ret_) auto name_(args_) const -> ret_
#else // #if __cplusplus < 201402L
#define FUNC_AUTO_REF_RETURN(name_, ret_) decltype(auto) name_ ()
#define FUNC_AUTO_REF_RETURN_CONST(name_, ret_) decltype(auto) name_ () const
#define FUNC_ARGS_AUTO_REF_RETURN(name_, args_, ret_) decltype(auto) name_(args_) -> ret_
#define FUNC_ARGS_AUTO_REF_RETURN_CONST(name_, args_, ret_) decltype(auto) name_(args_) const -> ret_
#endif // #if __cplusplus < 201402L
