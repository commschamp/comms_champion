//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
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

/// @file comms/GenericHandler.h
/// This file contains definition of common handler.

#pragma once

#include <tuple>
#include <type_traits>

#include "util/Tuple.h"

namespace comms
{

/// @brief Generic common message handler.
/// @details Will define virtual message handling functions for all the
///          messages bundled in TAll plus one to handle TDefault
///          type of message as default behaviour. The declaration of the
///          handling function is as following:
///          @code
///          virtual void handle(ActualMessageType& msg);
///          @endcode
///          All the handling functions will upcast the message to TDefault and
///          call the default message handling function with signature:
///          @code
///          virtual void handle(TDefault& msg);
///          @endcode
///          which does nothing. To override the handling behaviour just inherit
///          your handler from comms::GenericHandler and override the appropriate
///          function.
/// @tparam TDefault Base class of all custom messages bundled in TAll.
/// @tparam TAll All message types bundled in std::tuple that need to
///         be handled.
/// @pre TAll is any variation of std::tuple
/// @pre TDefault is a base class for all the custom messages in TAll.
template <typename TDefault, typename TAll>
class GenericHandler
{
    static_assert(util::IsTuple<TAll>::Value,
                  "TAll must be std::tuple");
};

/// @cond SKIP_DOC
template <typename TDefault, typename TFirst, typename... TRest>
class GenericHandler<TDefault, std::tuple<TFirst, TRest...> > :
                        public GenericHandler<TDefault, std::tuple<TRest...> >
{
    typedef GenericHandler<TDefault, std::tuple<TRest...> > Base;
public:

    virtual ~GenericHandler() {}

    using Base::handle;
    virtual void handle(TFirst& msg)
    {
        static_assert(std::is_base_of<TDefault, TFirst>::value,
            "TDefault must be base class for every element in TAll");

        this->handle(static_cast<TDefault&>(msg));
    }
};

template <typename TDefault>
class GenericHandler<TDefault, std::tuple<> >
{
public:

    virtual ~GenericHandler() {}

    virtual void handle(TDefault& msg)
    {
        // Nothing to do
        static_cast<void>(msg);
    }
};

/// @endcond

}  // namespace comms

