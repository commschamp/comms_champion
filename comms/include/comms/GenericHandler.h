//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
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
///          virtual TRetType handle(ActualMessageType& msg);
///          @endcode
///          All the handling functions will upcast the message to TDefault and
///          call the default message handling function with signature:
///          @code
///          virtual TRetType handle(TDefault& msg);
///          @endcode
///          which does nothing. To override the handling behaviour just inherit
///          your handler from comms::GenericHandler and override the appropriate
///          function.
/// @tparam TDefault Base class of all custom messages bundled in TAll.
/// @tparam TAll All message types bundled in std::tuple that need to
///         be handled.
/// @tparam TRetType Return type of the implemented handle() functions.
/// @pre TAll is any variation of std::tuple
/// @pre TDefault is a base class for all the custom messages in TAll.
/// @note The default destructor is @b NOT virtual. To allow polymorphic delete
///     make sure to declare the destructor virtual in the inherited class.
/// @headerfile comms/GenericHandler.h
template <typename TDefault, typename TAll, typename TRetType = void>
class GenericHandler
{
    static_assert(util::IsTuple<TAll>::Value,
                  "TAll must be std::tuple");

#ifdef FOR_DOXYGEN_DOC_ONLY
public:
    /// @brief Return type of every handle() member function.
    using RetType = TRetType;

    /// @brief Handle message object
    /// @details Does nothing, can be overridden in the derived class.
    virtual TRetType handle(TDefault& msg);

protected:
    /// @brief Destructor
    /// @details Although there are virtual functions, the destructor is @b NOT
    ///     virtual. The protected destructor prevents typedef of @ref GenericHandler
    ///     and use it as actual handler class. To allow polymorphic delete
    ///     (destruction) make sure to declare the inherited destructor as
    ///     virtual.
    ~GenericHandler() noexcept = default;
#endif
};

/// @cond SKIP_DOC
template <
    typename TDefault,
    typename T1,
    typename T2,
    typename T3,
    typename T4,
    typename T5,
    typename T6,
    typename T7,
    typename T8,
    typename T9,
    typename T10,
    typename... TRest,
    typename TRetType>
class GenericHandler<TDefault, std::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, TRest...>, TRetType>
                    : public GenericHandler<TDefault, std::tuple<TRest...>, TRetType>
{
    using BaseImpl = GenericHandler<TDefault, std::tuple<TRest...>, TRetType>;
public:

    using BaseImpl::handle;
    virtual TRetType handle(T1& msg)
    {
        static_assert(std::is_base_of<TDefault, T1>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T2& msg)
    {
        static_assert(std::is_base_of<TDefault, T2>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T3& msg)
    {
        static_assert(std::is_base_of<TDefault, T3>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T4& msg)
    {
        static_assert(std::is_base_of<TDefault, T4>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T5& msg)
    {
        static_assert(std::is_base_of<TDefault, T5>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T6& msg)
    {
        static_assert(std::is_base_of<TDefault, T6>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T7& msg)
    {
        static_assert(std::is_base_of<TDefault, T7>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T8& msg)
    {
        static_assert(std::is_base_of<TDefault, T8>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T9& msg)
    {
        static_assert(std::is_base_of<TDefault, T9>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T10& msg)
    {
        static_assert(std::is_base_of<TDefault, T10>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

protected:
    ~GenericHandler() noexcept = default;
};

namespace details
{

template <typename TDefault, typename TRetType>
class GenericHandlerBase
{
public:
    using RetType = TRetType;

    virtual TRetType handle(TDefault& msg)
    {
        // Nothing to do
        static_cast<void>(msg);
        using Tag =
            typename std::conditional<
                std::is_void<TRetType>::value,
                VoidReturnTag,
                typename std::conditional<
                    std::is_lvalue_reference<TRetType>::value,
                    ReferenceReturnTag,
                    ValueReturnTag
                >::type
            >::type;
        return defaultHandle(Tag());
    }

private:
    struct VoidReturnTag {};
    struct ReferenceReturnTag {};
    struct ValueReturnTag {};

    void defaultHandle(VoidReturnTag)
    {
    }

    TRetType defaultHandle(ReferenceReturnTag)
    {
        static typename std::decay<TRetType>::type Value;
        return Value;
    }

    TRetType defaultHandle(ValueReturnTag)
    {
        return typename std::decay<TRetType>::type();
    }

};

} // namespace details

template <
    typename TDefault,
    typename T1,
    typename T2,
    typename T3,
    typename T4,
    typename T5,
    typename T6,
    typename T7,
    typename T8,
    typename T9,
    typename TRetType>
class GenericHandler<TDefault, std::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9>, TRetType> : public
    details::GenericHandlerBase<TDefault, TRetType>
{
    using BaseImpl = details::GenericHandlerBase<TDefault, TRetType>;
public:

    using BaseImpl::handle;
    virtual TRetType handle(T1& msg)
    {
        static_assert(std::is_base_of<TDefault, T1>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T2& msg)
    {
        static_assert(std::is_base_of<TDefault, T2>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T3& msg)
    {
        static_assert(std::is_base_of<TDefault, T3>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T4& msg)
    {
        static_assert(std::is_base_of<TDefault, T4>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T5& msg)
    {
        static_assert(std::is_base_of<TDefault, T5>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T6& msg)
    {
        static_assert(std::is_base_of<TDefault, T6>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T7& msg)
    {
        static_assert(std::is_base_of<TDefault, T7>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T8& msg)
    {
        static_assert(std::is_base_of<TDefault, T8>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T9& msg)
    {
        static_assert(std::is_base_of<TDefault, T9>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

protected:
    ~GenericHandler() noexcept = default;
};

template <
    typename TDefault,
    typename T1,
    typename T2,
    typename T3,
    typename T4,
    typename T5,
    typename T6,
    typename T7,
    typename T8,
    typename TRetType>
class GenericHandler<TDefault, std::tuple<T1, T2, T3, T4, T5, T6, T7, T8>, TRetType> : public
    details::GenericHandlerBase<TDefault, TRetType>
{
    using BaseImpl = details::GenericHandlerBase<TDefault, TRetType>;
public:

    using BaseImpl::handle;
    virtual TRetType handle(T1& msg)
    {
        static_assert(std::is_base_of<TDefault, T1>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T2& msg)
    {
        static_assert(std::is_base_of<TDefault, T2>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T3& msg)
    {
        static_assert(std::is_base_of<TDefault, T3>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T4& msg)
    {
        static_assert(std::is_base_of<TDefault, T4>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T5& msg)
    {
        static_assert(std::is_base_of<TDefault, T5>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T6& msg)
    {
        static_assert(std::is_base_of<TDefault, T6>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T7& msg)
    {
        static_assert(std::is_base_of<TDefault, T7>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T8& msg)
    {
        static_assert(std::is_base_of<TDefault, T8>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

protected:
    ~GenericHandler() noexcept = default;
};

template <
    typename TDefault,
    typename T1,
    typename T2,
    typename T3,
    typename T4,
    typename T5,
    typename T6,
    typename T7,
    typename TRetType>
class GenericHandler<TDefault, std::tuple<T1, T2, T3, T4, T5, T6, T7>, TRetType> : public
    details::GenericHandlerBase<TDefault, TRetType>
{
    using BaseImpl = details::GenericHandlerBase<TDefault, TRetType>;
public:

    using BaseImpl::handle;
    virtual TRetType handle(T1& msg)
    {
        static_assert(std::is_base_of<TDefault, T1>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T2& msg)
    {
        static_assert(std::is_base_of<TDefault, T2>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T3& msg)
    {
        static_assert(std::is_base_of<TDefault, T3>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T4& msg)
    {
        static_assert(std::is_base_of<TDefault, T4>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T5& msg)
    {
        static_assert(std::is_base_of<TDefault, T5>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T6& msg)
    {
        static_assert(std::is_base_of<TDefault, T6>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T7& msg)
    {
        static_assert(std::is_base_of<TDefault, T7>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }


protected:
    ~GenericHandler() noexcept = default;
};

template <
    typename TDefault,
    typename T1,
    typename T2,
    typename T3,
    typename T4,
    typename T5,
    typename T6,
    typename TRetType>
class GenericHandler<TDefault, std::tuple<T1, T2, T3, T4, T5, T6>, TRetType> : public
    details::GenericHandlerBase<TDefault, TRetType>
{
    using BaseImpl = details::GenericHandlerBase<TDefault, TRetType>;
public:

    using BaseImpl::handle;
    virtual TRetType handle(T1& msg)
    {
        static_assert(std::is_base_of<TDefault, T1>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T2& msg)
    {
        static_assert(std::is_base_of<TDefault, T2>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T3& msg)
    {
        static_assert(std::is_base_of<TDefault, T3>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T4& msg)
    {
        static_assert(std::is_base_of<TDefault, T4>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T5& msg)
    {
        static_assert(std::is_base_of<TDefault, T5>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T6& msg)
    {
        static_assert(std::is_base_of<TDefault, T6>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }


protected:
    ~GenericHandler() noexcept = default;
};

template <
    typename TDefault,
    typename T1,
    typename T2,
    typename T3,
    typename T4,
    typename T5,
    typename TRetType>
class GenericHandler<TDefault, std::tuple<T1, T2, T3, T4, T5>, TRetType> : public
    details::GenericHandlerBase<TDefault, TRetType>
{
    using BaseImpl = details::GenericHandlerBase<TDefault, TRetType>;
public:

    using BaseImpl::handle;
    virtual TRetType handle(T1& msg)
    {
        static_assert(std::is_base_of<TDefault, T1>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T2& msg)
    {
        static_assert(std::is_base_of<TDefault, T2>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T3& msg)
    {
        static_assert(std::is_base_of<TDefault, T3>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T4& msg)
    {
        static_assert(std::is_base_of<TDefault, T4>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T5& msg)
    {
        static_assert(std::is_base_of<TDefault, T5>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }


protected:
    ~GenericHandler() noexcept = default;
};

template <
    typename TDefault,
    typename T1,
    typename T2,
    typename T3,
    typename T4,
    typename TRetType>
class GenericHandler<TDefault, std::tuple<T1, T2, T3, T4>, TRetType> : public
    details::GenericHandlerBase<TDefault, TRetType>
{
    using BaseImpl = details::GenericHandlerBase<TDefault, TRetType>;
public:

    using BaseImpl::handle;
    virtual TRetType handle(T1& msg)
    {
        static_assert(std::is_base_of<TDefault, T1>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T2& msg)
    {
        static_assert(std::is_base_of<TDefault, T2>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T3& msg)
    {
        static_assert(std::is_base_of<TDefault, T3>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T4& msg)
    {
        static_assert(std::is_base_of<TDefault, T4>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

protected:
    ~GenericHandler() noexcept = default;
};

template <typename TDefault, typename T1, typename T2, typename T3, typename TRetType>
class GenericHandler<TDefault, std::tuple<T1, T2, T3>, TRetType> : public
    details::GenericHandlerBase<TDefault, TRetType>
{
    using BaseImpl = details::GenericHandlerBase<TDefault, TRetType>;
public:

    using BaseImpl::handle;
    virtual TRetType handle(T1& msg)
    {
        static_assert(std::is_base_of<TDefault, T1>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T2& msg)
    {
        static_assert(std::is_base_of<TDefault, T2>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T3& msg)
    {
        static_assert(std::is_base_of<TDefault, T3>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

protected:
    ~GenericHandler() noexcept = default;
};

template <typename TDefault, typename T1, typename T2, typename TRetType>
class GenericHandler<TDefault, std::tuple<T1, T2>, TRetType> : public
    details::GenericHandlerBase<TDefault, TRetType>
{
    using BaseImpl = details::GenericHandlerBase<TDefault, TRetType>;
public:

    using BaseImpl::handle;
    virtual TRetType handle(T1& msg)
    {
        static_assert(std::is_base_of<TDefault, T1>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

    virtual TRetType handle(T2& msg)
    {
        static_assert(std::is_base_of<TDefault, T2>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

protected:
    ~GenericHandler() noexcept = default;
};

template <typename TDefault, typename T1, typename TRetType>
class GenericHandler<TDefault, std::tuple<T1>, TRetType> : public
    details::GenericHandlerBase<TDefault, TRetType>
{
    using BaseImpl = details::GenericHandlerBase<TDefault, TRetType>;
public:

    using BaseImpl::handle;
    virtual TRetType handle(T1& msg)
    {
        static_assert(std::is_base_of<TDefault, T1>::value,
            "TDefault must be base class for every element in TAll");

        return this->handle(static_cast<TDefault&>(msg));
    }

protected:
    ~GenericHandler() noexcept = default;
};

template <typename TDefault, typename TRetType>
class GenericHandler<TDefault, std::tuple<>, TRetType> : public
    details::GenericHandlerBase<TDefault, TRetType>
{
    using BaseImpl = details::GenericHandlerBase<TDefault, TRetType>;
public:
    using BaseImpl::handle;
protected:
    ~GenericHandler() noexcept = default;
};

/// @endcond

}  // namespace comms

