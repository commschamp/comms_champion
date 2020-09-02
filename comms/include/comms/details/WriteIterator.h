//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "comms/Message.h"
#include "comms/details/detect.h"
#include "comms/util/type_traits.h"

namespace comms
{

namespace details
{

struct WriteIteratorCastHelper
{
    template <typename TIter>
    static auto get(TIter&& iter) -> decltype(std::forward<TIter>(iter))
    {
        return std::forward<TIter>(iter);
    }
};

template <typename TMsg>
class WriteIteratorMsgObjHelper
{
    struct HasWriteIterTag {};
    struct NoWriteIterTag {};

    using Tag =
        typename comms::util::Conditional<
            TMsg::hasWrite()
        >::template Type<
            HasWriteIterTag,
            NoWriteIterTag
        >;

#ifdef CC_COMPILER_GCC47
    // g++-4.7 complains about accessing private getInternal
public:
#endif

    template <typename TMsgTmp, typename TIter>
    static auto getInternal(TIter&& iter, HasWriteIterTag) -> typename TMsgTmp::WriteIterator
    {
        static_assert(std::is_convertible<typename std::decay<decltype(iter)>::type, typename TMsgTmp::WriteIterator>::value,
            "Provided iterator is not convertible to write iterator type used by message interface");
        return typename TMsgTmp::WriteIterator(std::forward<TIter>(iter));
    }

    template <typename TMsgTmp, typename TIter>
    static auto getInternal(TIter&& iter, NoWriteIterTag) -> decltype(WriteIteratorCastHelper::get(std::forward<TIter>(iter)))
    {
        return WriteIteratorCastHelper::get(std::forward<TIter>(iter));
    }

public:
    template <typename TIter>
    static auto get(TIter&& iter) -> decltype(getInternal<TMsg>(std::forward<TIter>(iter), Tag()))
    {
        return getInternal<TMsg>(std::forward<TIter>(iter), Tag());
    }
};

template <typename TMsgPtr>
struct WriteIteratorSmartPtrHelper
{
    template <typename TIter>
    static auto get(TIter&& iter) -> decltype(WriteIteratorMsgObjHelper<typename TMsgPtr::element_type>::get(std::forward<TIter>(iter)))
    {
        return WriteIteratorMsgObjHelper<typename TMsgPtr::element_type>::get(std::forward<TIter>(iter));
    }
};

template <typename TMsg>
class WriteIterator
{
    using Helper =
        typename comms::util::Conditional<
            comms::isMessage<TMsg>()
        >::template Type<
            WriteIteratorMsgObjHelper<TMsg>,
            typename comms::util::Conditional<
                hasElementType<TMsg>()
            >::template Type<
                WriteIteratorSmartPtrHelper<TMsg>,
                WriteIteratorCastHelper
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
