//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
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

#include "comms/CompileControl.h"
#include "comms/details/macro_common.h"

#define COMMS_ALIAS_MEM_ACC(prefix_, m_) COMMS_CONCATENATE(prefix_, m_)()

#define COMMS_ALIAS_MEM_ACC_1(prefix_, m_) COMMS_EXPAND(COMMS_ALIAS_MEM_ACC(prefix_, m_))
#define COMMS_ALIAS_MEM_ACC_2(prefix_, m_, ...) \
    COMMS_ALIAS_MEM_ACC(prefix_, m_) . COMMS_EXPAND(COMMS_ALIAS_MEM_ACC_1(field_, __VA_ARGS__)) // using field_ as next prefix on purpose
#define COMMS_ALIAS_MEM_ACC_3(prefix_, m_, ...) \
    COMMS_ALIAS_MEM_ACC(prefix_, m_) . COMMS_EXPAND(COMMS_ALIAS_MEM_ACC_2(field_, __VA_ARGS__)) // using field_ as next prefix on purpose
#define COMMS_ALIAS_MEM_ACC_4(prefix_, m_, ...) \
    COMMS_ALIAS_MEM_ACC(prefix_, m_) . COMMS_EXPAND(COMMS_ALIAS_MEM_ACC_3(field_, __VA_ARGS__)) // using field_ as next prefix on purpose
#define COMMS_ALIAS_MEM_ACC_5(prefix_, m_, ...) \
    COMMS_ALIAS_MEM_ACC(prefix_, m_) . COMMS_EXPAND(COMMS_ALIAS_MEM_ACC_4(field_, __VA_ARGS__)) // using field_ as next prefix on purpose
#define COMMS_ALIAS_MEM_ACC_6(prefix_, m_, ...) \
    COMMS_ALIAS_MEM_ACC(prefix_, m_) . COMMS_EXPAND(COMMS_ALIAS_MEM_ACC_5(field_, __VA_ARGS__)) // using field_ as next prefix on purpose
#define COMMS_ALIAS_MEM_ACC_7(prefix_, m_, ...) \
    COMMS_ALIAS_MEM_ACC(prefix_, m_) . COMMS_EXPAND(COMMS_ALIAS_MEM_ACC_6(field_, __VA_ARGS__)) // using field_ as next prefix on purpose
#define COMMS_ALIAS_MEM_ACC_8(prefix_, m_, ...) \
    COMMS_ALIAS_MEM_ACC(prefix_, m_) . COMMS_EXPAND(COMMS_ALIAS_MEM_ACC_7(field_, __VA_ARGS__)) // using field_ as next prefix on purpose

#define COMMS_ALIAS_ALL_MEM_ACC_(N, prefix_, ...) COMMS_EXPAND(COMMS_ALIAS_MEM_ACC_ ## N(prefix_, __VA_ARGS__))
#define COMMS_ALIAS_ALL_MEM_ACC(N, prefix_, ...) COMMS_EXPAND(COMMS_ALIAS_ALL_MEM_ACC_(N, prefix_, __VA_ARGS__))
#define COMMS_DO_ALIAS_ALL_MEM_ACC(prefix_, ...) \
    COMMS_EXPAND(COMMS_ALIAS_ALL_MEM_ACC(COMMS_NUM_ARGS(__VA_ARGS__), prefix_, __VA_ARGS__))


#if COMMS_IS_CPP14

#define COMMS_DO_ALIAS(prefix_, f1_, ...) \
    decltype(auto) COMMS_CONCATENATE(prefix_, f1_) () \
    { \
        return COMMS_DO_ALIAS_ALL_MEM_ACC(prefix_, __VA_ARGS__); \
    }\
    decltype(auto) COMMS_CONCATENATE(prefix_, f1_) () const \
    { \
        return COMMS_DO_ALIAS_ALL_MEM_ACC(prefix_, __VA_ARGS__); \
    }

#define COMMS_DO_ALIAS_NOTEMPLATE(prefix_, f1_, ...) COMMS_EXPAND(COMMS_DO_ALIAS(prefix_, f1_, __VA_ARGS__))

#else // #if COMMS_IS_CPP14

#ifndef COMMS_DO_ALIAS_RET_TYPE_PREFIX
#define COMMS_DO_ALIAS_RET_TYPE_PREFIX
#endif // #ifndef COMMS_DO_ALIAS_RET_TYPE_PREFIX

#if defined(__GNUC__) && !defined(__clang__)
#if __GNUC__ < 5
#undef COMMS_DO_ALIAS_RET_TYPE_PREFIX
#define COMMS_DO_ALIAS_RET_TYPE_PREFIX this->
#endif // #if __GNUC__ < 5
#endif // #if defined(__GNUC__) && !defined(__clang__)

#define COMMS_DO_ALIAS(prefix_, f1_, ...) \
    auto COMMS_CONCATENATE(prefix_, f1_) () -> decltype(COMMS_DO_ALIAS_RET_TYPE_PREFIX COMMS_DO_ALIAS_ALL_MEM_ACC(prefix_, __VA_ARGS__)) \
    { \
        return COMMS_DO_ALIAS_ALL_MEM_ACC(prefix_, __VA_ARGS__); \
    }\
    auto COMMS_CONCATENATE(prefix_, f1_) () const -> decltype(COMMS_DO_ALIAS_RET_TYPE_PREFIX COMMS_DO_ALIAS_ALL_MEM_ACC(prefix_, __VA_ARGS__)) \
    { \
        return COMMS_DO_ALIAS_ALL_MEM_ACC(prefix_, __VA_ARGS__); \
    }

#define COMMS_DO_ALIAS_NOTEMPLATE(prefix_, f1_, ...) \
    auto COMMS_CONCATENATE(prefix_, f1_) () -> decltype(COMMS_DO_ALIAS_ALL_MEM_ACC(prefix_, __VA_ARGS__)) \
    { \
        return COMMS_DO_ALIAS_ALL_MEM_ACC(prefix_, __VA_ARGS__); \
    }\
    auto COMMS_CONCATENATE(prefix_, f1_) () const -> decltype(COMMS_DO_ALIAS_ALL_MEM_ACC(prefix_, __VA_ARGS__)) \
    { \
        return COMMS_DO_ALIAS_ALL_MEM_ACC(prefix_, __VA_ARGS__); \
    }

#endif // #if COMMS_IS_CPP14

// ----------------------------------------------

#define COMMS_FIELD_ALIAS_TYPE(P_, m_) COMMS_CONCATENATE(P_, m_)

#define COMMS_FIELD_ALIAS_TYPE_1(P_, m_) COMMS_EXPAND(COMMS_FIELD_ALIAS_TYPE(P_, m_))
#define COMMS_FIELD_ALIAS_TYPE_2(P_, m_, ...) \
    COMMS_FIELD_ALIAS_TYPE(P_, m_) :: COMMS_EXPAND(COMMS_FIELD_ALIAS_TYPE_1(Field_, __VA_ARGS__)) // using Field_ as next prefix on purpose
#define COMMS_FIELD_ALIAS_TYPE_3(P_, m_, ...) \
    COMMS_FIELD_ALIAS_TYPE(P_, m_) :: COMMS_EXPAND(COMMS_FIELD_ALIAS_TYPE_2(Field_, __VA_ARGS__)) // using Field_ as next prefix on purpose
#define COMMS_FIELD_ALIAS_TYPE_4(P_, m_, ...) \
    COMMS_FIELD_ALIAS_TYPE(P_, m_) :: COMMS_EXPAND(COMMS_FIELD_ALIAS_TYPE_3(Field_, __VA_ARGS__)) // using Field_ as next prefix on purpose
#define COMMS_FIELD_ALIAS_TYPE_5(P_, m_, ...) \
    COMMS_FIELD_ALIAS_TYPE(P_, m_) :: COMMS_EXPAND(COMMS_FIELD_ALIAS_TYPE_4(Field_, __VA_ARGS__)) // using Field_ as next prefix on purpose
#define COMMS_FIELD_ALIAS_TYPE_6(P_, m_, ...) \
    COMMS_FIELD_ALIAS_TYPE(P_, m_) :: COMMS_EXPAND(COMMS_FIELD_ALIAS_TYPE_5(Field_, __VA_ARGS__)) // using Field_ as next prefix on purpose
#define COMMS_FIELD_ALIAS_TYPE_7(P_, m_, ...) \
    COMMS_FIELD_ALIAS_TYPE(P_, m_) :: COMMS_EXPAND(COMMS_FIELD_ALIAS_TYPE_6(Field_, __VA_ARGS__)) // using Field_ as next prefix on purpose
#define COMMS_FIELD_ALIAS_TYPE_8(P_, m_, ...) \
    COMMS_FIELD_ALIAS_TYPE(P_, m_) :: COMMS_EXPAND(COMMS_FIELD_ALIAS_TYPE_7(Field_, __VA_ARGS__)) // using Field_ as next prefix on purpose

#define COMMS_ALIAS_ALL_TYPES_(N, P_, ...) COMMS_EXPAND(COMMS_FIELD_ALIAS_TYPE_ ## N(P_, __VA_ARGS__))
#define COMMS_ALIAS_ALL_TYPES(N, P_, ...) COMMS_EXPAND(COMMS_ALIAS_ALL_TYPES_(N, P_, __VA_ARGS__))
#define COMMS_DO_ALIAS_ALL_TYPES(P_, ...) \
    COMMS_EXPAND(COMMS_ALIAS_ALL_TYPES(COMMS_NUM_ARGS(__VA_ARGS__), P_, __VA_ARGS__))

#define COMMS_FIELD_ALIAS_TYPE_PREFIX_1
#define COMMS_FIELD_ALIAS_TYPE_PREFIX_2 typename
#define COMMS_FIELD_ALIAS_TYPE_PREFIX_3 typename
#define COMMS_FIELD_ALIAS_TYPE_PREFIX_4 typename
#define COMMS_FIELD_ALIAS_TYPE_PREFIX_5 typename
#define COMMS_FIELD_ALIAS_TYPE_PREFIX_6 typename
#define COMMS_FIELD_ALIAS_TYPE_PREFIX_7 typename
#define COMMS_FIELD_ALIAS_TYPE_PREFIX_8 typename

#define COMMS_FIELD_ALIAS_TYPE_PREFIX_(N) COMMS_FIELD_ALIAS_TYPE_PREFIX_ ## N
#define COMMS_FIELD_ALIAS_TYPE_PREFIX(N) COMMS_EXPAND(COMMS_FIELD_ALIAS_TYPE_PREFIX_(N))

#define COMMS_DO_ALIAS_TYPEDEF(P_, n_, ...) \
    using COMMS_CONCATENATE(P_, n_) = \
        COMMS_EXPAND(COMMS_FIELD_ALIAS_TYPE_PREFIX(COMMS_NUM_ARGS(__VA_ARGS__))) COMMS_EXPAND(COMMS_DO_ALIAS_ALL_TYPES(P_, __VA_ARGS__));
