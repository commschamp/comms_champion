//
// Copyright 2016 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "macro_common.h"

#define COMMS_WRAP_ENUM(p_, v_) COMMS_CONCATENATE(p_, v_)

#define COMMS_ENUM_VAL_0(...)
#define COMMS_ENUM_VAL_1(p_, v_) COMMS_WRAP_ENUM(p_, v_)
#define COMMS_ENUM_VAL_2(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_1(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_3(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_2(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_4(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_3(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_5(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_4(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_6(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_5(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_7(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_6(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_8(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_7(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_9(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_8(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_10(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_9(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_11(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_10(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_12(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_11(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_13(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_12(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_14(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_13(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_15(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_14(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_16(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_15(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_17(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_16(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_18(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_17(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_19(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_18(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_20(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_19(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_21(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_20(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_22(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_21(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_23(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_22(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_24(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_23(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_25(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_24(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_26(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_25(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_27(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_26(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_28(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_27(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_29(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_28(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_30(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_29(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_31(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_30(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_32(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_31(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_33(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_32(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_34(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_33(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_35(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_34(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_36(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_35(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_37(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_36(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_38(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_37(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_39(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_38(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_40(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_39(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_41(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_40(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_42(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_41(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_43(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_42(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_44(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_43(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_45(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_44(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_46(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_45(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_47(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_46(p_, __VA_ARGS__))
#define COMMS_ENUM_VAL_48(p_, v_, ...) COMMS_WRAP_ENUM(p_, v_), COMMS_EXPAND(COMMS_ENUM_VAL_47(p_, __VA_ARGS__))

#define COMMS_CHOOSE_ENUM_(N, p_, ...) COMMS_EXPAND(COMMS_ENUM_VAL_ ## N(p_, __VA_ARGS__))
#define COMMS_CHOOSE_ENUM(N, p_, ...) COMMS_EXPAND(COMMS_CHOOSE_ENUM_(N, p_, __VA_ARGS__))
#define COMMS_DO_ENUM(p_, ...) COMMS_EXPAND(COMMS_CHOOSE_ENUM(COMMS_NUM_ARGS(__VA_ARGS__), p_, __VA_ARGS__))

#define COMMS_END_ENUM(p_) COMMS_CONCATENATE(p_, _numOfValues)

#define COMMS_DEFINE_ENUM(p_, ...) \
    enum p_ { \
        COMMS_EXPAND(COMMS_DO_ENUM(COMMS_CONCATENATE(p_, _), __VA_ARGS__)) \
        , COMMS_END_ENUM(p_) \
    };



