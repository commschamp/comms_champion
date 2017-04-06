//
// Copyright 2016 - 2017 (C). Alex Robenko. All rights reserved.
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

#include <tuple>
#include <type_traits>

#include "macro_common.h"
#include "gen_enum.h"


#define COMMS_FIELD_ACC_FUNC(t_, n_) \
    FUNC_AUTO_REF_RETURN(COMMS_CONCATENATE(field_, n_), decltype(std::get<COMMS_CONCATENATE(FieldIdx_, n_)>(t_))) {\
        return std::get<COMMS_CONCATENATE(FieldIdx_, n_)>(t_); \
    } \
    FUNC_AUTO_REF_RETURN_CONST(COMMS_CONCATENATE(field_, n_), decltype(std::get<COMMS_CONCATENATE(FieldIdx_, n_)>(t_))) {\
        return std::get<COMMS_CONCATENATE(FieldIdx_, n_)>(t_); \
    }

#define COMMS_FIELD_ACC_FUNC_1(t_, n_) COMMS_FIELD_ACC_FUNC(t_, n_)
#define COMMS_FIELD_ACC_FUNC_2(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_1(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_3(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_2(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_4(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_3(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_5(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_4(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_6(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_5(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_7(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_6(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_8(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_7(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_9(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_8(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_10(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_9(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_11(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_10(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_12(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_11(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_13(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_12(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_14(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_13(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_15(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_14(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_16(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_15(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_17(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_16(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_18(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_17(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_19(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_18(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_20(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_19(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_21(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_20(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_22(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_21(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_23(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_22(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_24(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_23(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_25(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_24(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_26(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_25(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_27(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_26(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_28(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_27(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_29(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_28(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_30(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_29(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_31(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_30(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_32(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_31(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_33(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_32(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_34(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_33(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_35(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_34(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_36(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_35(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_37(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_36(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_38(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_37(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_39(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_38(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_40(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_39(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_41(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_40(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_42(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_41(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_43(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_42(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_44(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_43(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_45(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_44(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_46(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_45(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_47(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_46(t_, __VA_ARGS__))
#define COMMS_FIELD_ACC_FUNC_48(t_, n_, ...) \
    COMMS_FIELD_ACC_FUNC(t_, n_) \
    COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_47(t_, __VA_ARGS__))

#define COMMS_CHOOSE_FIELD_ACC_FUNC_(N, t_, ...) COMMS_EXPAND(COMMS_FIELD_ACC_FUNC_ ## N(t_, __VA_ARGS__))
#define COMMS_CHOOSE_FIELD_ACC_FUNC(N, t_, ...) COMMS_EXPAND(COMMS_CHOOSE_FIELD_ACC_FUNC_(N, t_, __VA_ARGS__))
#define COMMS_DO_FIELD_ACC_FUNC(t_, ...) \
    COMMS_EXPAND(COMMS_CHOOSE_FIELD_ACC_FUNC(COMMS_NUM_ARGS(__VA_ARGS__), t_, __VA_ARGS__))

#define COMMS_DEFINE_FIELD_ENUM(...) COMMS_EXPAND(COMMS_DEFINE_ENUM(FieldIdx, __VA_ARGS__));

#define COMMS_FIELDS_ACCESS_ALL(t_, ...) \
    COMMS_EXPAND(COMMS_DEFINE_FIELD_ENUM(__VA_ARGS__)) \
    COMMS_EXPAND(COMMS_DO_FIELD_ACC_FUNC(t_, __VA_ARGS__))














