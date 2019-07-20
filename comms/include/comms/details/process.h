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
