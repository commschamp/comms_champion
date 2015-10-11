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

/// @addtogroup comms
/// @{

template <typename... TOptions>
class Message : public details::MessageInterfaceBuilderT<TOptions...>
{
    typedef details::MessageInterfaceBuilderT<TOptions...> Base;
public:

    /// @brief Type used for message ID
    typedef typename Base::MsgIdType MsgIdType;

    typedef typename Base::MsgIdParamType MsgIdParamType;

    /// Actual Endian defined in provided Traits class
    typedef typename Base::Endian Endian;

    /// @brief Type of default base class for all the fields
    typedef comms::Field<comms::option::Endian<Endian> > Field;

    typedef details::MessageInterfaceOptionsParser<TOptions...> InterfaceOptions;

    /// @brief Destructor
    virtual ~Message() {};

protected:

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

