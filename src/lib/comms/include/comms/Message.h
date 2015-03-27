//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

/// @file comms/Message.h
/// Contains definition of Message object interface and various base classes
/// for custom messages.

#pragma once

#include <cstdint>
#include <type_traits>

#include "util/Tuple.h"
#include "util/access.h"

#include "ErrorStatus.h"
#include "Assert.h"
#include "traits.h"
#include "Field.h"
#include "EmptyHandler.h"

#include "details/MessageBase.h"

namespace comms
{

/// @addtogroup comms
/// @{

/// @brief Abstract base class for all the messages.
/// @details This class provides basic API to allow retrieval of message ID,
///          reading and writing of the message contents.
/// @tparam TTraits Various behavioural traits relevant for the message. Must
///         define:
///         @li Type MsgIdType. Type used for message identification.
///         @li Type Endian. Must be either comms::traits::endian::Big
///             or comms::traits::endian::Little.
///         @li Type ReadIterator. Can be any type of input iterator. It will
///             be used to read message contents from serialised data sequence.
///         @li Type WriteIterator. Can be any type of output iterator. It will
///             be used to serialise message contents to provided data sequence.
/// @headerfile comms/Message.h
template <typename... TOptions>
class Message : public details::MessageBase<TOptions...>
{
    typedef details::MessageBase<TOptions...> Base;
public:

    /// @brief Type used for message ID
    typedef typename Base::MsgIdType MsgIdType;

    typedef typename Base::AllOptions AllOptions;

    /// Actual Endian defined in provided Traits class
    typedef typename Base::Endian Endian;

    /// @brief Type that must be returned by getId function.
    /// @details In case the message ID is of integral or enum types, the
    ///          return type is "MsgIdType", otherwise it is "const MsgIdType&"
    typedef typename std::conditional<
            std::is_integral<MsgIdType>::value || std::is_enum<MsgIdType>::value,
            MsgIdType,
            const MsgIdType&
        >::type MsgIdParamType;

    /// @brief Type of default base class for all the fields
    typedef comms::Field<comms::option::UseEndian<Endian> > Field;

    /// @brief Destructor
    virtual ~Message() {};

    /// @brief Retrieve ID of the message
    MsgIdParamType getId() const
    {
        return this->getIdImpl();
    }

#ifndef COMMS_NO_READ

    /// @brief Type of read iterator
    /// @details Must be defined in Traits as ReadIterator type.
    typedef typename Base::ReadIterator ReadIterator;

    /// @brief Read body of the message from stream buffer
    /// @details Calls to pure virtual function readImpl() which must
    ///          be implemented in one of the derived classes. Prior to call
    ///          to readImpl(), call to length() is performed to
    ///          verify that buffer contains enough data to create a message.
    ///          If the amount of data in the buffer is not enough
    ///          ErrorStatus::NotEnoughData will be returned.
    ///          It is possible to suppress read functionality by defining
    ///          COMMS_NO_READ. In this case neither read() nor readImpl()
    ///          functions are going to be defined.
    /// @param[in, out] iter Input iterator.
    /// @param[in] size Size of the data in iterated data structure.
    /// @return Status of the read operation.
    /// @pre Input iterator must be valid and can be successfully dereferenced
    ///      and incremented at least size times.
    /// @post The input iterator is advanced.
    /// @post There is no guarantee about state of the message object after
    ///       the read operation is not successful. It may be only partially
    ///       updated.
    ErrorStatus read(ReadIterator& iter, std::size_t size)
    {
        auto minSize = length();
        if (size < minSize) {
            return ErrorStatus::NotEnoughData;
        }

        return this->readImpl(iter, size);
    }
#endif // #ifndef COMMS_NO_READ

#ifndef COMMS_NO_WRITE
    /// @brief Type of write iterator
    /// @details Must be defined in Traits as WriteIterator type.
    typedef typename Base::WriteIterator WriteIterator;

    /// @brief Write body of the message to the stream buffer
    /// @details This function checks whether required buffer size returned
    ///          by length() is less or equal to the "size" parameter
    ///          value. In case there is not enough space this function will
    ///          return ErrorStatus::BufferOverflow. It there is enough space
    ///          to successfully write the message, pure virtual function
    ///          writeImpl() will be called. It must
    ///          be implemented in one of the derived classes.
    ///          It is possible to suppress write functionality by defining
    ///          COMMS_NO_WRITE. In this case neither write() nor writeImpl()
    ///          functions are going to be defined.
    /// @param[in, out] iter Output iterator.
    /// @param[in] size Size of the buffer, message data must fit it.
    /// @return Status of the write operation.
    /// @pre Input iterator must be valid and can be successfully dereferenced
    ///      and incremented at least size times.
    /// @post The output iterator is advanced.
    ErrorStatus write(WriteIterator& iter, std::size_t size) const
    {
        if (size < length()) {
            return ErrorStatus::BufferOverflow;
        }

        return this->writeImpl(iter, size);
    }

#endif // #ifndef COMMS_NO_WRITE

#if !defined(COMMS_NO_READ) || !defined(COMMS_NO_WRITE) || !defined(COMMS_NO_LENGTH)

#if (!defined(COMMS_NO_READ) || !defined(COMMS_NO_WRITE)) && defined(COMMS_NO_LENGTH)
#error "COMMS_NO_LENGTH cannot be defined when either read or write operation is not disabled"
#endif // #if (!defined(COMMS_NO_READ) || !defined(COMMS_NO_WRITE)) && defined(COMMS_NO_LENGTH)

    /// @brief Get size required to serialise a message.
    /// @details This function will call lengthImpl() pure virtual
    ///          function. It is a responsibility of the actual message
    ///          to implement it to provide the information.
    /// @return Number of bytes required to serialise a message.
    /// @post Returned value is less than std::numeric_limits<std::size_t>::max();
    std::size_t length() const
    {
        auto value = this->lengthImpl();
        GASSERT(value < std::numeric_limits<decltype(value)>::max());
        return value;
    }

#endif // #if !defined(COMMS_NO_READ) || !defined(COMMS_NO_WRITE) || !defined(COMMS_NO_LENGTH)

#ifndef COMMS_NO_VALID

    /// @brief Check whether message contents are valid.
    /// @details This function must return an indication whether all internal
    ///          fields are valid. It invokes validImpl() pure virtual function
    ///          that must be implemented by the derived class.
    ///          It is possible to suppress validity check functionality by
    ///          defining COMMS_NO_VALID. In this case neither valid() nor
    ///          validImpl() functions are going to be defined.
    /// @return True in case all fields are valid, false otherwise.
    bool valid() const
    {
        return this->validImpl();
    }

#endif // #ifndef COMMS_NO_VALID

#ifndef COMMS_NO_DISPATCH
    /// @brief Message handler type
    typedef typename Base::Handler Handler;

    /// @brief Dispatch message to its handler
    /// @details Calls to pure virtual function dispatchImpl() which must
    ///          be implemented in one of the derived classes. The message
    ///          should be dispatched to the handler using
    ///          appropriate "handle(...) member function of
    ///          the latter.
    ///          It is possible to suppress dispatch functionality by defining
    ///          COMMS_NO_DISPATCH. In this case neither dispatch() nor
    ///          dispatchImpl() functions are going to be defined.
    /// @param[in] handler Reference to the handler object
    void dispatch(Handler& handler)
    {
        this->dispatchImpl(handler);
    }

#endif // #ifndef COMMS_NO_DISPATCH

protected:

    /// @brief Pure virtual function to retrieve message ID.
    /// @details Must be implemented in the derived class.
    virtual MsgIdParamType getIdImpl() const = 0;

#ifndef COMMS_NO_READ
    /// @brief Pure virtual function to be called to update contents of the
    ///        message based on the data in the provided buffer.
    /// @details Must be implemented in the derived class
    /// @param[in, out] iter Input iterator.
    /// @param[in] size Size of the data in the iterated sequence.
    /// @return Status of the read operation.
    /// @note Must comply with all the preconditions, postconditions,
    ///       Thread safety and Exception guarantee specified in read().
    virtual ErrorStatus readImpl(ReadIterator& iter, std::size_t size) = 0;
#endif // #ifndef COMMS_NO_READ

#ifndef COMMS_NO_WRITE
    /// @brief Pure virtual function to be called to write contents of the
    ///        message to the output buffer.
    /// @details Must be implemented in the derived class
    /// @param[in, out] iter Output stream buffer.
    /// @param[in] size Maximal size the message data must fit it.
    /// @return Status of the write operation.
    /// @pre lengthImpl() <= size. This check is performed in write()
    ///      prior to call to this function.
    /// @note Must comply with all the preconditions, postconditions,
    ///       Thread safety and Exception guarantee specified in write().
    virtual ErrorStatus writeImpl(WriteIterator& iter, std::size_t size) const = 0;
#endif // #ifndef COMMS_NO_WRITE

#if !defined(COMMS_NO_READ) || !defined(COMMS_NO_WRITE) || !defined(COMMS_NO_LENGTH)
    /// @brief Pure virtual function to be called to retrieve
    ///        number of bytes required to serialise current message.
    /// @details Must be implemented in the derived class.
    /// @return Number of bytes required to serialise a message.
    /// @note Must comply with all the preconditions, postconditions,
    ///       Thread safety and Exception guarantee specified in length().
    virtual std::size_t lengthImpl() const = 0;
#endif // #if !defined(COMMS_NO_READ) || !defined(COMMS_NO_WRITE) || !defined(COMMS_NO_LENGTH)

#ifndef COMMS_NO_VALID
    /// @brief Pure virtual function to be called to identify the validity of
    ///        the message.
    /// @details Must be implemented in the derived class
    /// @return True in case the message is valid, false otherwise.
    /// @note Must comply with all the preconditions, postconditions,
    ///       Thread safety and Exception guarantee specified in write().
    virtual bool validImpl() const = 0;
#endif // #ifndef COMMS_NO_VALID

#ifndef COMMS_NO_DISPATCH

    /// @brief Pure virtual function to be called to dispatch message to its
    ///        handler.
    /// @details Must be implemented in the derived class. The implementation
    ///          is basically:
    ///          @code
    ///          handler.handle(*this);
    ///          @endcode
    /// @param[in] handler Reference to the handler object
    virtual void dispatchImpl(Handler& handler) = 0;

#endif // #ifndef COMMS_NO_DISPATCH

    /// @brief Write data into the output sequence.
    /// @details Use this function to write data to the stream buffer.
    ///          The endianness of the data will be as specified in the TTraits
    ///          template parameter of the class.
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

    /// @brief Write partial data into the output sequence.
    /// @details Use this function to write partial data to the stream buffer.
    ///          The endianness of the data will be as specified in the TTraits
    ///          template parameter of the class.
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

    /// @brief Read data from input sequence.
    /// @details Use this function to read data from the stream buffer.
    /// The endianness of the data will be as specified in the TTraits
    /// template parameter of the class.
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

    /// @brief Read partial data from input sequence.
    /// @details Use this function to read data from the stream buffer.
    /// The endianness of the data will be as specified in the TTraits
    /// template parameter of the class.
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

}  // namespace comms

