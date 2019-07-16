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

#include "comms/Message.h"
#include "comms/details/detect.h"

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
        typename std::conditional<
            TMsg::hasWrite(),
            HasWriteIterTag,
            NoWriteIterTag
        >::type;

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
        typename std::conditional<
            comms::isMessage<TMsg>(),
            WriteIteratorMsgObjHelper<TMsg>,
            typename std::conditional<
                hasElementType<TMsg>(),
                WriteIteratorSmartPtrHelper<TMsg>,
                WriteIteratorCastHelper
            >::type
        >::type;
public:
    template <typename TIter>
    static auto get(TIter&& iter) -> decltype(Helper::get(std::forward<TIter>(iter)))
    {
        return Helper::get(std::forward<TIter>(iter));
    }
};

} // namespace details

} // namespace comms
