//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

#define COMMS_NUM_ARGS_(X,\
    X64,X63,X62,X61,X60,\
    X59,X58,X57,X56,X55,X54,X53,X52,X51,X50,\
    X49,X48,X47,X46,X45,X44,X43,X42,X41,X40,\
    X39,X38,X37,X36,X35,X34,X33,X32,X31,X30,\
    X29,X28,X27,X26,X25,X24,X23,X22,X21,X20,\
    X19,X18,X17,X16,X15,X14,X13,X12,X11,X10,\
    X9,X8,X7,X6,X5,X4,X3,X2,X1,N,...) N
#define COMMS_NUM_ARGS(...) COMMS_EXPAND(COMMS_NUM_ARGS_(0, __VA_ARGS__ ,\
    64,63,62,61,60,\
    59,58,57,56,55,54,53,52,51,50,\
    49,48,47,46,45,44,43,42,41,40,\
    39,38,37,36,35,34,33,32,31,30,\
    29,28,27,26,25,24,23,22,21,20,\
    19,18,17,16,15,14,13,12,11,10,\
    9,8,7,6,5,4,3,2,1,0))

#define COMMS_CHOOSE_ENUM_(N, p_, ...) COMMS_EXPAND(COMMS_ENUM_VAL_ ## N(p_, __VA_ARGS__))
#define COMMS_CHOOSE_ENUM(N, p_, ...) COMMS_EXPAND(COMMS_CHOOSE_ENUM_(N, p_, __VA_ARGS__))
#define COMMS_DO_ENUM(p_, ...) COMMS_EXPAND(COMMS_CHOOSE_ENUM(COMMS_NUM_ARGS(__VA_ARGS__), p_, __VA_ARGS__))

#define COMMS_END_ENUM(p_) COMMS_CONCATENATE(p_, _numOfValues)

#define COMMS_DEFINE_ENUM(p_, ...) \
    enum p_ { \
        COMMS_EXPAND(COMMS_DO_ENUM(COMMS_CONCATENATE(p_, _), __VA_ARGS__)) \
        , COMMS_END_ENUM(p_) \
    };



