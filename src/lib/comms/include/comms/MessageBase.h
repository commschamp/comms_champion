//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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
/// Provides common base class for the custom messages with default implementation.

#pragma once

#include "details/MessageImplBuilder.h"
#include "details/fields_access.h"

namespace comms
{

/// @brief Base class for all the custom protocol messages.
/// @details The main purpose of this class is to provide default implementation
///     for some pure virtual functions defined in @ref Message class. Just
///     like with @ref Message class, the provided methods implementation
///     depends on the options passed as TOption template parameter.
/// @tparam TMessage the main interface class of the custom protocol messages.
///     It may be either @ref Message class itself or any other class that
///     extends @ref Message. The @ref MessageBase inherits from class provided
///     as TMessage template parameter. As the result the real inheritance
///     diagram will look like: comms::Message <-- TMessage <-- comms::MessageBase.
/// @tparam TOptions Variadic template parameter that can include zero or more
///     options that specify. The options may be comma separated as well as
///     bundled into std::tuple. Supported options are:
///     @li comms::option::StaticNumIdImpl - In case message have numeric IDs
///         (comms::Message::MsgIdType is of integral or enum type), usage of
///         this option will cause this class to implement getIdImpl() virtual
///         function that returns provided numeric value.
///     @li comms::option::NoIdImpl - Some message may not have valid IDs and
///         their getId() function is never going to be called. Usage of this
///         option will create dummy implementation of getIdImpl() virtual
///         function that contains always failing assertion. In DEBUG mode
///         compilation the application will crash while in release mode the
///         default constructed value of comms::Message::MsgIdType will be returned.
///     @li comms::option::DispatchImpl - If this option is used, the virtual
///         dispatchImpl() function will be defined. Please refer to the
///         function's documentation for details.
///     @li comms::option::FieldsImpl - Usually implementation of read, write,
///         validity check, and length calculation is pretty straight forward. For
///         example the message is considered valid if all the field values
///         are considered to be valid, or read operation is to perform read for
///         all the fields in the message. If the comms::option::FieldsImpl
///         option with all the message field classes bundled into
///         the std::tuple is provided, then @ref MessageBase class can implement
///         readImpl(), writeImpl(), validImpl(), lengthImpl() virtual functions
///         declared as pure in comms::Message interface. The option also
///         provides an accessor functions to the all the field objects: fields().
///     @li comms::option::NoFieldsImpl - This option is an alias to
///         comms::option::FieldsImpl<std::tuple<> >, which provides implementation
///         readImpl(), writeImpl(), validImpl(), lengthImpl() virtual functions
///         when message contains no fields, i.e. readImpl() and writeImple() will
///         always report success doing nothing, validImpl() will always return
///         true, and lengthImpl() will always return 0.
///     @li comms::option::NoDefaultFieldsReadImpl - Quite often the actual
///         message class that inherits from comms::MessageBase implements its
///         own version of readImpl() member function. In this case the default
///         implementation is not needed. Usage of
///         comms::option::NoDefaultFieldsReadImpl option will suppress
///         generation of the default readImpl() member function. It may be
///         used to reduce compilation time and decrease the final binary code
///         size.
///     @li comms::option::NoDefaultFieldsWriteImpl - There can be cases when the actual
///         message class that inherits from comms::MessageBase implements its
///         own version of writeImpl() member function. In this case the default
///         implementation is not needed. Usage of
///         comms::option::NoDefaultFieldsWriteImpl option will suppress
///         generation of the default writeImpl() member function. It may be
///         used to reduce compilation time and decrease the final binary code
///         size.
/// @extends Message
template <typename TMessage, typename... TOptions>
class MessageBase : public details::MessageImplBuilderT<TMessage, TOptions...>
{
    typedef details::MessageImplBuilderT<TMessage, TOptions...> Base;
public:
    /// @brief All the options provided to this class bundled into struct.
    /// @details See @ref page_message_options_impl for reference.
    typedef details::MessageImplOptionsParser<TOptions...> ImplOptions;

#ifdef FOR_DOXYGEN_DOC_ONLY

    /// @brief All field classes provided with comms::option::FieldsImpl option.
    /// @details The type is not defined if comms::option::FieldsImpl option
    ///     wasn't provided to comms::MessageBase.
    typedef FieldsProvidedWithOption AllFields;

    /// @brief Get an access to the fields of the message.
    /// @details The function doesn't exist if comms::option::FieldsImpl option
    ///     wasn't provided to comms::MessageBase.
    /// @return Reference to the fields of the message.
    AllFields& fields();

    /// @brief Get an access to the fields of the message.
    /// @details The function doesn't exist if comms::option::FieldsImpl option
    ///     wasn't provided to comms::MessageBase.
    /// @return Const reference to the fields of the message.
    const AllFields& fields() const;

#endif // #ifdef FOR_DOXYGEN_DOC_ONLY

protected:

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Implementation of ID retrieval functionality.
    /// @details This function exists only if comms::option::StaticNumIdImpl or
    ///     comms::option::NoIdImpl options was provided to comms::MessageBase.
    ///     In case of comms::option::StaticNumIdImpl option the value
    ///     provided with this option casted to comms::Message::MsgIdType type is
    ///     returned.
    /// @return ID of the message.
    virtual MsgIdParamType getIdImpl() const override;

    /// @brief Implementation of dispatch functionality.
    /// @details This function exists only if comms::option::DispatchImpl option
    ///     was provided to comms::MessageBase and if comms::option::Handler option
    ///     option was provided to comms::Message.
    ///     In order to properly implement the dispatch functionality
    ///     this class imposes several requirements. First of all, the custom
    ///     message class must provide its own type as an argument to
    ///     comms::option::DispatchImpl option:
    ///     @code
    ///     class MessageBase :  public comms::Message<...> { ...};
    ///
    ///     class Message1 :
    ///         public comms::MessageBase<
    ///             MessageBase,
    ///             ...
    ///             comms::option::DispatchImpl<Message1>,
    ///             ...
    ///         >
    ///     {
    ///         ...
    ///     };
    ///     @endcode
    ///     Second, The @ref Handler type (inherited from comms::Message) must
    ///     implement "handle()" member function for every message type (specified as
    ///     Message1, Message2, ...) it is supposed to handle:
    ///     @code
    ///     class MyHandler {
    ///     public:
    ///         void handle(Message1& msg);
    ///         void handle(Message2& msg);
    ///         ...
    ///     }
    ///     @endcode
    ///     The "handle()" functions may be virtual. If the handler is capable
    ///     of handling only limited number of messages, there is
    ///     a need to provide additional "handle()" member function to implement
    ///     default handling functionality (usually ignore the message by doing
    ///     nothing) for all other messages that weren't handled explicitly.
    ///     @code
    ///     class MyHandler {
    ///     public:
    ///         ...
    ///         void handle(MessageBase& msg);
    ///     }
    ///     @endcode
    ///     Where "MessageBase" is a common base class for all the possible
    ///     messages.
    ///
    ///     Once the requirements above a properly implemented, the implementation
    ///     of this message is very simple:
    ///     @code
    ///     void dispatchImpl(Handler& handler)
    ///     {
    ///         typedef <actual-message-type-provided-with-option> Actual;
    ///         handler.handle(static_cast<Actual&>(*this));
    ///     }
    ///     @endcode
    ///     The code above forces a compiler to choose appropriate handle()
    ///     function in the Handler class, based on the actual type of the message.
    ///     If such function is not found, the compiler will choose to call
    ///     the one that covers all possible messages @b "void handle(MessageBase& msg)".
    /// @param handler Reference to handler object.
    virtual void dispatchImpl(Handler& handler) override;

    /// @brief Implementation of read functionality.
    /// @details This function exists only if comms::option::FieldsImpl (or
    ///     comms::option::NoFieldsImpl) option WAS provided and
    ///     comms::option::NoDefaultFieldsReadImpl option WASN'T provided
    ///     to comms::MessageBase
    ///     as well as comms::option::ReadIterator option was provided to
    ///     comms::Message class when specifying interface.
    ///     To make this function works, every field class must provide "read"
    ///     function with following signature:
    ///     @code
    ///     template <typename TIter>
    ///     ErrorStatus read(TIter& iter, std::size_t size);
    ///     @endcode
    ///     This function will invoke such "read()" member function for every
    ///     field object listed with comms::option::FieldsImpl option. If
    ///     any field doesn't report ErrorStatus::Success, then read operation
    ///     stops, i.e. the provided iterator is not advanced any more.
    /// @param[in, out] iter Iterator used for reading the data.
    /// @param[in] size Maximum number of bytes that can be read.
    /// @return Status of the operation.
    virtual ErrorStatus readImpl(ReadIterator& iter, std::size_t size) override;

    /// @brief Helper function that allows to read only limited number of fields.
    /// @details Sometimes the default implementation of readImpl() is incorrect.
    ///     For example, some bit in specific field specifies whether other field
    ///     exists or must be skipped. In this case the derived class must
    ///     implement different read functionality. To help in such task this
    ///     function provides an ability to read all the fields up to (not including) requested
    ///     field. The overriding readImpl() function in the custom message
    ///     definition class may use this function for such task.
    ///     This function exists only if comms::option::FieldsImpl or
    ///     comms::option::NoFieldsImpl option was provided to comms::MessageBase
    ///     as well as comms::option::ReadIterator option was provided to
    ///     comms::Message class when specifying interface.
    ///     The requirements from field classes is the same as explained in
    ///     readImpl() documentation.
    /// @tparam TIdx Zero based index of the field to read until. The function
    ///     returns when field with index "TIdx - 1" (if such exists) has been
    ///     read, while field with index "TIdx" still hasn't.
    /// @param[in, out] iter Iterator used for reading the data.
    /// @param[in] size Maximum number of bytes that can be read.
    /// @return Status of the operation.
    /// @pre TIdx <= std::tuple_size<AllFields>::value
    template <std::size_t TIdx>
    ErrorStatus readFieldsUntil(ReadIterator& iter, std::size_t& size);

    /// @brief Helper function that allows to read only limited number of fields.
    /// @details Sometimes the default implementation of readImpl() is incorrect.
    ///     For example, some bit in specific field specifies whether other field
    ///     exists or must be skipped. In this case the derived class must
    ///     implement different read functionality. To help in such task
    ///     readFieldsUntil() function allows to read fields up to a specified one,
    ///     while this function provides an ability to resume reading from some
    ///     other field in the middle. The overriding readImpl() function in the
    ///     custom message definition class may use this function for such task.
    ///     This function exists only if comms::option::FieldsImpl or
    ///     comms::option::NoFieldsImpl option was provided to comms::MessageBase
    ///     as well as comms::option::ReadIterator option was provided to
    ///     comms::Message class when specifying interface.
    ///     The requirements from field classes is the same as explained in
    ///     readImpl() documentation.
    /// @tparam TIdx Zero based index of the field to read from. The function
    ///     reads all the fields between the one indexed TIdx (included) and
    ///     the last one (also included).
    /// @param[in, out] iter Iterator used for reading the data.
    /// @param[in] size Maximum number of bytes that can be read.
    /// @return Status of the operation.
    /// @pre TIdx < std::tuple_size<AllFields>::value
    template <std::size_t TIdx>
    ErrorStatus readFieldsFrom(ReadIterator& iter, std::size_t& size);

    /// @brief Helper function that allows to read only limited number of fields.
    /// @details Sometimes the default implementation of readImpl() is incorrect.
    ///     For example, some bit in specific field specifies whether other fields
    ///     exist or must be skipped. In this case the derived class must
    ///     implement different read functionality. In similar way to
    ///     readFieldsFrom() and readFieldsUntil() this function provides an
    ///     ability to read any number of fields.
    ///     This function exists only if comms::option::FieldsImpl or
    ///     comms::option::NoFieldsImpl option was provided to comms::MessageBase
    ///     as well as comms::option::ReadIterator option was provided to
    ///     comms::Message class when specifying interface.
    ///     The requirements from field classes is the same as explained in
    ///     readImpl() documentation.
    /// @tparam TFromIdx Zero based index of the field to read from.
    /// @tparam TUntilIdx Zero based index of the field to read until (not included).
    /// @param[in, out] iter Iterator used for reading the data.
    /// @param[in] size Maximum number of bytes that can be read.
    /// @return Status of the operation.
    /// @pre TFromIdx < std::tuple_size<AllFields>::value
    /// @pre TUntilIdx <= std::tuple_size<AllFields>::value
    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    ErrorStatus readFieldsFromUntil(ReadIterator& iter, std::size_t& size);

    /// @brief Implementation of write functionality.
    /// @details This function exists only if comms::option::FieldsImpl or
    ///     comms::option::NoFieldsImpl option was provided, and
    ///     comms::option::NoDefaultFieldsWriteImpl option WASN'T provided
    ///     to comms::MessageBase
    ///     as well as comms::option::WriteIterator option was provided to
    ///     comms::Message class when specifying interface.
    ///     To make this function works, every field class must provide "write"
    ///     function with following signature:
    ///     @code
    ///     template <typename TIter>
    ///     ErrorStatus write(TIter& iter, std::size_t size) const;
    ///     @endcode
    ///     This function will invoke such "write()" member function for every
    ///     field object listed with comms::option::FieldsImpl option. If
    ///     any field doesn't report ErrorStatus::Success, then write operation
    ///     stops, i.e. the provided iterator is not advanced any more.
    /// @param[in, out] iter Iterator used for writing the data.
    /// @param[in] size Maximum number of bytes that can be written.
    /// @return Status of the operation.
    virtual ErrorStatus writeImpl(WriteIterator& iter, std::size_t size) const override;

    /// @brief Helper function that allows to writing only limited number of fields.
    /// @details In a similar way to readFieldsUntil(), this function allows
    ///     writing limited number of fields starting from the first one.
    ///     This function exists only if comms::option::FieldsImpl or
    ///     comms::option::NoFieldsImpl option was provided to comms::MessageBase
    ///     as well as comms::option::WriteIterator option was provided to
    ///     comms::Message class when specifying interface.
    ///     The requirements from field classes is the same as explained in
    ///     writeImpl() documentation.
    /// @tparam TIdx Zero based index of the field to write until. The function
    ///     returns when field with index "TIdx - 1" (if such exists) has been
    ///     written, while field with index "TIdx" still hasn't.
    /// @param[in, out] iter Iterator used for writing the data.
    /// @param[in] size Maximum number of bytes that can be written.
    /// @return Status of the operation.
    /// @pre TIdx <= std::tuple_size<AllFields>::value
    template <std::size_t TIdx>
    ErrorStatus writeFieldsUntil(WriteIterator& iter, std::size_t size) const;

    /// @brief Helper function that allows to writing only limited number of fields.
    /// @details In a similar way to readFieldsFrom(), this function allows
    ///     writing limited number of fields starting from the requested one until
    ///     the end.
    ///     This function exists only if comms::option::FieldsImpl or
    ///     comms::option::NoFieldsImpl option was provided to comms::MessageBase
    ///     as well as comms::option::WriteIterator option was provided to
    ///     comms::Message class when specifying interface.
    ///     The requirements from field classes is the same as explained in
    ///     writeImpl() documentation.
    /// @tparam TIdx Zero based index of the field to write from.
    /// @param[in, out] iter Iterator used for writing the data.
    /// @param[in] size Maximum number of bytes that can be written.
    /// @return Status of the operation.
    /// @pre TIdx < std::tuple_size<AllFields>::value
    template <std::size_t TIdx>
    ErrorStatus writeFieldsFrom(WriteIterator& iter, std::size_t size) const;

    /// @brief Helper function that allows to writing only limited number of fields.
    /// @details In a similar way to readFieldsFromUnti(), this function allows
    ///     writing limited number of fields between the requested indices.
    ///     This function exists only if comms::option::FieldsImpl or
    ///     comms::option::NoFieldsImpl option was provided to comms::MessageBase
    ///     as well as comms::option::WriteIterator option was provided to
    ///     comms::Message class when specifying interface.
    ///     The requirements from field classes is the same as explained in
    ///     writeImpl() documentation.
    /// @tparam TFromIdx Zero based index of the field to write from.
    /// @tparam TUntilIdx Zero based index of the field to write until (not including).
    /// @param[in, out] iter Iterator used for writing the data.
    /// @param[in] size Maximum number of bytes that can be written.
    /// @return Status of the operation.
    /// @pre TFromIdx < std::tuple_size<AllFields>::value
    /// @pre TUntilIdx <= std::tuple_size<AllFields>::value
    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    ErrorStatus writeFieldsFromUntil(WriteIterator& iter, std::size_t size) const;

    /// @brief Implementation of validity check functionality.
    /// @details This function exists only if comms::option::FieldsImpl or
    ///     comms::option::NoFieldsImpl option was provided to comms::MessageBase
    ///     as well as comms::option::ValidCheckInterface option was provided to
    ///     comms::Message class when specifying interface.
    ///     To make this function works, every field class must provide "valid()"
    ///     function with following signature:
    ///     @code
    ///     bool valid() const;
    ///     @endcode
    ///     This function will invoke such "valid()" member function for every
    ///     field object listed with comms::option::FieldsImpl option.
    /// @return true when @b all fields are valid.
    virtual bool validImpl() const override;

    /// @brief Implementation of length calculation functionality.
    /// @details This function exists only if comms::option::FieldsImpl or
    ///     comms::option::NoFieldsImpl option was provided to comms::MessageBase
    ///     as well as comms::option::LengthInfoInterface option was provided to
    ///     comms::Message class when specifying interface.
    ///     To make this function works, every field class must provide "length()"
    ///     function with following signature:
    ///     @code
    ///     std::size_t length() const;
    ///     @endcode
    ///     This function will invoke such "length()" member function for every
    ///     field object listed with comms::option::FieldsImpl option. The
    ///     final result is a summary of the "length" values of all the
    ///     fields.
    /// @return Serialisation length of the message.
    virtual std::size_t lengthImpl() const override;
#endif // #ifdef FOR_DOXYGEN_DOC_ONLY
};

/// @brief Add convenience access enum, structs and functions to message fields.
/// @details The comms::MessageBase class provides access to its fields via
///     comms::MessageBase::fields() member function(s). The fields are bundled
///     into <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>
///     and can be accessed using indices with
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple/get">std::get</a>.
///     For convenience, the fields should be named. The COMMS_MSG_FIELDS_ACCESS()
///     macro does exactly that. @n
///     As an example, let's assume that custom message uses 3 fields of any
///     types:
///     @code
///     typedef ... Field1;
///     typedef ... Field2;
///     typedef ... Field3;
///
///     typedef std::tuple<Field1, Field2, Field3> MyMessageFields
///
///     class Message1 : public comms::MessageBase<MyInterface, comms::option::FieldsImpl<MyMessageFields> >
///     {
///         typedef comms::MessageBase<MyInterface, comms::option::FieldsImpl<MyMessageFields> > Base
///     public:
///         COMMS_MSG_FIELDS_ACCESS(Base, name1, name2, name3);
///     };
///     @endcode
///     The usage of the COMMS_MSG_FIELDS_ACCESS() macro with type of the base
///     class (a variant of comms::MessageBase with comms::option::FieldsImpl option)
///     as the first parameter followed by the list of the field's names
///     is equivalent to having the following definitions inside the message class
///     @code
///     class Message1 : public comms::MessageBase<...>
///     {
///     public:
///         enum FieldIdx {
///             FieldIdx_name1,
///             FieldIdx_name2,
///             FieldIdx_name3,
///             FieldIdx_nameOfValues
///         };
///
///         static_assert(std::tuple_size<Base::AllFields>::value == FieldIdx_nameOfValues,
///             "Number of expected fields is incorrect");
///
///         struct FieldsAsStruct
///         {
///             Field1& name1;
///             Field2& name2;
///             Field3& name3;
///         };
///
///         struct ConstFieldsAsStruct
///         {
///             const Field1& name1;
///             const Field2& name2;
///             const Field3& name3;
///         };
///
///         FieldsAsStruct fieldsAsStruct()
///         {
///             return FieldsAsStruct{
///                 std::get<0>(Base::fields()),
///                 std::get<1>(Base::fields()),
///                 std::get<2>(Base::fields())};
///         }
///
///         ConstFieldsAsStruct fieldsAsStruct() const
///         {
///             return ConstFieldsAsStruct{
///                 std::get<0>(Base::fields()),
///                 std::get<1>(Base::fields()),
///                 std::get<2>(Base::fields())};
///         }
///     };
///     @endcode
///     @b NOTE, that provided names @b name1, @b name2, and @b name3 have
///     found their way to the enum @b FieldIdx and as data members of the
///     @b FieldsAsStruct and @b ConstFieldsAsStruct structs. @n
///     Also note, that there is automatically added @b FieldIdx_nameOfValues
///     value to the end of @b FieldIdx enum.
///
///     As the result, the fields can be accessed using @b FieldIdx enum
///     @code
///     void handle(Message1& msg)
///     {
///         auto& allFields = msg.fields();
///         auto& field1 = std::get<MyMessage::FieldIdx_name1>(allFields);
///         auto& field2 = std::get<MyMessage::FieldIdx_name2>(allFields);
///         auto& field3 = std::get<MyMessage::FieldIdx_name1>(allFields);
///
///         auto value1 = field1.value();
///         auto value2 = field2.value();
///         auto value3 = field3.value();
///     }
///     @endcode
///     or using provided struct(s):
///     @code
///     void handle(Message1& msg)
///     {
///         auto allFields = msg.fieldsAsStruct();
///
///         auto value1 = allFields.name1.value();
///         auto value2 = allFields.name2.value();
///         auto value3 = allFields.name3.value();
///     }
///     @endcode
/// @param[in] base_ Base class of the defined message class, expected to be a
///     variant of comms::MessageBase, with usage of  comms::option::FieldsImpl
///     option, i.e. defines and contains internal fields as a tuple.
/// @param[in] ... List of fields' names.
/// @related comms::MessageBase
#define COMMS_MSG_FIELDS_ACCESS(base_, ...) COMMS_FIELDS_ACCESS_ALL(typename base_::AllFields, base_::fields(), __VA_ARGS__)

}  // namespace comms


