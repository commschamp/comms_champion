//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/CompileControl.h"

#define COMMS_EXPAND(x_) x_
#define COMMS_CONCATENATE_(x_, y_) x_##y_
#define COMMS_CONCATENATE(x_, y_) COMMS_CONCATENATE_(x_, y_)

#define COMMS_NUM_ARGS_(X,\
    X129,X128,X127,X126,X125,X124,X123,X122,X121,X120,\
    X119,X118,X117,X116,X115,X114,X113,X112,X111,X110,\
    X109,X108,X107,X106,X105,X104,X103,X102,X101,X100,\
    X99,X98,X97,X96,X95,X94,X93,X92,X91,X90,\
    X89,X88,X87,X86,X85,X84,X83,X82,X81,X80,\
    X79,X78,X77,X76,X75,X74,X73,X72,X71,X70,\
    X69,X68,X67,X66,X65,X64,X63,X62,X61,X60,\
    X59,X58,X57,X56,X55,X54,X53,X52,X51,X50,\
    X49,X48,X47,X46,X45,X44,X43,X42,X41,X40,\
    X39,X38,X37,X36,X35,X34,X33,X32,X31,X30,\
    X29,X28,X27,X26,X25,X24,X23,X22,X21,X20,\
    X19,X18,X17,X16,X15,X14,X13,X12,X11,X10,\
    X9,X8,X7,X6,X5,X4,X3,X2,X1,N,...) N
#define COMMS_NUM_ARGS(...) COMMS_EXPAND(COMMS_NUM_ARGS_(0, __VA_ARGS__ ,\
    129,128,127,126,125,124,123,122,121,120,\
    119,118,117,116,115,114,113,112,111,110,\
    109,108,106,106,105,104,103,102,101,100,\
    99,98,97,96,95,94,93,92,91,90,\
    89,88,87,86,85,84,83,82,81,80,\
    79,78,77,76,75,74,73,72,71,70,\
    69,68,67,66,65,64,63,62,61,60,\
    59,58,57,56,55,54,53,52,51,50,\
    49,48,47,46,45,44,43,42,41,40,\
    39,38,37,36,35,34,33,32,31,30,\
    29,28,27,26,25,24,23,22,21,20,\
    19,18,17,16,15,14,13,12,11,10,\
    9,8,7,6,5,4,3,2,1,0))

#if COMMS_IS_CPP14

#define FUNC_AUTO_REF_RETURN(name_, ret_) decltype(auto) name_ ()
#define FUNC_AUTO_REF_RETURN_CONST(name_, ret_) decltype(auto) name_ () const
#define FUNC_ARGS_AUTO_REF_RETURN(name_, args_, ret_) decltype(auto) name_(args_)
#define FUNC_ARGS_AUTO_REF_RETURN_CONST(name_, args_, ret_) decltype(auto) name_(args_) const

#else // #if COMMS_IS_CPP14
#define FUNC_AUTO_REF_RETURN(name_, ret_) auto name_ () -> ret_
#define FUNC_AUTO_REF_RETURN_CONST(name_, ret_) auto name_ () const -> ret_
#define FUNC_ARGS_AUTO_REF_RETURN(name_, args_, ret_) auto name_(args_) -> ret_
#define FUNC_ARGS_AUTO_REF_RETURN_CONST(name_, args_, ret_) auto name_(args_) const -> ret_

#endif // #if COMMS_IS_CPP14
