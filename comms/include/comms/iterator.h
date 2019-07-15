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

/// @file
/// Provides auxiliary functions for retrieving best type of iterator for read/write operations

#include "comms/details/ReadIterator.h"
#include "comms/details/WriteIterator.h"

namespace comms
{

/// @brief Create and initialise iterator for message read
/// @tparam TMessage Type of message interface class or smart pointer to it.
/// @param[in] iter Iterator value.
/// @return The same @b iter value, but casted to appropriate type
template <typename TMessage, typename TIter>
auto readIteratorFor(TIter&& iter) -> decltype(details::ReadIterator<TMessage>::get(std::forward<TIter>(iter)))
{
    return details::ReadIterator<TMessage>::get(std::forward<TIter>(iter));
}

/// @brief Create and initialise iterator for message read
/// @tparam TMessage Type of message interface class or smart pointer to it.
/// @param[in] msg Unused message object helps detecting proper message type.
/// @param[in] iter Iterator value.
/// @return The same @b iter value, but casted to appropriate type
template <typename TMessage, typename TIter>
auto readIteratorFor(TMessage&& msg, TIter&& iter) ->
    decltype(details::ReadIterator<typename std::decay<decltype(msg)>::type>::get(std::forward<TIter>(iter)))
{
    return details::ReadIterator<typename std::decay<decltype(msg)>::type>::get(std::forward<TIter>(iter));
}

/// @brief Create and initialise iterator for message write
/// @tparam TMessage Type of message interface class or smart pointer to it.
/// @param[in] iter Iterator value.
/// @return The same @b iter value, but casted to appropriate type
template <typename TMessage, typename TIter>
auto writeIteratorFor(TIter&& iter) -> decltype(details::WriteIterator<TMessage>::get(std::forward<TIter>(iter)))
{
    return details::WriteIterator<TMessage>::get(std::forward<TIter>(iter));
}

/// @brief Create and initialise iterator for message write
/// @tparam TMessage Type of message interface class or smart pointer to it.
/// @param[in] msg Unused message object helps detecting proper message type.
/// @param[in] iter Iterator value.
/// @return The same @b iter value, but casted to appropriate type
template <typename TMessage, typename TIter>
auto writeIteratorFor(TMessage&& msg, TIter&& iter) ->
    decltype(details::WriteIterator<typename std::decay<decltype(msg)>::type>::get(std::forward<TIter>(iter)))
{
    return details::WriteIterator<typename std::decay<decltype(msg)>::type>::get(std::forward<TIter>(iter));
}

} // namespace comms
