//
// Copyright 2019 (C). Alex Robenko. All rights reserved.
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

#define COMMS_ALIAS_MEM_ACC(m_) COMMS_CONCATENATE(field_, m_)()

#define COMMS_ALIAS_MEM_ACC_1(m_) COMMS_ALIAS_MEM_ACC(m_)
#define COMMS_ALIAS_MEM_ACC_2(m_, ...) COMMS_ALIAS_MEM_ACC(m_) . COMMS_ALIAS_MEM_ACC_1(__VA_ARGS__)
#define COMMS_ALIAS_MEM_ACC_3(m_, ...) COMMS_ALIAS_MEM_ACC(m_) . COMMS_ALIAS_MEM_ACC_2(__VA_ARGS__)
#define COMMS_ALIAS_MEM_ACC_4(m_, ...) COMMS_ALIAS_MEM_ACC(m_) . COMMS_ALIAS_MEM_ACC_3(__VA_ARGS__)
#define COMMS_ALIAS_MEM_ACC_5(m_, ...) COMMS_ALIAS_MEM_ACC(m_) . COMMS_ALIAS_MEM_ACC_4(__VA_ARGS__)
#define COMMS_ALIAS_MEM_ACC_6(m_, ...) COMMS_ALIAS_MEM_ACC(m_) . COMMS_ALIAS_MEM_ACC_5(__VA_ARGS__)
#define COMMS_ALIAS_MEM_ACC_7(m_, ...) COMMS_ALIAS_MEM_ACC(m_) . COMMS_ALIAS_MEM_ACC_6(__VA_ARGS__)
#define COMMS_ALIAS_MEM_ACC_8(m_, ...) COMMS_ALIAS_MEM_ACC(m_) . COMMS_ALIAS_MEM_ACC_7(__VA_ARGS__)

#define COMMS_ALIAS_ALL_MEM_ACC_(N, ...) COMMS_EXPAND(COMMS_ALIAS_MEM_ACC_ ## N(__VA_ARGS__))
#define COMMS_ALIAS_ALL_MEM_ACC(N, ...) COMMS_EXPAND(COMMS_ALIAS_ALL_MEM_ACC_(N, __VA_ARGS__))
#define COMMS_DO_ALIAS_ALL_MEM_ACC(...) \
    COMMS_EXPAND(COMMS_ALIAS_ALL_MEM_ACC(COMMS_NUM_ARGS(__VA_ARGS__), __VA_ARGS__))


#define COMMS_DO_ALIAS(f1_, ...) \
    auto COMMS_CONCATENATE(field_, f1_) () -> decltype(COMMS_DO_ALIAS_ALL_MEM_ACC(__VA_ARGS__)) \
    { \
        return COMMS_DO_ALIAS_ALL_MEM_ACC(__VA_ARGS__); \
    }\
    auto COMMS_CONCATENATE(field_, f1_) () const -> decltype(COMMS_DO_ALIAS_ALL_MEM_ACC(__VA_ARGS__)) \
    { \
        return COMMS_DO_ALIAS_ALL_MEM_ACC(__VA_ARGS__); \
    }
