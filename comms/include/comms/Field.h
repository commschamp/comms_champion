//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
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

/// @file
/// @brief Contains definition of @ref comms::Field class.

#pragma once

#include <type_traits>

#include "util/access.h"
#include "details/FieldBase.h"
#include "comms/details/macro_common.h"
#include "comms/details/fields_access.h"

namespace comms
{

/// @brief Base class to all the field classes.
/// @details Every custom "field" class should inherit from this one.
/// @tparam TOptions Zero or more options. The supported options are:
///     @li comms::option::def::BigEndian or comms::option::def::LittleEndian - Option to
///         specify serialisation endian. If none is provided "Big" endian is
///         assumed.
/// @headerfile comms/Field.h
template <typename... TOptions>
class Field : public details::FieldBase<TOptions...>
{
    using BaseImpl = details::FieldBase<TOptions...>;
public:
    /// @brief Endian type
    /// @details Equal to either @ref comms::traits::endian::Big or
    ///     @ref comms::traits::endian::Little
    using Endian = typename BaseImpl::Endian;

    /// @brief Version type
    using VersionType = typename BaseImpl::VersionType;

    /// @brief Default validity check
    /// @details Always returns true, can be overriden by the derived class
    /// @return Always @b true
    static constexpr bool valid()
    {
        return true;
    }

    /// @brief Default refresh functionality
    /// @details Does nothing and returns false, can be overriden by the
    ///     derived class
    /// @return Always @b false
    static constexpr bool refresh()
    {
        return false;
    }

    /// @brief Default check of whether the field is version dependent.
    /// @return Always @b false.
    static constexpr bool isVersionDependent()
    {
        return false;
    }

    /// @brief Default check of whether the field has defines refresh functionality
    /// @return Always @b false.
    static constexpr bool hasNonDefaultRefresh()
    {
        return false;
    }

    /// @brief Default version update functionality
    /// @details Does nothing and returns false, can be overriden by the
    ///     derived class
    /// @return Always @b false
    static constexpr bool setVersion(VersionType)
    {
        return false;
    }

protected:
    /// @brief Write data into the output buffer.
    /// @details Use this function to write data to the the buffer
    ///          maintained by the caller. The endianness of the data will be
    ///          as specified in the options provided to the class.
    /// @tparam T Type of the value to write. Must be integral.
    /// @tparam Type of output iterator
    /// @param[in] value Integral type value to be written.
    /// @param[in, out] iter Output iterator.
    /// @pre The iterator must be valid and can be successfully dereferenced
    ///      and incremented at least sizeof(T) times.
    /// @post The iterator is advanced.
    /// @note Thread safety: Safe for distinct buffers, unsafe otherwise.
    template <typename T, typename TIter>
    static void writeData(T value, TIter& iter)
    {
        writeData<sizeof(T), T>(value, iter);
    }

    /// @brief Write partial data into the output buffer.
    /// @details Use this function to write partial data to the buffer maintained
    ///          by the caller. The endianness of the data will be as specified
    ///          the class options.
    /// @tparam TSize Length of the value in bytes known in compile time.
    /// @tparam T Type of the value to write. Must be integral.
    /// @tparam TIter Type of output iterator
    /// @param[in] value Integral type value to be written.
    /// @param[in, out] iter Output iterator.
    /// @pre TSize <= sizeof(T)
    /// @pre The iterator must be valid and can be successfully dereferenced
    ///      and incremented at least TSize times.
    /// @post The iterator is advanced.
    /// @note Thread safety: Safe for distinct buffers, unsafe otherwise.
    template <std::size_t TSize, typename T, typename TIter>
    static void writeData(T value, TIter& iter)
    {
        static_assert(TSize <= sizeof(T),
                                    "Cannot put more bytes than type contains");
        return util::writeData<TSize, T>(value, iter, Endian());
    }

    /// @brief Read data from input buffer.
    /// @details Use this function to read data from the intput buffer maintained
    ///     by the caller. The endianness of the data will be as specified in
    ///     options of the class.
    /// @tparam T Return type
    /// @tparam TIter Type of input iterator
    /// @param[in, out] iter Input iterator.
    /// @return The integral type value.
    /// @pre TSize <= sizeof(T)
    /// @pre The iterator must be valid and can be successfully dereferenced
    ///      and incremented at least sizeof(T) times.
    /// @post The iterator is advanced.
    /// @note Thread safety: Safe for distinct stream buffers, unsafe otherwise.
    template <typename T, typename TIter>
    static T readData(TIter& iter)
    {
        return readData<T, sizeof(T)>(iter);
    }

    /// @brief Read partial data from input buffer.
    /// @details Use this function to read data from the intput buffer maintained
    ///     by the caller. The endianness of the data will be as specified in
    ///     options of the class.
    /// @tparam T Return type
    /// @tparam TSize number of bytes to read
    /// @tparam TIter Type of input iterator
    /// @param[in, out] iter Input iterator.
    /// @return The integral type value.
    /// @pre TSize <= sizeof(T)
    /// @pre The iterator must be valid and can be successfully dereferenced
    ///      and incremented at least TSize times.
    /// @post The internal pointer of the stream buffer is advanced.
    /// @note Thread safety: Safe for distinct stream buffers, unsafe otherwise.
    template <typename T, std::size_t TSize, typename TIter>
    static T readData(TIter& iter)
    {
        static_assert(TSize <= sizeof(T),
            "Cannot get more bytes than type contains");
        return util::readData<T, TSize>(iter, Endian());
    }

};

/// @brief Add convenience access enum and functions to the members of
///     bundle fields, such as comms::field::Bundle or comms::field::Bitfield.
/// @details The fields of "bundle" types, such as comms::field::Bundle or
///     comms::field::Bitfield keep their members bundled in
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>
///     and provide access to them via @b value() member functions.
///     The access to the specific member field can be obtained using
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple/get">std::get</a>
///     later on:
///     @code
///     using MyFieldBase = comms::Field<comms::option::def::BigEndian>;
///     using ... Field1;
///     using ... Field2;
///     using ... Field3;
///     using MyField =
///         comms::field::Bitfield<
///             MyFieldBase,
///             std::tuple<Field1, Field2, Field3>
///         >;
///
///     MyField field;
///     auto& members = field.value();
///     auto& firstMember = std::get<0>(members);
///     auto& secondMember = std::get<1>(members);
///     auto& thirdMember = std::get<2>(members);
///     @endcode
///     However, it would be convenient to provide names and easier access to
///     the member fields. The COMMS_FIELD_MEMBERS_ACCESS() macro does exactly
///     that when used inside the field class definition. Just inherit from
///     the "bundle" field and use the macro inside with the names for the
///     member fields:
///     @code
///     class MyField : public comms::field::Bitfield<...>
///     {
///     public:
///         COMMS_FIELD_MEMBERS_ACCESS(member1, member2, member3);
///     }
///     @endcode
///     It would be equivalent to having the following types and functions
///     definitions:
///     @code
///     class MyField : public comms::field::Bitfield<...>
///     {
///     public:
///         // Access indices for member fields
///         enum FieldIdx {
///             FieldIdx_member1,
///             FieldIdx_member2,
///             FieldIdx_member3,
///             FieldIdx_numOfValues
///         };
///
///         // Accessor to "member1" member field.
///         auto field_member1() -> decltype(std::get<FieldIdx_member1>(value()))
///         {
///             return std::get<FieldIdx_member1>(value());
///         }
///
///         // Accessor to const "member1" member field.
///         auto field_member1() const -> decltype(std::get<FieldIdx_member1>(value()))
///         {
///             return std::get<FieldIdx_member1>(value());
///         }
///
///         // Accessor to "member2" member field.
///         auto field_member2() -> decltype(std::get<FieldIdx_member2>(value()))
///         {
///             return std::get<FieldIdx_member2>(value());
///         }
///
///         // Accessor to const "member2" member field.
///         auto field_member2() const -> decltype(std::get<FieldIdx_member2>(value()))
///         {
///             return std::get<FieldIdx_member2>(value());
///         }
///
///         // Accessor to "member3" member field.
///         auto field_member3() -> decltype(std::get<FieldIdx_member2>(value()))
///         {
///             return std::get<FieldIdx_member3>(value());
///         }
///
///         // Accessor to const "member3" member field.
///         auto field_member2() const -> decltype(std::get<FieldIdx_member3>(value()))
///         {
///             return std::get<FieldIdx_member3>(value());
///         }
///     };
///     @endcode
///     @b NOTE, that provided names @b member1, @b member2, and @b member3, have
///         found their way to the following definitions:
///     @li @b FieldIdx enum. The names are prefixed with @b FieldIdx_. The
///         @b FieldIdx_nameOfValues value is automatically added at the end.
///     @li Accessor functions prefixed with @b field_
///
///     See @ref sec_field_tutorial_bitfield for more examples and details
/// @param[in] ... List of member fields' names.
/// @related comms::field::Bitfield
/// @warning Some compilers, such as @b clang or early versions of @b g++
///     may have problems compiling code generated by this macro even
///     though it uses valid C++11 constructs in attempt to automatically identify the
///     type of the base class. If the compilation fails,
///     and this macro resides inside a @b NON-template class, please use
///     @ref COMMS_FIELD_MEMBERS_ACCESS_NOTEMPLATE() macro instead. In
///     case this macro needs to reside inside a @b template class, then
///     there is a need to define inner @b Base type, which specifies
///     exact type of the @ref comms::field::Bitfield class. For example:
///     @code
///     template <typename... TExtraOptions>
///     class MyField : public
///         comms::field::Bitfield<
///             MyFieldBase,
///             std::tuple<Field1, Field2, Field3>,
///             TExtraOptions...
///         >
///     {
///         // Define type of the base class
///         using Base =
///             comms::field::Bitfield<
///                 MyFieldBase,
///                 std::tuple<Field1, Field2, Field3>,
///                 TExtraOptions...
///             >;
///     public:
///         COMMS_FIELD_MEMBERS_ACCESS(member1, member2, member3);
///     };
///     @endcode
#define COMMS_FIELD_MEMBERS_ACCESS(...) \
    COMMS_EXPAND(COMMS_DEFINE_FIELD_ENUM(__VA_ARGS__)) \
    COMMS_FIELD_VALUE_ACCESS_FUNC { \
        auto& val = comms::field::toFieldBase(*this).value(); \
        using AllFieldsTuple = typename std::decay<decltype(val)>::type; \
        static_assert(std::tuple_size<AllFieldsTuple>::value == FieldIdx_numOfValues, \
            "Invalid number of names for fields tuple"); \
        return val; \
    } \
    COMMS_FIELD_VALUE_ACCESS_CONST_FUNC { \
        auto& val = comms::field::toFieldBase(*this).value(); \
        using AllFieldsTuple = typename std::decay<decltype(val)>::type; \
        static_assert(std::tuple_size<AllFieldsTuple>::value == FieldIdx_numOfValues, \
            "Invalid number of names for fields tuple"); \
        return val; \
    } \
    COMMS_EXPAND(COMMS_DO_FIELD_ACC_FUNC(ValueType, value(), __VA_ARGS__))

/// @brief Similar to @ref COMMS_FIELD_MEMBERS_ACCESS(), but dedicated for
///     non-template classes.
/// @details The @ref COMMS_FIELD_MEMBERS_ACCESS() macro is a generic one,
///     which can be used in any class (template, or non-template). However,
///     some compilers (such as <b>g++-4.9</b> and below, @b clang-4.0 and below) may fail
///     to compile it even though it uses valid C++11 constructs. If the
///     compilation fails and the class it is being used in is @b NOT a
///     template one, please use @ref COMMS_FIELD_MEMBERS_ACCESS_NOTEMPLATE()
///     instead.
/// @related comms::field::Bitfield
#define COMMS_FIELD_MEMBERS_ACCESS_NOTEMPLATE(...) \
    COMMS_EXPAND(COMMS_DEFINE_FIELD_ENUM(__VA_ARGS__)) \
    COMMS_EXPAND(COMMS_DO_FIELD_ACC_FUNC_NOTEMPLATE(__VA_ARGS__))


#ifdef FOR_DOXYGEN_DOC_ONLY
/// @brief Add convenience access enum and functions to the members of
///     bundle fields, such as comms::field::Bundle or comms::field::Bitfield.
/// @details The fields of "bundle" types, such as comms::field::Bundle or
///     comms::field::Bitfield keep their members bundled in
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>
///     and provide access to them via @b value() member functions.
///     The access to the specific member field can be obtained using
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple/get">std::get</a>
///     later on:
///     @code
///     using MyFieldBase = comms::Field<comms::option::def::BigEndian>;
///     using ... Field1;
///     using ... Field2;
///     using ... Field3;
///     using MyField =
///         comms::field::Bundle<
///             MyFieldBase,
///             std::tuple<Field1, Field2, Field3>
///         >;
///
///     MyField field;
///     auto& members = field.value();
///     auto& firstMember = std::get<0>(members);
///     auto& secondMember = std::get<1>(members);
///     auto& thirdMember = std::get<2>(members);
///     @endcode
///     However, it would be convenient to provide names and easier access to
///     the member fields. The COMMS_FIELD_MEMBERS_ACCESS() macro does exaclty
///     that when used inside the field class definition. Just inherit from
///     the "bundle" field and use the macro inside with names for the
///     member fields:
///     @code
///     class MyField : public comms::field::Bundle<...>
///     {
///     public:
///         COMMS_FIELD_MEMBERS_ACCESS(member1, member2, member3);
///     }
///     @endcode
///     It would be equivalent to having the following types and functions
///     definitions:
///     @code
///     class MyField : public comms::field::Bundle<...>
///     {
///     public:
///         // Access indices for member fields
///         enum FieldIdx {
///             FieldIdx_member1,
///             FieldIdx_member2,
///             FieldIdx_member3,
///             FieldIdx_numOfValues
///         };
///
///         // Accessor to "member1" member field.
///         auto field_member1() -> decltype(std::get<FieldIdx_member1>(value()))
///         {
///             return std::get<FieldIdx_member1>(value());
///         }
///
///         // Accessor to const "member1" member field.
///         auto field_member1() const -> decltype(std::get<FieldIdx_member1>(value()))
///         {
///             return std::get<FieldIdx_member1>(value());
///         }
///
///         // Accessor to "member2" member field.
///         auto field_member2() -> decltype(std::get<FieldIdx_member2>(value()))
///         {
///             return std::get<FieldIdx_member2>(value());
///         }
///
///         // Accessor to const "member2" member field.
///         auto field_member2() const -> decltype(std::get<FieldIdx_member2>(value()))
///         {
///             return std::get<FieldIdx_member2>(value());
///         }
///
///         // Accessor to "member3" member field.
///         auto field_member3() -> decltype(std::get<FieldIdx_member2>(value()))
///         {
///             return std::get<FieldIdx_member3>(value());
///         }
///
///         // Accessor to const "member3" member field.
///         auto field_member2() const -> decltype(std::get<FieldIdx_member3>(value()))
///         {
///             return std::get<FieldIdx_member3>(value());
///         }
///     };
///     @endcode
///     @b NOTE, that provided names @b member1, @b member2, and @b member3, have
///         found their way to the following definitions:
///     @li @b FieldIdx enum. The names are prefixed with @b FieldIdx_. The
///         @b FieldIdx_nameOfValues value is automatically added at the end.
///     @li Accessor functions prefixed with @b field_
///
///     See @ref sec_field_tutorial_bundle for more examples and details
/// @param[in] ... List of member fields' names.
/// @related comms::field::Bundle
/// @warning Some compilers, such as @b clang or early versions of @b g++
///     may have problems compiling code generated by this macro even
///     though it uses valid C++11 constructs in attempt to automatically identify the
///     type of the base class. If the compilation fails,
///     and this macro resides inside a @b NON-template class, please use
///     @ref COMMS_FIELD_MEMBERS_ACCESS_NOTEMPLATE() macro instead. In
///     case this macro needs to reside inside a @b template class, then
///     there is a need to define inner @b Base type, which specifies
///     exact type of the @ref comms::field::Bundle class. For example:
///     @code
///     template <typename... TExtraOptions>
///     class MyField : public
///         comms::field::Bundle<
///             MyFieldBase,
///             std::tuple<Field1, Field2, Field3>,
///             TExtraOptions...
///         >
///     {
///         // Define type of the base class
///         using Base =
///             comms::field::Bundle<
///                 MyFieldBase,
///                 std::tuple<Field1, Field2, Field3>,
///                 TExtraOptions...
///             >;
///     public:
///         COMMS_FIELD_MEMBERS_ACCESS(member1, member2, member3);
///     };
///     @endcode
#define COMMS_FIELD_MEMBERS_ACCESS(...)

/// @brief Similar to @ref COMMS_FIELD_MEMBERS_ACCESS(), but dedicated for
///     non-template classes.
/// @details The @ref COMMS_FIELD_MEMBERS_ACCESS() macro is a generic one,
///     which can be used in any class (template, or non-template). However,
///     some compilers (such as <b>g++-4.9</b> and below, @b clang-4.0 and below) may fail
///     to compile it even though it uses valid C++11 constructs. If the
///     compilation fails and the class it is being used in is @b NOT a
///     template one, please use @ref COMMS_FIELD_MEMBERS_ACCESS_NOTEMPLATE()
///     instead.
/// @related comms::field::Bundle
#define COMMS_FIELD_MEMBERS_ACCESS_NOTEMPLATE(...) \
    COMMS_EXPAND(COMMS_DEFINE_FIELD_ENUM(__VA_ARGS__)) \
    COMMS_EXPAND(COMMS_DO_FIELD_ACC_FUNC_NOTEMPLATE(__VA_ARGS__))
#endif // #ifdef FOR_DOXYGEN_DOC_ONLY

}  // namespace comms

