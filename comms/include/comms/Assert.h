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

/// @file comms/Assert.h
/// This file contains classes required for generic custom assertion
/// functionality

#pragma once

#include <cassert>
#include <type_traits>
#include <utility>

namespace comms
{

/// @brief Base class for any custom assertion behaviour.
/// @details In order to implement custom assertion failure behaviour it
///          is necessary to inherit from this class and override
///          fail() virtual member function.
/// @headerfile comms/Assert.h
class Assert
{
public:
    /// @brief Destructor
    virtual ~Assert() noexcept {}

    /// @brief Pure virtual function to be called when assertion fails.
    /// @param[in] expr Assertion condition/expression
    /// @param[in] file File name
    /// @param[in] line Line number of the assert statement.
    /// @param[in] function Function name.
    virtual void fail(
        const char* expr,
        const char* file,
        unsigned int line,
        const char* function) = 0;

private:
};

/// @cond DOCUMENT_ASSERT_MANAGER
class AssertManager
{
public:

    static AssertManager& instance()
    {
        static AssertManager mgr;
        return mgr;
    }

    AssertManager(const AssertManager&) = delete;

    AssertManager& operator=(const AssertManager&) = delete;

    Assert* reset(Assert* newAssert = nullptr)
    {
        auto prevAssert = assert_;
        assert_ = newAssert;
        return prevAssert;
    }

    Assert* getAssert()
    {
        return assert_;
    }

    bool hasAssertRegistered() const
    {
        return (assert_ != nullptr);
    }

    static void infiniteLoop()
    {
        while (true) {};
    }

private:
    AssertManager() : assert_(nullptr) {}

    Assert* assert_;
};

/// @endcond

/// @brief Enable new assertion behaviour.
/// @details Instantiate object of this class to enable new behaviour of
///          assertion failure.
/// @tparam TAssert Class derived from Assert that implements new custom
///                 behaviour of the assertion failure.
/// @pre TAssert class must be derived from comms::Assert.
/// @headerfile comms/Assert.h
template < typename TAssert>
class EnableAssert
{
    static_assert(std::is_base_of<Assert, TAssert>::value,
        "TAssert class must be derived class of Assert");
public:
    /// Type of assert object.
    using AssertType = TAssert;

    /// @brief Constructor
    /// @details Registers new assertion failure behaviour. It forwards
    ///          all the provided parameters to the constructor of embedded
    ///          assertion object of type TAssert.
    /// @param args Arguments to pass to the assertion class constructor.
    template<typename... TParams>
    EnableAssert(TParams&&... args)
        : assert_(std::forward<TParams>(args)...),
          prevAssert_(AssertManager::instance().reset(&assert_))
    {
    }

    /// @brief Destructor
    /// @details Restores the assertion behaviour that was recorded during
    ///          the instantiation of this object.
    ~EnableAssert() noexcept
    {
        AssertManager::instance().reset(prevAssert_);
    }


    /// @brief Provides reference to internal Assert object
    /// @return Reference to object of type TAssert.
    AssertType& getAssert()
    {
        return assert_;
    }

private:
    AssertType assert_;
    Assert* prevAssert_;
};


#ifndef COMMS_ASSERT
#ifndef NDEBUG

/// @cond DOCUCMENT_AM_ASSERT_FUNCTION
#ifndef __ASSERT_FUNCTION
#define COMMS_ASSERT_FUNCTION_STR __FUNCTION__
#else // #ifndef __ASSERT_FUNCTION
#define COMMS_ASSERT_FUNCTION_STR __ASSERT_FUNCTION
#endif // #ifndef __ASSERT_FUNCTION

#ifndef NOSTDLIB
#define COMMS_ASSERT_FAIL_FUNC(expr) assert(expr)
#else // #ifndef NOSTDLIB
#define COMMS_ASSERT_FAIL_FUNC(expr) comms::AssertManager::instance().infiniteLoop()
#endif // #ifndef NOSTDLIB

/// @endcond

/// @brief Generic assert macro
/// @details Will use custom assertion failure behaviour if such is defined,
///          otherwise it will use standard "assert()" macro.
///          In case NOSTDLIB is defined and no custom assertion failure was
///          enabled, infinite loop will be executed.
/// @param expr Boolean expression
#define COMMS_ASSERT(expr) \
    ((expr)                               \
      ? static_cast<void>(0)                     \
      : (comms::AssertManager::instance().hasAssertRegistered() \
            ? comms::AssertManager::instance().getAssert()->fail( \
                #expr, __FILE__, __LINE__, COMMS_ASSERT_FUNCTION_STR) \
            : COMMS_ASSERT_FAIL_FUNC(expr)))

#else // #ifndef NDEBUG

#define COMMS_ASSERT(expr) static_cast<void>(0)

#endif // #ifndef NDEBUG
#endif // #ifndef COMMS_ASSERT

#ifndef GASSERT
/// @brief Same as @ref COMMS_ASSERT
/// @details Kept for backward compatibility of already written protocols.
#define GASSERT(expr) COMMS_ASSERT(expr)
#endif // #ifndef GASSERT

}  // namespace comms
