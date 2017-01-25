//
// Copyright 2014 - 2017 (C). Alex Robenko. All rights reserved.
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

#include "util/access.h"
#include "details/FieldBase.h"
#include "comms/details/fields_access.h"

namespace comms
{

/// @brief Base class to all the field classes.
/// @details Every custom "field" class should inherit from this one.
/// @tparam TOptions Zero or more options. The supported options are:
///     @li comms::option::BigEndian or comms::option::LittleEndian - Option to
///         specify serialisation endian. If none is provided "Big" endian is
///         assumed.
template <typename... TOptions>
class Field : public details::FieldBase<TOptions...>
{
    typedef details::FieldBase<TOptions...> Base;
public:
    /// @brief Endian type
    /// @details Equal to either comms::traits::endian::Big or
    ///     comms::traits::endian::Little
    typedef typename Base::Endian Endian;

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

/// @brief Add convenience access enum, structs and functions to the members of
///     bundle fields, such as comms::field::Bundle or comms::field::Bitfield.
/// @details The fields of "bundle" types, such as comms::field::Bundle or
///     comms::field::Bitfield keep their members bundled in
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>
///     and provide access to them via @b value() member functions.
///     The access to the specific member field can be obtained using
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple/get">std::get</a>
///     later on:
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
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
///     the "bundle" field and use the macro inside while providing the type
///     of the base class as first parameter, followed by the names for the
///     member fields:
///     @code
///     class MyField : public comms::field::Bitfield<...>
///     {
///         typedef comms::field::Bitfield<...> Base;
///     public:
///         COMMS_FIELD_MEMBERS_ACCESS(Base, member1, member2, member3);
///     }
///     @endcode
///     It would be equivalent to having the following types and functions
///     definitions:
///     @code
///     class MyField : public comms::field::Bitfield<...>
///     {
///         typedef comms::field::Bitfield<...> Base;
///     public:
///         // Access indices for member fields
///         enum FieldIdx {
///             FieldIdx_member1,
///             FieldIdx_member2,
///             FieldIdx_member3,
///             FieldIdx_numOfValues
///         };
///
///         // Types of the member fields
///         typedef Field1 Field_member1;
///         typedef Field2 Field_member2;
///         typedef Field3 Field_member3;
///
///         // Access to member fields bundled into struct
///         struct FieldsAsStruct
///         {
///             Field_member1& member1;
///             Field_member2& member2;
///             Field_member3& member3;
///         };
///
///         // Access to const member fields bundled into struct
///         struct ConstFieldsAsStruct
///         {
///             const Field_member1& member1;
///             const Field_member2& member2;
///             const Field_member3& member3;
///         };
///
///         // Get access to member fields bundled into a struct
///         FieldsAsStruct fieldsAsStruct()
///         {
///             return FieldsAsStruct{
///                 std::get<0>(Base::value()),
///                 std::get<1>(Base::value()),
///                 std::get<2>(Base::value())};
///         }
///
///         // Get access to const member fields bundled into a struct
///         ConstFieldsAsStruct fieldsAsStruct() const
///         {
///             return ConstFieldsAsStruct{
///                 std::get<0>(Base::value()),
///                 std::get<1>(Base::value()),
///                 std::get<2>(Base::value())};
///         }
///
///         // Accessor to "member1" member field.
///         Field_member1& field_meber1()
///         {
///             return std::get<0>(Base::value());
///         }
///
///         // Accessor to const "member1" member field.
///         const Field_member1& field_meber1() const
///         {
///             return std::get<0>(Base::value());
///         }
///
///         // Accessor to "member2" member field.
///         Field_member2& field_meber2()
///         {
///             return std::get<1>(Base::value());
///         }
///
///         // Accessor to const "member2" member field.
///         const Field_member2& field_meber2() const
///         {
///             return std::get<1>(Base::value());
///         }
///
///         // Accessor to "member3" member field.
///         Field_member3& field_meber3()
///         {
///             return std::get<2>(Base::value());
///         }
///
///         // Accessor to const "member3" member field.
///         const Field_member3& field_meber3() const
///         {
///             return std::get<2>(Base::value());
///         }
///     };
///     @endcode
///     @b NOTE, that provided names @b baud, @b parity, and @b flags, have
///         found their way to the following definitions:
///     @li @b FieldIdx enum. The names are prefixed with @b FieldIdx_. The
///         @b FieldIdx_nameOfValues value is automatically added at the end.
///     @li Types of the member fields, prefixed with @b Field_.
///     @li References to the member fields bundled into
///         @b FieldsAsStruct and @b ConstFieldsAsStruct structs.
///     @li Accessor functions prefixed with @b field_
///
///     See @ref sec_field_tutorial_bitfield for more examples and details
/// @param[in] base_ Base class of the defined field which defines @b ValueType
///     internal type to be <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>
///     of field members and provides @b value() member functions to access them.
/// @param[in] ... List of member fields' names.
/// @related comms::field::Bitfield
#define COMMS_FIELD_MEMBERS_ACCESS(...) \
    COMMS_EXPAND(COMMS_DEFINE_FIELD_ENUM(__VA_ARGS__)) \
    auto value() -> decltype(comms::field::toFieldBase(*this).value()) { \
    typedef typename std::decay<decltype(value())>::type AllFieldsTuple; \
    static_assert(std::tuple_size<AllFieldsTuple>::value == FieldIdx_numOfValues, \
        "Invalid number of names for fields tuple"); \
        return comms::field::toFieldBase(*this).value(); \
    } \
    auto value() const -> decltype(comms::field::toFieldBase(*this).value()) { \
        typedef typename std::decay<decltype(value())>::type AllFieldsTuple; \
        static_assert(std::tuple_size<AllFieldsTuple>::value == FieldIdx_numOfValues, \
            "Invalid number of names for fields tuple"); \
        return comms::field::toFieldBase(*this).value(); \
    } \
    COMMS_EXPAND(COMMS_DO_FIELD_ACC_FUNC(value(), __VA_ARGS__))

#ifdef FOR_DOXYGEN_DOC_ONLY
/// @brief Add convenience access enum, structs and functions to the members of
///     bundle fields, such as comms::field::Bundle or comms::field::Bitfield.
/// @details The fields of "bundle" types, such as comms::field::Bundle or
///     comms::field::Bitfield keep their members bundled in
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>
///     and provide access to them via @b value() member functions.
///     The access to the specific member field can be obtained using
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple/get">std::get</a>
///     later on:
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
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
///     the "bundle" field and use the macro inside while providing the type
///     of the base class as first parameter, followed by the names for the
///     member fields:
///     @code
///     class MyField : public comms::field::Bundle<...>
///     {
///         typedef comms::field::Bundle<...> Base;
///     public:
///         COMMS_FIELD_MEMBERS_ACCESS(Base, member1, member2, member3);
///     }
///     @endcode
///     It would be equivalent to having the following types and functions
///     definitions:
///     @code
///     class MyField : public comms::field::Bundle<...>
///     {
///         typedef comms::field::Bundle<...> Base;
///     public:
///         // Access indices for member fields
///         enum FieldIdx {
///             FieldIdx_member1,
///             FieldIdx_member2,
///             FieldIdx_member3,
///             FieldIdx_numOfValues
///         };
///
///         // Types of the member fields
///         typedef Field1 Field_member1;
///         typedef Field2 Field_member2;
///         typedef Field3 Field_member3;
///
///         // Access to member fields bundled into struct
///         struct FieldsAsStruct
///         {
///             Field_member1& member1;
///             Field_member2& member2;
///             Field_member3& member3;
///         };
///
///         // Access to const member fields bundled into struct
///         struct ConstFieldsAsStruct
///         {
///             const Field_member1& member1;
///             const Field_member2& member2;
///             const Field_member3& member3;
///         };
///
///         // Get access to member fields bundled into a struct
///         FieldsAsStruct fieldsAsStruct()
///         {
///             return FieldsAsStruct{
///                 std::get<0>(Base::value()),
///                 std::get<1>(Base::value()),
///                 std::get<2>(Base::value())};
///         }
///
///         // Get access to const member fields bundled into a struct
///         ConstFieldsAsStruct fieldsAsStruct() const
///         {
///             return ConstFieldsAsStruct{
///                 std::get<0>(Base::value()),
///                 std::get<1>(Base::value()),
///                 std::get<2>(Base::value())};
///         }
///
///         // Accessor to "member1" member field.
///         Field_member1& field_meber1()
///         {
///             return std::get<0>(Base::value());
///         }
///
///         // Accessor to const "member1" member field.
///         const Field_member1& field_meber1() const
///         {
///             return std::get<0>(Base::value());
///         }
///
///         // Accessor to "member2" member field.
///         Field_member2& field_meber2()
///         {
///             return std::get<1>(Base::value());
///         }
///
///         // Accessor to const "member2" member field.
///         const Field_member2& field_meber2() const
///         {
///             return std::get<1>(Base::value());
///         }
///
///         // Accessor to "member3" member field.
///         Field_member3& field_meber3()
///         {
///             return std::get<2>(Base::value());
///         }
///
///         // Accessor to const "member3" member field.
///         const Field_member3& field_meber3() const
///         {
///             return std::get<2>(Base::value());
///         }
///     };
///     @endcode
///     @b NOTE, that provided names @b baud, @b parity, and @b flags, have
///         found their way to the following definitions:
///     @li @b FieldIdx enum. The names are prefixed with @b FieldIdx_. The
///         @b FieldIdx_nameOfValues value is automatically added at the end.
///     @li Types of the member fields, prefixed with @b Field_.
///     @li References to the member fields bundled into
///         @b FieldsAsStruct and @b ConstFieldsAsStruct structs.
///     @li Accessor functions prefixed with @b field_
///
///     See @ref sec_field_tutorial_bundle for more examples and details
/// @param[in] base_ Base class of the defined field which defines @b ValueType
///     internal type to be <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>
///     of field members and provides @b value() member functions to access them.
/// @param[in] ... List of member fields' names.
/// @related comms::field::Bundle
#define COMMS_FIELD_MEMBERS_ACCESS(...)
#endif

}  // namespace comms

