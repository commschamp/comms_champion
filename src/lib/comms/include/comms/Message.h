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

#include "util/access.h"

#include "ErrorStatus.h"
#include "Assert.h"
#include "Field.h"

#include "details/MessageInterfaceBuilder.h"

namespace comms
{

/// @brief Main interface class for all the messages.
/// @details Provides basic interface for all the messages.
/// @tparam TOptions Any number of options to define functionality/behaviour
///     of the message. Supported options are:
///     @li comms::option::BigEndian or comms::option::LittleEndian - a "must have"
///         option to specify endianness of the serialisation,
///         without providing one the compilation will fail.
///     @li comms::option::MsgIdType - a "must have" option to specify type of
///         message ID value, without providing one the compilation will fail.
///     @li comms::option::ReadIterator - an option used to specify type of iterator
///         used for reading. If this option is not used, then read()
///         member function doesn't exist.
///     @li comms::option::WriteIterator - an option used to specify type of iterator
///         used for writing. If this option is not used, then write()
///         member function doesn't exist.
template <typename... TOptions>
class Message : public details::MessageInterfaceBuilderT<TOptions...>
{
    typedef details::MessageInterfaceBuilderT<TOptions...> Base;
public:

    /// @brief Type used for message ID.
    typedef typename Base::MsgIdType MsgIdType;

    /// @brief Type used for message ID passed as parameter or returned from function.
    typedef typename Base::MsgIdParamType MsgIdParamType;

    /// @brief Actual Endian defined in provided Traits class.
    typedef typename Base::Endian Endian;

    /// @brief Type of default base class for all the fields.
    typedef comms::Field<comms::option::Endian<Endian> > Field;

    /// @brief All the options bundled into struct.
    typedef details::MessageInterfaceOptionsParser<TOptions...> InterfaceOptions;

    /// @brief Destructor
    virtual ~Message() {};

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Type of the iterator used for reading message contents from
    ///     sequence of bytes stored somewhere.
    /// @details The type exists only if comms::option::ReadIterator option
    ///     was used to specify one.
    typedef TypeProvidedWithOption ReadIterator;

    /// @brief Read message contents using provided iterator.
    /// @details The function exists only if comms::option::ReadIterator option
    ///     was used to specify type of the ReadIterator.
    ///     The contents of the message are updated with bytes being read.
    ///     The buffer is external and maintained by the caller.
    ///     The provided iterator is advanced. The function invokes virtual
    ///     readImpl() function.
    /// @param[in, out] iter Iterator used for reading the data.
    /// @param[in] size Maximum number of bytes that can be read.
    /// @return Status of the operation.
    ErrorStatus read(ReadIterator& iter, std::size_t size);

    /// @brief Type of the iterator used for writing message contents into
    ///     sequence of bytes stored somewhere.
    /// @details The type exists only if comms::option::WriteIterator option
    ///     was used to specify one.
    typedef TypeProvidedWithOption WriteIterator;

    /// @brief Write message contents using provided iterator.
    /// @details The function exists only if comms::option::WriteIterator option
    ///     was used to specify type of the WriteIterator.
    ///     The contents of the message are serialised into buffer. The buffer
    ///     is external and is maintained by the caller.
    ///     The provided iterator is advanced. The function checks whether the
    ///     write is possible by invoking length() function and comparing the
    ///     result to the size of the provided buffer. If there is enough space
    ///     the virtual writeImpl() function is called.
    /// @param[in, out] iter Iterator used for writing the data.
    /// @param[in] size Maximum number of bytes that can be written.
    /// @return Status of the operation.
    ErrorStatus write(WriteIterator& iter, std::size_t size) const;

#endif // #ifdef FOR_DOXYGEN_DOC_ONLY

protected:

    /// @brief Write data into the output area.
    /// @details Use this function to write data to the output area using
    ///     provided iterator. The endianness of the data will be as specified
    ///     in the options of the class.
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

    /// @brief Write partial data into the output area.
    /// @details Use this function to write partial data to the output area using
    ///     provided iterator. The endianness of the data will be as specified
    ///     using in the options of the class.
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

    /// @brief Read data from input area.
    /// @details Use this function to read data from the input area using
    ///     provided iterator.The endianness of the data will be as specified
    ///     in the options of the class.
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

    /// @brief Read partial data from input area.
    /// @details Use this function to read partial data from the input area using
    ///     provided iterator. The endianness of the data will be as specified
    ///     in the options of the class
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

