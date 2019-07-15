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

/// @file
/// Contains definition of Message object interface and various base classes
/// for custom messages.

#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>

#include "comms/ErrorStatus.h"
#include "comms/Assert.h"
#include "comms/Field.h"

#include "comms/details/MessageInterfaceBuilder.h"
#include "comms/details/transport_fields_access.h"
#include "comms/details/detect.h"
#include "comms/details/MessageIdTypeRetriever.h"

namespace comms
{

/// @brief Main interface class for all the messages.
/// @details Provides basic interface for all the messages.
/// @tparam TOptions Variadic template parameter that contain any number of
///     options to define functionality/behaviour of the message.
///     The options may be comma separated as well as bundled
///     into std::tuple. Supported options are:
///     @li @ref comms::option::def::BigEndian or @ref comms::option::def::LittleEndian - options
///         used to specify endianness of the serialisation. If this option is
///         @ref Field internal types get defined.
///         used, readData() functions as well as @ref Endian and
///     @li @ref comms::option::def::MsgIdType - an option used to specify type of the ID
///         value used to identify the message. If this option is used,
///         the @ref MsgIdType and
///         @ref MsgIdParamType types get defined.
///     @li @ref comms::option::def::ExtraTransportFields - Provide extra fields that
///         are read / written by transport layers, but may influence the way
///         the message being serialized / deserialized and/or handled.
///     @li @ref comms::option::def::VersionInExtraTransportFields - Provide index of
///         the version field in extra transport fields.
///     @li @ref comms::option::app::IdInfoInterface - an option used to provide polymorphic
///         id retrieval functionality. If this option is used in conjunction with
///         comms::option::MsgIdType, the
///         getId() member function is defined.
///     @li @ref comms::option::app::ReadIterator - an option used to specify type of iterator
///         used for reading. If this option is not used, then @ref read()
///         member function doesn't exist.
///     @li @ref comms::option::app::WriteIterator - an option used to specify type of iterator
///         used for writing. If this option is not used, then @ref write()
///         member function doesn't exist.
///     @li @ref comms::option::app::ValidCheckInterface - an option used to add @ref valid()
///         member function to the default interface.
///     @li @ref comms::option::app::LengthInfoInterface - an option used to add @ref length()
///         member function to the default interface.
///     @li @ref comms::option::app::RefreshInterface - an option used to add @ref refresh()
///         member function to the default interface.
///     @li @ref comms::option::app::NameInterface - an option used to add @ref name()
///         member function to the default interface.
///     @li @ref comms::option::app::Handler - an option used to specify type of message handler
///         object used to handle the message when it received. If this option
///         is not used, then dispatch() member function doesn't exist. See
///         dispatch() documentation for details.
///     @li @ref comms::option::app::NoVirtualDestructor - Force the destructor to be
///         non-virtual, even if there are virtual functions in use.
///     @headerfile comms/Message.h
template <typename... TOptions>
class Message : public details::MessageInterfaceBuilderT<TOptions...>
{
    using BaseImpl = details::MessageInterfaceBuilderT<TOptions...>;
public:

    /// @brief All the options bundled into struct.
    using InterfaceOptions = details::MessageInterfaceOptionsParser<TOptions...>;

    /// @brief Destructor.
    /// @details Becomes @b virtual if the message interface is defined to expose
    ///     any polymorphic behavior, i.e. if there is at least one virtual function.
    ///     It is possible to explicitly suppress @b virtual declaration by
    ///     using comms::option::app::NoVirtualDestructor option.
    ~Message() noexcept = default;

    /// @brief Compile type inquiry whether message interface class defines @ref MsgIdType
    ///     and @ref MsgIdParamType types.
    static constexpr bool hasMsgIdType()
    {
        return InterfaceOptions::HasMsgIdType;
    }

    /// @brief Compile type inquiry whether message interface class defines @ref Endian
    ///     and @ref Field types.
    static constexpr bool hasEndian()
    {
        return InterfaceOptions::HasEndian;
    }

    /// @brief Compile type inquiry whether message interface class defines
    ///     @ref getId() and @ref getIdImpl() member functions.
    static constexpr bool hasGetId()
    {
        return hasMsgIdType() && InterfaceOptions::HasMsgIdInfo;
    }

    /// @brief Compile type inquiry whether message interface class defines
    ///     @ref read() and @ref readImpl() member functions as well as @ref
    ///     ReadIterator type.
    static constexpr bool hasRead()
    {
        return InterfaceOptions::HasReadIterator;
    }

    /// @brief Compile type inquiry whether message interface class defines
    ///     @ref write() and @ref writeImpl() member functions as well as @ref
    ///     WriteIterator type.
    static constexpr bool hasWrite()
    {
        return InterfaceOptions::HasWriteIterator;
    }

    /// @brief Compile type inquiry whether message interface class defines
    ///     @ref valid() and @ref validImpl() member functions.
    static constexpr bool hasValid()
    {
        return InterfaceOptions::HasValid;
    }

    /// @brief Compile type inquiry whether message interface class defines
    ///     @ref length() and @ref lengthImpl() member functions.
    static constexpr bool hasLength()
    {
        return InterfaceOptions::HasLength;
    }

    /// @brief Compile type inquiry whether message interface class defines
    ///     @ref refresh() and @ref refreshImpl() member functions.
    static constexpr bool hasRefresh()
    {
        return InterfaceOptions::HasRefresh;
    }

    /// @brief Compile type inquiry whether message interface class defines
    ///     @ref dispatch() and @ref dispatchImpl() member functions as well as @ref
    ///     Handler and @ref DispatchRetType types.
    static constexpr bool hasDispatch()
    {
        return InterfaceOptions::HasHandler;
    }

    /// @brief Compile type inquiry whether message interface class defines
    ///     @ref transportFields() member functions as well as @ref
    ///     TransportFields type.
    static constexpr bool hasTransportFields()
    {
        return InterfaceOptions::HasExtraTransportFields;
    }

    /// @brief Compile type inquiry whether there is version information
    ///     inside transport fields.
    static constexpr bool hasVersionInTransportFields()
    {
        return InterfaceOptions::HasVersionInExtraTransportFields;
    }

    /// @brief Compile type inquiry whether message interface class defines
    ///     @ref name() and @ref nameImpl() member functions.
    static constexpr bool hasName()
    {
        return InterfaceOptions::HasName;
    }

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Type used for message ID.
    /// @details The type exists only if @ref comms::option::def::MsgIdType option
    ///     was provided to comms::Message to specify it.
    /// @see hasMsgIdType()
    using MsgIdType = typename BaseImpl::MsgIdType;

    /// @brief Type used for message ID passed as parameter or returned from function.
    /// @details It is equal to @ref MsgIdType for numeric types and becomes
    ///     "const-reference-to" @ref MsgIdType for more complex types.
    ///      The type exists only if @ref MsgIdType exists, i.e.
    ///      the @ref comms::option::def::MsgIdType option was used.
    using MsgIdParamType = typename BaseImpl::MsgIdParamType;

    /// @brief Serialisation endian type.
    /// @details The type exists only if @ref comms::option::def::BigEndian or
    ///     @ref comms::option::def::LittleEndian options were used to specify it.
    /// @see @ref hasEndian()
    using Endian = typename BaseImpl::Endian;

    /// @brief Type of default base class for all the fields.
    /// @details Requires definition of the @ref Endian type, i.e. the type
    ///     exist only if @ref comms::option::def::BigEndian or
    ///     @ref comms::option::def::LittleEndian options were used.
    using Field = BaseImpl::Field;

    /// @brief Retrieve ID of the message.
    /// @details Invokes pure virtual @ref getIdImpl(). This function exists
    ///     only if @ref comms::option::def::MsgIdType option was used to specify type
    ///     of the ID value and @ref comms::option::app::IdInfoInterface option are used.
    /// @return ID of the message.
    /// @see @ref hasGetId();
    MsgIdParamType getId() const;

    /// @brief Type of the iterator used for reading message contents from
    ///     sequence of bytes stored somewhere.
    /// @details The type exists only if @ref comms::option::app::ReadIterator option
    ///     was provided to comms::Message to specify one.
    /// @see @ref hasRead()
    using ReadIterator = TypeProvidedWithOption;

    /// @brief Read message contents using provided iterator.
    /// @details The function exists only if @ref comms::option::app::ReadIterator option
    ///     was provided to comms::Message to specify type of the @ref ReadIterator.
    ///     The contents of the message are updated with bytes being read.
    ///     The buffer is external and maintained by the caller.
    ///     The provided iterator is advanced. The function invokes virtual
    ///     readImpl() function.
    /// @param[in, out] iter Iterator used for reading the data.
    /// @param[in] size Maximum number of bytes that can be read.
    /// @return Status of the operation.
    /// @see @ref hasRead()
    ErrorStatus read(ReadIterator& iter, std::size_t size);

    /// @brief Type of the iterator used for writing message contents into
    ///     sequence of bytes stored somewhere.
    /// @details The type exists only if @ref comms::option::app::WriteIterator option
    ///     was provided to comms::Message to specify one.
    /// @see @ref hasWrite()
    using WriteIterator = TypeProvidedWithOption;

    /// @brief Write message contents using provided iterator.
    /// @details The function exists only if @ref comms::option::app::WriteIterator option
    ///     was provided to comms::Message to specify type of the @ref WriteIterator.
    ///     The contents of the message are serialised into buffer. The buffer
    ///     is external and is maintained by the caller.
    ///     The provided iterator is advanced. The function invokes virtual
    ///     writeImpl() function.
    /// @param[in, out] iter Iterator used for writing the data.
    /// @param[in] size Maximum number of bytes that can be written.
    /// @return Status of the operation.
    /// @see @ref hasWrite()
    ErrorStatus write(WriteIterator& iter, std::size_t size) const;

    /// @brief Check validity of message contents.
    /// @details The function exists only if @ref comms::option::app::ValidCheckInterface option
    ///     was provided to comms::Message. The function invokes virtual validImpl() function.
    /// @return true for valid contents, false otherwise.
    /// @see @ref hasValid()
    bool valid() const;

    /// @brief Get number of bytes required to serialise this message.
    /// @details The function exists only if @ref comms::option::app::LengthInfoInterface option
    ///     was provided to comms::Message. The function invokes virtual lengthImpl() function.
    /// @return Number of bytes required to serialise this message.
    /// @see @ref hasLength()
    std::size_t length() const;

    /// @brief Refresh to contents of the message.
    /// @details Many protocols define their messages in a way that the content
    ///     of some fields may depend on the value of the other field(s). For
    ///     example, providing in one field the information about number of
    ///     elements in the list that will follow later. Another example is
    ///     having bits in a bitmask field specifying whether other optional
    ///     fields exist. In this case, directly modifying value of some
    ///     fields may leave a message contents in an inconsistent state.
    ///     Having refresh() member function allows the developer to bring
    ///     the message into a consistent state prior to sending it over
    ///     I/O link . @n
    ///     The function exists only if @ref comms::option::app::RefreshInterface option
    ///     was provided to comms::Message. The function invokes virtual
    ///     refreshImpl() function.
    /// @return true in case the contents of the message were modified, false if
    ///     all the fields of the message remained unchanged.
    bool refresh();

    /// @brief Get name of the message.
    /// @details The function exists only if @ref comms::option::app::NameInterface option
    ///     was provided to comms::Message. The function invokes virtual
    ///     @ref nameImpl() function.
    /// @see @ref hasName()
    const char* name() const;

    /// @brief Type of the message handler object.
    /// @details The type exists only if @ref comms::option::app::Handler option
    ///     was provided to comms::Message to specify one.
    using Handler = TypeProvidedWithOption;

    /// @brief Return type of the @ref dispatch() member function.
    /// @details Equal to @b Handler::RetType if such exists, @b void otherwise.
    using DispatchRetType = typename Handler::RetType;

    /// @brief Dispatch message to the handler for processing.
    /// @details The function exists only if @ref comms::option::app::Handler option
    ///     was provided to comms::Message to specify type of the handler.
    ///     The function invokes virtual dispatchImpl() function.
    /// @param handler Handler object to dispatch message to.
    DispatchRetType dispatch(Handler& handler);

    /// @brief @b std::tuple of extra fields from transport layers that
    ///     may affect the way the message fields get serialized / deserialized.
    /// @details The type exists only if @ref comms::option::def::ExtraTransportFields
    ///     option has been provided to @ref comms::Message class to specify them.
    /// @see @ref hasTransportFields()
    using TransportFields = FieldsProvidedWithOption;

    /// @brief Get access to extra transport fields.
    /// @details The function exists only if @ref comms::option::def::ExtraTransportFields
    ///     option has been provided to @ref comms::Message class to specify them.
    ////    Some protocols may use additional values in transport information, such
    ///     as message version for example. Such values may influence the way
    ///     message data is being serialized / deserialized.
    ///     The provided extra transport fields are @b NOT serialised as part
    ///     of message payload. Their values are expected to be set by transport layer(s)
    ///     when such information is read. The transport layers are also responsible to
    ///     take the updated information from the relevant field and write it
    ///     when message contents being written.
    /// @see @ref hasTransportFields()
    TransportFields& transportFields();

    /// @brief Const version of @ref transportFields
    /// @details The function exists only if @ref comms::option::def::ExtraTransportFields
    ///     option has been provided to @ref comms::Message class to specify them.
    /// @see @ref hasTransportFields()
    const TransportFields& transportFields() const;

    /// @brief Type used for version info
    /// @details The type exists only if @ref comms::option::def::VersionInExtraTransportFields
    ///     option has been provided.
    using VersionType = typename BaseImpl::VersionType;

    /// @brief Access to version information
    /// @details The function exists only if @ref comms::option::def::VersionInExtraTransportFields
    ///     option has been provided.
    VersionType& version();

    /// @brief Const access to version information
    /// @details The function exists only if @ref comms::option::def::VersionInExtraTransportFields
    ///     option has been provided.
    const VersionType& version() const;
#endif // #ifdef FOR_DOXYGEN_DOC_ONLY

protected:

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Pure virtual function used to retrieve ID of the message.
    /// @details Called by getId(), must be implemented in the derived class.
    ///     This function exists
    ///     only if @ref comms::option::def::MsgIdType option was used to specify type
    ///     of the ID value as well as @ref comms::option::app::IdInfoInterface.
    /// @return ID of the message.
    /// @see @ref hasGetId();
    virtual MsgIdParamType getIdImpl() const = 0;

    /// @brief Virtual function used to implement read operation.
    /// @details Called by read(), expected be implemented in the derived class.
    ///     The function exists only if @ref comms::option::app::ReadIterator option
    ///     was provided to comms::Message to specify type of the @ref ReadIterator.
    /// @param[in, out] iter Iterator used for reading the data.
    /// @param[in] size Maximum number of bytes that can be read.
    /// @return Status of the operation. If not overridden returns
    ///     comms::ErrorStatus::NotSupported.
    /// @see @ref hasRead()
    virtual comms::ErrorStatus readImpl(ReadIterator& iter, std::size_t size);

    /// @brief Virtual function used to implement write operation.
    /// @details Called by write(), expected be implemented in the derived class.
    ///     The function exists only if @ref comms::option::app::WriteIterator option
    ///     was provided to comms::Message to specify type of the @ref WriteIterator.
    /// @param[in, out] iter Iterator used for writing the data.
    /// @param[in] size Maximum number of bytes that can be written.
    /// @return Status of the operation. If not overridden returns
    ///     comms::ErrorStatus::NotSupported.
    /// @see @ref hasWrite()
    virtual comms::ErrorStatus writeImpl(WriteIterator& iter, std::size_t size) const;

    /// @brief Pure virtual function used to implement contents validity check.
    /// @details Called by valid(), must be implemented in the derived class.
    ///     The function exists only if @ref comms::option::app::ValidCheckInterface option
    ///     was provided to comms::Message.
    /// @return true for valid contents, false otherwise.
    /// @see @ref hasValid()
    virtual bool validImpl() const = 0;

    /// @brief Pure virtual function used to retrieve number of bytes required
    ///     to serialise this message.
    /// @details Called by length(), must be implemented in the derived class.
    ///     The function exists only if @ref comms::option::app::LengthInfoInterface option
    ///     was provided to comms::Message.
    /// @return Number of bytes required to serialise this message.
    /// @see @ref hasLength()
    virtual std::size_t lengthImpl() const = 0;

    /// @brief Virtual function used to bring contents of the message
    ///     into a consistent state.
    /// @details Called by refresh(), can be overridden in the derived class.
    ///     If not overridden, does nothing and returns false indicating that
    ///     contents of the message haven't been changed.
    ///     The function exists only if @ref comms::option::app::RefreshInterface option
    ///     was provided to comms::Message.
    /// @return true in case the contents of the message were modified, false if
    ///     all the fields of the message remained unchanged.
    virtual bool refreshImpl();

    /// @brief Pure virtual function used to dispatch message to the handler
    ///     object for processing.
    /// @details Called by dispatch(), must be implemented in the derived class.
    ///     The function exists only if @ref comms::option::app::Handler option was
    ///     provided to comms::Message to specify type of the handler.
    /// @param handler Handler object to dispatch message to.
    virtual DispatchRetType dispatchImpl(Handler& handler) = 0;

    /// @brief Pure virtual function used to retrieve actual message name.
    /// @details Called by @ref name(), must be implemented in the derived class.
    ///     The function exists only if @ref comms::option::app::NameInterface option was
    ///     provided to @ref comms::Message.
    virtual const char* nameImpl() const = 0;

    /// @brief Write data into the output area.
    /// @details Use this function to write data to the output area using
    ///     provided iterator. This function requires knowledge about serialisation
    ///     endian. It exists only if endian type was
    ///     specified using @ref comms::option::def::BigEndian or @ref comms::option::def::LittleEndian
    ///     options to the class.
    /// @tparam T Type of the value to write. Must be integral.
    /// @tparam Type of output iterator
    /// @param[in] value Integral type value to be written.
    /// @param[in, out] iter Output iterator.
    /// @pre The iterator must be valid and can be successfully dereferenced
    ///      and incremented at least sizeof(T) times.
    /// @post The iterator is advanced.
    /// @note Thread safety: Safe for distinct buffers, unsafe otherwise.
    template <typename T, typename TIter>
    static void writeData(T value, TIter& iter);

    /// @brief Write partial data into the output area.
    /// @details Use this function to write partial data to the output area using
    ///     provided iterator. This function requires knowledge about serialisation
    ///     endian. It exists only if endian type was
    ///     specified using @ref comms::option::def::BigEndian or @ref comms::option::def::LittleEndian
    ///     options to the class.
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
    static void writeData(T value, TIter& iter);

    /// @brief Read data from input area.
    /// @details Use this function to read data from the input area using
    ///     provided iterator. This function requires knowledge about serialisation
    ///     endian. It exists only if endian type was
    ///     specified using @ref comms::option::def::BigEndian or @ref comms::option::def::LittleEndian
    ///     options to the class.
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
    static T readData(TIter& iter);

    /// @brief Read partial data from input area.
    /// @details Use this function to read partial data from the input area using
    ///     provided iterator. This function requires knowledge about serialisation
    ///     endian. It exists only if endian type was
    ///     specified using @ref comms::option::def::BigEndian or @ref comms::option::def::LittleEndian
    ///     options to the class.
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
    static T readData(TIter& iter);

#endif // #ifdef FOR_DOXYGEN_DOC_ONLY
};

/// @brief Upcast type of the message object to comms::Message in order to have
///     access to its internal types.
template <typename... TOptions>
inline
Message<TOptions...>& toMessage(Message<TOptions...>& msg)
{
    return msg;
}

/// @brief Upcast type of the message object to comms::Message in order to have
///     access to its internal types.
template <typename... TOptions>
inline
const Message<TOptions...>& toMessage(const Message<TOptions...>& msg)
{
    return msg;
}

/// @brief Compile time check of of whether the type
///     is a message.
/// @details Checks existence of @b InterfaceOptions inner
///     type.
template <typename T>
constexpr bool isMessage()
{
    return details::hasInterfaceOptions<T>();
}

/// @brief Get type of message ID used by interface class
/// @details In case common interface class defines its message
///     ID type (using @ref comms::option::MsgIdType option) the
///     latter is returned, otherwise the default type (@b TDefaultType)
///     is reported.
/// @tparam TMsg Message interface class (extended or typedef-ed @ref comms::Message)
/// @tparam TDefaultType Default type to return in case message
///     interface class doesn't define its ID type
template <typename TMsg, typename TDefaultType = std::intmax_t>
using MessageIdType =
    typename details::MessageIdTypeRetriever<TMsg::hasMsgIdType()>::
        template Type<typename TMsg::InterfaceOptions, TDefaultType>;

}  // namespace comms

/// @brief Add convenience access enum and functions to extra transport fields.
/// @details The comms::Message class provides access to its extra transport fields via
///     comms::MessageBase::transportFields() member function(s). The fields are bundled
///     into <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>
///     and can be accessed using indices with
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple/get">std::get</a>.
///     For convenience, the fields should be named. The COMMS_MSG_TRANSPORT_FIELDS_ACCESS()
///     macro does exactly that. @n
///     As an example, let's assume that custom message uses 3 fields of any
///     types:
///     @code
///     typedef ... TransportField1;
///     typedef ... TransportField2;
///     typedef ... TransportField3;
///
///     typedef std::tuple<TransportField1, TransportField2, TransportField3> MyExtraTransportFields
///
///     class MyInterface : public
///         comms::Message<
///             ...
///             comms::option::def::ExtraTransportFields<MyExtraTransportFields> >
///     {
///     public:
///         COMMS_MSG_TRANSPORT_FIELDS_ACCESS(name1, name2, name3);
///     };
///     @endcode
///     The usage of the COMMS_MSG_TRANSPORT_FIELDS_ACCESS() macro with the list of the extra transport field's names
///     is equivalent to having the following definitions inside the message class
///     @code
///     class MyInterface : public comms::Message<...>
///     {
///         using Base = comms::Message<...>;
///     public:
///         enum TransportFieldIdx {
///             TransportFieldIdx_name1,
///             TransportFieldIdx_name2,
///             TransportFieldIdx_name3,
///             TransportFieldIdx_nameOfValues
///         };
///
///         static_assert(std::tuple_size<Base::TransportFields>::value == TransportFieldIdx_nameOfValues,
///             "Number of expected transport fields is incorrect");
///
///         // Accessor to "name1" transport field.
///         auto transportField_name1() -> decltype(std::get<FieldIdx_name1>(Base::transportFields()))
///         {
///             return std::get<FieldIdx_name1>(Base::transportFields());
///         }
///
///         // Accessor to "name1" field.
///         auto transportField_name1() const -> decltype(std::get<FieldIdx_name1>(Base::transportFields()))
///         {
///             return std::get<FieldIdx_name1>(Base::transportFields());
///         }
///
///         // Accessor to "name2" field.
///         auto transportField_name2() -> decltype(std::get<FieldIdx_name2>(Base::transportFields()))
///         {
///             return std::get<FieldIdx_name2>(Base::transportFields());
///         }
///
///         // Accessor to "name2" field.
///         auto transportField_name2() const -> decltype(std::get<FieldIdx_name2>(Base::transportFields()))
///         {
///             return std::get<FieldIdx_name2>(Base::transportFields());
///         }
///
///         // Accessor to "name3" field.
///         auto transportField_name3() -> decltype(std::get<FieldIdx_name3>(Base::transportFields()))
///         {
///             return std::get<FieldIdx_name3>(Base::transportFields());
///         }
///
///         // Accessor to "name3" field.
///         auto transportField_name3() const -> decltype(std::get<FieldIdx_name3>(Base::transportFields()))
///         {
///             return std::get<FieldIdx_name3>(Base::transportFields());
///         }
///     };
///     @endcode
///     @b NOTE, that provided names @b name1, @b name2, and @b name3 have
///     found their way to the following definitions:
///     @li @b TransportFieldIdx enum. The names are prefixed with @b TransportFieldIdx_. The
///         @b TransportFieldIdx_nameOfValues value is automatically added at the end.
///     @li Accessor functions prefixed with @b transportField_
///
///     As the result, the fields can be accessed using @b TransportFieldIdx enum
///     @code
///     void handle(Message1& msg)
///     {
///         auto& transportFields = msg.transportFields();
///         auto& field1 = std::get<Message1::TransportFieldIdx_name1>(transportFields);
///         auto& field2 = std::get<Message1::TransportFieldIdx_name2>(transportFields);
///         auto& field3 = std::get<Message1::TransportFieldIdx_name3>(transportFields);
///
///         auto value1 = field1.value();
///         auto value2 = field2.value();
///         auto value3 = field3.value();
///     }
///     @endcode
///     or using accessor functions:
///     @code
///     void handle(Message1& msg)
///     {
///         auto value1 = transportField_name1().value();
///         auto value2 = transportField_name2().value();
///         auto value3 = transportField_name3().value();
///     }
///     @endcode
/// @param[in] ... List of fields' names.
/// @related comms::Message
#define COMMS_MSG_TRANSPORT_FIELDS_ACCESS(...) \
    COMMS_EXPAND(COMMS_DEFINE_TRANSPORT_FIELD_ENUM(__VA_ARGS__)) \
    COMMS_MSG_TRANSPORT_FIELDS_ACCESS_FUNC { \
        auto& msgBase = comms::toMessage(*this); \
        using MsgBase = typename std::decay<decltype(msgBase)>::type; \
        static_assert(MsgBase::hasTransportFields(), \
            "Message interface class doesn't define extra transport fields."); \
        using TransportFieldsTuple = typename MsgBase::TransportFields; \
        static_assert(std::tuple_size<TransportFieldsTuple>::value == TransportFieldIdx_numOfValues, \
            "Invalid number of names for transport fields tuple"); \
        return msgBase.transportFields(); \
    } \
    COMMS_MSG_TRANSPORT_FIELDS_ACCESS_CONST_FUNC { \
        return comms::toMessage(*this).transportFields(); \
    } \
    COMMS_EXPAND(COMMS_DO_TRANSPORT_FIELD_ACC_FUNC(TransportFields, transportFields(), __VA_ARGS__))

