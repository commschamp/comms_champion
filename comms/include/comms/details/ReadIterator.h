//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <type_traits>
#include "comms/CompileControl.h"
#include "comms/Message.h"
#include "comms/details/detect.h"
#include "comms/util/type_traits.h"

namespace comms
{

namespace details
{

struct ReadIteratorCastHelper
{
    template <typename TIter>
    static auto get(TIter&& iter) -> decltype(std::forward<TIter>(iter))
    {
        return std::forward<TIter>(iter);
    }
};

template <typename TMsg>
class ReadIteratorMsgObjHelper
{
    struct HasReadIterTag {};
    struct NoReadIterTag {};

    using Tag =
        typename comms::util::Conditional<
            TMsg::hasRead()
        >::template Type<
            HasReadIterTag,
            NoReadIterTag
        >;

#ifdef CC_COMPILER_GCC47
    // g++-4.7 complains about accessing private getInternal
public:
#endif

    template <typename TMsgTmp, typename TIter>
    static auto getInternal(TIter&& iter, HasReadIterTag) -> typename TMsgTmp::ReadIterator
    {
        static_assert(std::is_convertible<typename std::decay<decltype(iter)>::type, typename TMsgTmp::ReadIterator>::value,
            "Provided iterator is not convertible to read iterator type used by message interface");
        return typename TMsgTmp::ReadIterator(std::forward<TIter>(iter));
    }

    template <typename TMsgTmp, typename TIter>
    static auto getInternal(TIter&& iter, NoReadIterTag) -> decltype(ReadIteratorCastHelper::get(std::forward<TIter>(iter)))
    {
        return ReadIteratorCastHelper::get(std::forward<TIter>(iter));
    }

public:
    template <typename TIter>
    static auto get(TIter&& iter) -> decltype(getInternal<TMsg>(std::forward<TIter>(iter), Tag()))
    {
        return getInternal<TMsg>(std::forward<TIter>(iter), Tag());
    }
};

template <typename TMsgPtr>
struct ReadIteratorSmartPtrHelper
{
    template <typename TIter>
    static auto get(TIter&& iter) -> decltype(ReadIteratorMsgObjHelper<typename TMsgPtr::element_type>::get(std::forward<TIter>(iter)))
    {
        return ReadIteratorMsgObjHelper<typename TMsgPtr::element_type>::get(std::forward<TIter>(iter));
    }
};

template <typename TMsg>
class ReadIterator
{
    using Helper =
        typename comms::util::Conditional<
            comms::isMessage<TMsg>()
        >::template Type<
            ReadIteratorMsgObjHelper<TMsg>,
            typename comms::util::Conditional<
                hasElementType<TMsg>()
            >::template Type<
                ReadIteratorSmartPtrHelper<TMsg>,
                ReadIteratorCastHelper
            >
        >;
public:
    template <typename TIter>
    static auto get(TIter&& iter) -> decltype(Helper::get(std::forward<TIter>(iter)))
    {
        return Helper::get(std::forward<TIter>(iter));
    }
};

} // namespace details

} // namespace comms
