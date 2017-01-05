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

#include <tuple>
#include <type_traits>

#include "gen_enum.h"

#define COMMS_WRAP_TUP_ELEM(t_, i_, n_) \
    typename std::add_lvalue_reference< \
        typename std::tuple_element<std::tuple_size<t_>::value - i_, t_>::type \
    >::type n_;

#define COMMS_TUP_ELEM_0(t_)
#define COMMS_TUP_ELEM_1(t_, n_) COMMS_WRAP_TUP_ELEM(t_, 1, n_)
#define COMMS_TUP_ELEM_2(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 2, n_) COMMS_EXPAND(COMMS_TUP_ELEM_1(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_3(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 3, n_) COMMS_EXPAND(COMMS_TUP_ELEM_2(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_3(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 3, n_) COMMS_EXPAND(COMMS_TUP_ELEM_2(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_4(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 4, n_) COMMS_EXPAND(COMMS_TUP_ELEM_3(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_5(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 5, n_) COMMS_EXPAND(COMMS_TUP_ELEM_4(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_6(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 6, n_) COMMS_EXPAND(COMMS_TUP_ELEM_5(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_7(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 7, n_) COMMS_EXPAND(COMMS_TUP_ELEM_6(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_8(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 8, n_) COMMS_EXPAND(COMMS_TUP_ELEM_7(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_9(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 9, n_) COMMS_EXPAND(COMMS_TUP_ELEM_8(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_10(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 10, n_) COMMS_EXPAND(COMMS_TUP_ELEM_9(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_11(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 11, n_) COMMS_EXPAND(COMMS_TUP_ELEM_10(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_12(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 12, n_) COMMS_EXPAND(COMMS_TUP_ELEM_11(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_13(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 13, n_) COMMS_EXPAND(COMMS_TUP_ELEM_12(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_14(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 14, n_) COMMS_EXPAND(COMMS_TUP_ELEM_13(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_15(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 15, n_) COMMS_EXPAND(COMMS_TUP_ELEM_14(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_16(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 16, n_) COMMS_EXPAND(COMMS_TUP_ELEM_15(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_17(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 17, n_) COMMS_EXPAND(COMMS_TUP_ELEM_16(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_18(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 18, n_) COMMS_EXPAND(COMMS_TUP_ELEM_17(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_19(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 19, n_) COMMS_EXPAND(COMMS_TUP_ELEM_18(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_20(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 20, n_) COMMS_EXPAND(COMMS_TUP_ELEM_19(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_21(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 21, n_) COMMS_EXPAND(COMMS_TUP_ELEM_20(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_22(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 22, n_) COMMS_EXPAND(COMMS_TUP_ELEM_21(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_23(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 23, n_) COMMS_EXPAND(COMMS_TUP_ELEM_22(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_24(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 24, n_) COMMS_EXPAND(COMMS_TUP_ELEM_23(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_25(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 25, n_) COMMS_EXPAND(COMMS_TUP_ELEM_24(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_26(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 26, n_) COMMS_EXPAND(COMMS_TUP_ELEM_25(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_27(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 27, n_) COMMS_EXPAND(COMMS_TUP_ELEM_26(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_28(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 28, n_) COMMS_EXPAND(COMMS_TUP_ELEM_27(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_29(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 29, n_) COMMS_EXPAND(COMMS_TUP_ELEM_28(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_30(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 30, n_) COMMS_EXPAND(COMMS_TUP_ELEM_29(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_31(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 31, n_) COMMS_EXPAND(COMMS_TUP_ELEM_30(t_, __VA_ARGS__))
#define COMMS_TUP_ELEM_32(t_, n_, ...) \
    COMMS_WRAP_TUP_ELEM(t_, 32, n_) COMMS_EXPAND(COMMS_TUP_ELEM_31(t_, __VA_ARGS__))

#define COMMS_CHOOSE_TUP_ELEM_(N, t_, ...) COMMS_EXPAND(COMMS_TUP_ELEM_ ## N(t_, __VA_ARGS__))
#define COMMS_CHOOSE_TUP_ELEM(N, t_, ...) COMMS_EXPAND(COMMS_CHOOSE_TUP_ELEM_(N, t_, __VA_ARGS__))
#define COMMS_DO_TUP_ELEM(t_, ...) \
    COMMS_EXPAND(COMMS_CHOOSE_TUP_ELEM(COMMS_NUM_ARGS(__VA_ARGS__), t_, __VA_ARGS__))

#define COMMS_TUP_ACC(t_, i_) std::get<i_>(t_)
#define COMMS_TUP_ACC_0(...)
#define COMMS_TUP_ACC_1(t_) COMMS_TUP_ACC(t_, 0)
#define COMMS_TUP_ACC_2(t_) COMMS_TUP_ACC_1(t_), COMMS_TUP_ACC(t_, 1)
#define COMMS_TUP_ACC_3(t_) COMMS_TUP_ACC_2(t_), COMMS_TUP_ACC(t_, 2)
#define COMMS_TUP_ACC_4(t_) COMMS_TUP_ACC_3(t_), COMMS_TUP_ACC(t_, 3)
#define COMMS_TUP_ACC_5(t_) COMMS_TUP_ACC_4(t_), COMMS_TUP_ACC(t_, 4)
#define COMMS_TUP_ACC_6(t_) COMMS_TUP_ACC_5(t_), COMMS_TUP_ACC(t_, 5)
#define COMMS_TUP_ACC_7(t_) COMMS_TUP_ACC_6(t_), COMMS_TUP_ACC(t_, 6)
#define COMMS_TUP_ACC_8(t_) COMMS_TUP_ACC_7(t_), COMMS_TUP_ACC(t_, 7)
#define COMMS_TUP_ACC_9(t_) COMMS_TUP_ACC_8(t_), COMMS_TUP_ACC(t_, 8)
#define COMMS_TUP_ACC_10(t_) COMMS_TUP_ACC_9(t_), COMMS_TUP_ACC(t_, 9)
#define COMMS_TUP_ACC_11(t_) COMMS_TUP_ACC_10(t_), COMMS_TUP_ACC(t_, 10)
#define COMMS_TUP_ACC_12(t_) COMMS_TUP_ACC_11(t_), COMMS_TUP_ACC(t_, 11)
#define COMMS_TUP_ACC_13(t_) COMMS_TUP_ACC_12(t_), COMMS_TUP_ACC(t_, 12)
#define COMMS_TUP_ACC_14(t_) COMMS_TUP_ACC_13(t_), COMMS_TUP_ACC(t_, 13)
#define COMMS_TUP_ACC_15(t_) COMMS_TUP_ACC_14(t_), COMMS_TUP_ACC(t_, 14)
#define COMMS_TUP_ACC_16(t_) COMMS_TUP_ACC_15(t_), COMMS_TUP_ACC(t_, 15)
#define COMMS_TUP_ACC_17(t_) COMMS_TUP_ACC_16(t_), COMMS_TUP_ACC(t_, 16)
#define COMMS_TUP_ACC_18(t_) COMMS_TUP_ACC_17(t_), COMMS_TUP_ACC(t_, 17)
#define COMMS_TUP_ACC_19(t_) COMMS_TUP_ACC_18(t_), COMMS_TUP_ACC(t_, 18)
#define COMMS_TUP_ACC_20(t_) COMMS_TUP_ACC_19(t_), COMMS_TUP_ACC(t_, 19)
#define COMMS_TUP_ACC_21(t_) COMMS_TUP_ACC_20(t_), COMMS_TUP_ACC(t_, 20)
#define COMMS_TUP_ACC_22(t_) COMMS_TUP_ACC_21(t_), COMMS_TUP_ACC(t_, 21)
#define COMMS_TUP_ACC_23(t_) COMMS_TUP_ACC_22(t_), COMMS_TUP_ACC(t_, 22)
#define COMMS_TUP_ACC_24(t_) COMMS_TUP_ACC_23(t_), COMMS_TUP_ACC(t_, 23)
#define COMMS_TUP_ACC_25(t_) COMMS_TUP_ACC_24(t_), COMMS_TUP_ACC(t_, 24)
#define COMMS_TUP_ACC_26(t_) COMMS_TUP_ACC_25(t_), COMMS_TUP_ACC(t_, 25)
#define COMMS_TUP_ACC_27(t_) COMMS_TUP_ACC_26(t_), COMMS_TUP_ACC(t_, 26)
#define COMMS_TUP_ACC_28(t_) COMMS_TUP_ACC_27(t_), COMMS_TUP_ACC(t_, 27)
#define COMMS_TUP_ACC_29(t_) COMMS_TUP_ACC_28(t_), COMMS_TUP_ACC(t_, 28)
#define COMMS_TUP_ACC_30(t_) COMMS_TUP_ACC_29(t_), COMMS_TUP_ACC(t_, 29)
#define COMMS_TUP_ACC_31(t_) COMMS_TUP_ACC_30(t_), COMMS_TUP_ACC(t_, 30)
#define COMMS_TUP_ACC_32(t_) COMMS_TUP_ACC_31(t_), COMMS_TUP_ACC(t_, 31)

#define COMMS_CHOOSE_TUP_ACC_(N, t_) COMMS_TUP_ACC_ ## N(t_)
#define COMMS_CHOOSE_TUP_ACC(N, t_) COMMS_CHOOSE_TUP_ACC_(N, t_)
#define COMMS_DO_TUP_ACC(t_, ...) COMMS_EXPAND(COMMS_CHOOSE_TUP_ACC(COMMS_NUM_ARGS(__VA_ARGS__), t_))

#define COMMS_DEFINE_FIELD_ENUM(...) COMMS_EXPAND(COMMS_DEFINE_ENUM(FieldIdx, __VA_ARGS__));

#define COMMS_DEFINE_FIELDS_STRUCT(t_, ...) \
    static_assert(std::tuple_size<t_>::value == COMMS_NUM_ARGS(__VA_ARGS__), \
        "Invalid number of arguments to the macro"); \
    struct FieldsAsStruct { \
        COMMS_EXPAND(COMMS_DO_TUP_ELEM(t_, __VA_ARGS__)) \
    };

#define COMMS_DEFINE_CONST_FIELDS_STRUCT(t_, ...) \
    static_assert(std::tuple_size<t_>::value == COMMS_EXPAND(COMMS_NUM_ARGS(__VA_ARGS__)), \
        "Invalid number of arguments to the macro"); \
    struct ConstFieldsAsStruct { \
        COMMS_EXPAND(COMMS_DO_TUP_ELEM(const t_, __VA_ARGS__)) \
    };

#define COMMS_DEFINE_FIELDS_STRUCT_ACCESS(t_, ...) \
    FieldsAsStruct fieldsAsStruct() \
    { \
        static_assert(std::tuple_size<typename std::decay<decltype(t_)>::type>::value == COMMS_NUM_ARGS(__VA_ARGS__), \
            "Invalid number of arguments to the macro"); \
        return FieldsAsStruct{COMMS_DO_TUP_ACC(t_, __VA_ARGS__)}; \
    }

#define COMMS_DEFINE_CONST_FIELDS_STRUCT_ACCESS(t_, ...) \
    ConstFieldsAsStruct fieldsAsStruct() const \
    { \
        static_assert(std::tuple_size<typename std::decay<decltype(t_)>::type>::value == COMMS_NUM_ARGS(__VA_ARGS__), \
            "Invalid number of arguments to the macro"); \
        return ConstFieldsAsStruct{COMMS_EXPAND(COMMS_DO_TUP_ACC(t_, __VA_ARGS__))}; \
    }

#define COMMS_FIELDS_ACCESS_ALL(T_, t_, ...) \
    static_assert(0U < std::tuple_size<T_>::value, \
        "Cannot use this macro with empty tuple of fields"); \
    COMMS_EXPAND(COMMS_DEFINE_FIELD_ENUM(__VA_ARGS__)) \
    COMMS_EXPAND(COMMS_DEFINE_FIELDS_STRUCT(T_, __VA_ARGS__)) \
    COMMS_EXPAND(COMMS_DEFINE_CONST_FIELDS_STRUCT(T_, __VA_ARGS__)) \
    COMMS_EXPAND(COMMS_DEFINE_FIELDS_STRUCT_ACCESS(t_, __VA_ARGS__)) \
    COMMS_EXPAND(COMMS_DEFINE_CONST_FIELDS_STRUCT_ACCESS(t_, __VA_ARGS__)) \
    static_assert(std::tuple_size<T_>::value == FieldIdx_numOfValues, \
        "Number of expected fields is incorrect");

















