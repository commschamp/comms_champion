//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

/// @file
/// Provides auxiliary functions for retrieving best type of iterator for read/write operations

#include "comms/details/ReadIteratorHelper.h"
#include "comms/details/WriteIteratorHelper.h"

namespace comms
{

/// @brief Create and initialise iterator for message read
/// @tparam TMessage Type of message interface class or smart pointer to it.
/// @param[in] iter Iterator value.
/// @return The same @b iter value, but casted to appropriate type
template <typename TMessage, typename TIter>
auto readIteratorFor(TIter&& iter) -> decltype(details::ReadIteratorHelper<>::template get<TMessage>(std::forward<TIter>(iter)))
{
    return details::ReadIteratorHelper<>::template get<TMessage>(std::forward<TIter>(iter));
}

/// @brief Create and initialise iterator for message read
/// @tparam TMessage Type of message interface class or smart pointer to it.
/// @param[in] msg Unused message object helps detecting proper message type.
/// @param[in] iter Iterator value.
/// @return The same @b iter value, but casted to appropriate type
template <typename TMessage, typename TIter>
auto readIteratorFor(TMessage&& msg, TIter&& iter) ->
    decltype(details::ReadIteratorHelper<>::template get<typename std::decay<decltype(msg)>::type>(std::forward<TIter>(iter)))
{
    static_cast<void>(msg);
    return details::ReadIteratorHelper<>::template get<typename std::decay<decltype(msg)>::type>(std::forward<TIter>(iter));
}

/// @brief Create and initialise iterator for message write
/// @tparam TMessage Type of message interface class or smart pointer to it.
/// @param[in] iter Iterator value.
/// @return The same @b iter value, but casted to appropriate type
template <typename TMessage, typename TIter>
auto writeIteratorFor(TIter&& iter) -> decltype(details::WriteIteratorHelper<>::template get<TMessage>(std::forward<TIter>(iter)))
{
    return details::WriteIteratorHelper<>::template get<TMessage>(std::forward<TIter>(iter));
}

/// @brief Create and initialise iterator for message write
/// @tparam TMessage Type of message interface class or smart pointer to it.
/// @param[in] msg Unused message object helps detecting proper message type.
/// @param[in] iter Iterator value.
/// @return The same @b iter value, but casted to appropriate type
template <typename TMessage, typename TIter>
auto writeIteratorFor(TMessage&& msg, TIter&& iter) ->
    decltype(details::WriteIteratorHelper<>::template get<typename std::decay<decltype(msg)>::type>(std::forward<TIter>(iter)))
{
    static_cast<void>(msg);
    return details::WriteIteratorHelper<>::template get<typename std::decay<decltype(msg)>::type>(std::forward<TIter>(iter));
}

} // namespace comms
