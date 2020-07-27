//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>

#include "comms/Message.h"
#include "comms/details/detect.h"

namespace  comms
{

namespace details
{

template <typename T>
using ProcessMsgDecayType = typename std::decay<T>::type;

template <bool TIsMessage, bool TIsMsgPtr>
struct ProcessMsgIdRetrieveHelper
{
    template <typename T>
    using Type = void;
};

template <>
struct ProcessMsgIdRetrieveHelper<true, false>
{
    template <typename T>
    using Type = comms::MessageIdType<T>;
};

template <>
struct ProcessMsgIdRetrieveHelper<false, true>
{
private:
    template <typename T>
    using ElementType = typename T::element_type;
public:
    template <typename T>
    using Type =
        typename ProcessMsgIdRetrieveHelper<comms::isMessage<ElementType<T> >(),  hasElementType<ElementType<T> >()>::
            template Type<ElementType<T> >;
};

template <typename T>
using ProcessMsgIdType =
    typename ProcessMsgIdRetrieveHelper<comms::isMessage<ProcessMsgDecayType<T> >(), hasElementType<ProcessMsgDecayType<T> >()>::
        template Type<ProcessMsgDecayType<T> >;

\
template <bool TIsMessage, bool TIsMsgPtr>
struct ProcessMsgCastToMsgObjHelper;

template <>
struct ProcessMsgCastToMsgObjHelper<true, false>
{
    template <typename T>
    static auto cast(T& msg) -> decltype(msg)
    {
        return msg;
    }
};

template <>
struct ProcessMsgCastToMsgObjHelper<false, true>
{
    template <typename T>
    static auto cast(T& msg) -> decltype(*msg)
    {
        return *msg;
    }
};

template <typename T>
struct ProcessMsgCastParamPrepareHelper
{
    using DecayedType = ProcessMsgDecayType<T>;
    static const bool IsMessage = comms::isMessage<DecayedType>();
    static const bool IsMsgPtr = hasElementType<DecayedType>();
};

template <typename T>
auto processMsgCastToMsgObj(T& msg) ->
    decltype(ProcessMsgCastToMsgObjHelper<ProcessMsgCastParamPrepareHelper<decltype(msg)>::IsMessage, ProcessMsgCastParamPrepareHelper<decltype(msg)>::IsMsgPtr>::cast(msg))
{
    return ProcessMsgCastToMsgObjHelper<ProcessMsgCastParamPrepareHelper<decltype(msg)>::IsMessage, ProcessMsgCastParamPrepareHelper<decltype(msg)>::IsMsgPtr>::cast(msg);
}

} // namespace details

} // namespace  comms
