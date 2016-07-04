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


#pragma once

#include <tuple>
#include <iterator>
#include "comms/Assert.h"
#include "comms/util/Tuple.h"
#include "comms/field/ArrayList.h"
#include "comms/field/IntValue.h"

namespace comms
{

namespace protocol
{

/// @brief Message data layer.
/// @details Must always be the last layer in protocol stack.
/// @tparam TMessage Common message interface class, must be the common base
///     to all the custom messages.
/// @tparam TField Field that can be used to store raw data of the message.
template <
    typename TMessage,
    typename TField =
        comms::field::ArrayList<
            typename TMessage::Field,
            std::uint8_t
        >
>
class MsgDataLayer
{
public:

    /// @brief Raw data field type.
    typedef TField Field;

    /// @brief All fields of the remaining transport layers, contains only @ref Field.
    typedef std::tuple<Field> AllFields;

    /// @brief Type of pointer to the message.
    /// @details At this point is unknown, will be redefined in
    ///     comms::protocol::MsgIdLayer.
    typedef void MsgPtr;

    /// @brief Type of the custom message interface.
    typedef TMessage Message;

    /// @brief Type of iterator used for reading.
    /// @details Same as comms::Message::ReadIterator if such exists, void otherwise.
    typedef typename std::conditional<
            Message::InterfaceOptions::HasReadIterator,
            typename TMessage::ReadIterator,
            void
        >::type ReadIterator;

    /// @brief Type of iterator used for writing.
    /// @details Same as comms::Message::WriteIterator if such exists, void otherwise.
    typedef typename std::conditional<
            Message::InterfaceOptions::HasWriteIterator,
            typename TMessage::WriteIterator,
            void
        >::type WriteIterator;

    /// @brief Static constant indicating amount of transport layers used.
    static const std::size_t NumOfLayers = 1;

    /// @brief Default constructor
    MsgDataLayer() = default;

    /// @brief Copy constructor
    MsgDataLayer(const MsgDataLayer&) = default;

    /// @brief Move constructor
    MsgDataLayer(MsgDataLayer&&) = default;

    /// @brief Destructor
    ~MsgDataLayer() = default;

    /// @brief Copy assignment operator
    MsgDataLayer& operator=(const MsgDataLayer&) = default;

    /// @brief Move assignment operator
    MsgDataLayer& operator=(MsgDataLayer&&) = default;

    /// @brief Read the message contents.
    /// @details Calls the read() member function of the message object.
    /// @tparam TMsgPtr Type of the smart pointer to the allocated message object.
    /// @param[in] msgPtr Reference to the smart pointer holding message object,
    /// @param[in, out] iter Iterator used for reading.
    /// @param[in] size Number of bytes available for reading.
    /// @param[out] missingSize In case there are not enough bytes in the buffer
    ///     (the function returns comms::ErrorStatus::NotEnoughData), and this
    ///     pointer is not nullptr, then it is used to provide information of
    ///     minimal number of bytes that need to be provided before message could
    ///     be successfully read.
    /// @return Status of the read operation.
    /// @pre msgPtr points to a valid message object.
    /// @post missingSize output value is updated if and only if function
    ///       returns comms::ErrorStatus::NotEnoughData.
    template <typename TMsgPtr>
    static ErrorStatus read(
        TMsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        static_assert(Message::InterfaceOptions::HasLength,
            "Message interface class is expected to provide length() interface function.");

        GASSERT(msgPtr);
        auto result = msgPtr->read(iter, size);
        if ((result == ErrorStatus::NotEnoughData) &&
            (missingSize != nullptr)) {
            if (size < msgPtr->length()) {
                *missingSize = msgPtr->length() - size;
            }
            else {
                *missingSize = 1;
            }
        }
        return result;
    }

    /// @brief Read the message contents while caching the read transport
    ///     information fields.
    /// @details Very similar to read() member function, but adds "allFields"
    ///     parameter to store raw data of the message.
    /// @tparam TIdx Index of the data field in TAllFields, expected to be last
    ///     element in the tuple.
    /// @tparam TAllFields std::tuple of all the transport fields, must be
    ///     @ref AllFields type defined in the last layer class that defines
    ///     protocol stack.
    /// @tparam TMsgPtr Type of the smart pointer to the allocated message object.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in] msgPtr Reference to the smart pointer holding message object,
    /// @param[in, out] iter Iterator used for reading.
    /// @param[in] size Number of bytes available for reading.
    /// @param[out] missingSize In case there are not enough bytes in the buffer
    ///     (the function returns comms::ErrorStatus::NotEnoughData), and this
    ///     pointer is not nullptr, then it is used to provide information of
    ///     minimal number of bytes that need to be provided before message could
    ///     be successfully read.
    template <std::size_t TIdx, typename TAllFields, typename TMsgPtr>
    static ErrorStatus readFieldsCached(
        TAllFields& allFields,
        TMsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        static_assert(comms::util::IsTuple<TAllFields>::Value,
                                        "Expected TAllFields to be tuple.");

        static_assert((TIdx + 1) == std::tuple_size<TAllFields>::value,
                    "All fields must be read when MsgDataLayer is reached");

        typedef typename std::iterator_traits<ReadIterator>::iterator_category IterType;
        auto& dataField = std::get<TIdx>(allFields);

        typedef typename std::decay<decltype(dataField)>::type FieldType;
        static_assert(
            std::is_same<Field, FieldType>::value,
            "Field has wrong type");
        return readWithFieldCachedInternal(dataField, msgPtr, iter, size, missingSize, IterType());
    }

    /// @brief Write the message contents.
    /// @details Calls the write() member function of the message object.
    /// @tparam TMessage Type of the message.
    /// @param[in] msg Reference to the message object,
    /// @param[in, out] iter Iterator used for writing.
    /// @param[in] size Max number of bytes that can be written.
    /// @return Status of the write operation.
    static ErrorStatus write(
        const TMessage& msg,
        WriteIterator& iter,
        std::size_t size)
    {
        return msg.write(iter, size);
    }

    /// @brief Write the message contents while caching the written transport
    ///     information fields.
    /// @details Very similar to write() member function, but adds "allFields"
    ///     parameter to store raw data of the message.
    /// @tparam TIdx Index of the data field in TAllFields, expected to be last
    ///     element in the tuple.
    /// @tparam TAllFields std::tuple of all the transport fields, must be
    ///     @ref AllFields type defined in the last layer class that defines
    ///     protocol stack.
    /// @tparam TMessage Type of the message.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in] msg Reference to the message object that is being written,
    /// @param[in, out] iter Iterator used for writing.
    /// @param[in] size Max number of bytes that can be written.
    /// @return Status of the write operation.
    template <std::size_t TIdx, typename TAllFields>
    static ErrorStatus writeFieldsCached(
        TAllFields& allFields,
        const TMessage& msg,
        WriteIterator& iter,
        std::size_t size)
    {
        static_assert(comms::util::IsTuple<TAllFields>::Value,
                                        "Expected TAllFields to be tuple.");

        static_assert((TIdx + 1) == std::tuple_size<TAllFields>::value,
                    "All fields must be written when MsgDataLayer is reached");

        typedef typename std::iterator_traits<WriteIterator>::iterator_category IterType;
        auto& dataField = std::get<TIdx>(allFields);

        typedef typename std::decay<decltype(dataField)>::type FieldType;
        static_assert(
            std::is_same<Field, FieldType>::value,
            "Field has wrong type");
        return writeWithFieldCachedInternal(dataField, msg, iter, size, IterType());
    }

    /// @brief Update recently written (using write()) message contents data.
    /// @details Sometimes, when NON random access iterator is used for writing
    ///     (for example std::back_insert_iterator), some transport data cannot
    ///     be properly written. In this case, write() function will return
    ///     comms::ErrorStatus::UpdateRequired. When such status is returned
    ///     it is necessary to call update() with random access iterator on
    ///     the written buffer to update written dummy information with
    ///     proper values.
    ///     This function in this layer does nothing, just advances the iterator
    ///     by "size".
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Number of bytes that have been written using write().
    /// @return Status of the update operation.
    template <typename TIter>
    static comms::ErrorStatus update(TIter& iter, std::size_t size)
    {
        std::advance(iter, size);
        return comms::ErrorStatus::Success;
    }

    /// @brief Update recently written (using writeFieldsCached()) message data as
    ///     well as cached transport information fields.
    /// @details Very similar to update() member function, but adds "allFields"
    ///     parameter to store raw data of the message.
    /// @tparam TIdx Index of the data field in TAllFields, expected to be last
    ///     element in the tuple.
    /// @tparam TAllFields std::tuple of all the transport fields, must be
    ///     @ref AllFields type defined in the last layer class that defines
    ///     protocol stack.
    /// @tparam TUpdateIter Type of the random access iterator.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in, out] iter Random access iterator to the written data.
    /// @param[in] size Number of bytes that have been written using writeFieldsCached().
    /// @return Status of the update operation.
    template <std::size_t TIdx, typename TAllFields, typename TUpdateIter>
    static ErrorStatus updateFieldsCached(
        TAllFields& allFields,
        TUpdateIter& iter,
        std::size_t size)
    {
        static_assert(comms::util::IsTuple<TAllFields>::Value,
                                        "Expected TAllFields to be tuple.");

        static_assert((TIdx + 1) == std::tuple_size<TAllFields>::value,
                    "All fields must be written when MsgDataLayer is reached");

        static_cast<void>(allFields);
        std::advance(iter, size);
        return comms::ErrorStatus::Success;
    }

    /// @brief Get remaining length of wrapping transport information.
    /// @details The message data always get wrapped with transport information
    ///     to be successfully delivered to and unpacked on the other side.
    ///     This function return remaining length of the transport information.
    /// @return 0.
    static constexpr std::size_t length()
    {
        return 0U;
    }

    /// @brief Get remaining length of wrapping transport information + length
    ///     of the provided message.
    /// @details This function usually gets called when there is a need to
    ///     identify the size of the buffer required to write provided message
    ///     wrapped in the transport information. This function is very similar
    ///     to length(), but adds also length of the message.
    /// @param[in] msg Message
    /// @return Length of the message.
    static constexpr std::size_t length(const TMessage& msg)
    {
        return msg.length();
    }

private:
    template <typename TMsgPtr>
    static ErrorStatus readWithFieldCachedInternal(
        Field& field,
        TMsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize,
        std::random_access_iterator_tag)
    {
        return readWithFieldCachedRandomAccess(field, msgPtr, iter, size, missingSize);
    }

    template <typename TMsgPtr>
    static ErrorStatus readWithFieldCachedInternal(
        Field& field,
        TMsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize,
        std::input_iterator_tag)
    {
        static_cast<void>(field);
        GASSERT(!"Not supported yet");
        return read(msgPtr, iter, size, missingSize);
    }

    template <typename TMsgPtr>
    static ErrorStatus readWithFieldCachedRandomAccess(
        Field& field,
        TMsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize)
    {
        ReadIterator dataIter = iter;
        auto es = read(msgPtr, iter, size, missingSize);
        auto dataSize = static_cast<std::size_t>(std::distance(dataIter, iter));
        auto dataEs = field.read(dataIter, dataSize);
        static_cast<void>(dataEs);
        GASSERT(dataEs == comms::ErrorStatus::Success);
        GASSERT((es != ErrorStatus::Success) || (!msgPtr) || (dataSize == msgPtr->length()));
        return es;
    }

    static ErrorStatus writeWithFieldCachedInternal(
        Field& field,
        const TMessage& msg,
        WriteIterator& iter,
        std::size_t size,
        std::random_access_iterator_tag)
    {
        return writeWithFieldCachedRandomAccess(field, msg, iter, size);
    }

    static ErrorStatus writeWithFieldCachedInternal(
        Field& field,
        const TMessage& msg,
        WriteIterator& iter,
        std::size_t size,
        std::output_iterator_tag)
    {
        return writeWithFieldCachedOutput(field, msg, iter, size);
    }

    static ErrorStatus writeWithFieldCachedRandomAccess(
        Field& field,
        const TMessage& msg,
        WriteIterator& iter,
        std::size_t size)
    {
        auto dataReadIter = iter;
        auto es = write(msg, iter, size);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto dataEs = field.read(dataReadIter, msg.length());
        GASSERT(dataEs == comms::ErrorStatus::Success);
        static_cast<void>(dataEs);
        return comms::ErrorStatus::Success;
    }

    template <typename TCollection>
    static ErrorStatus writeWithFieldCachedOutput(
        Field& field,
        const TMessage& msg,
        std::back_insert_iterator<TCollection>& iter,
        std::size_t size)
    {
        auto es = write(msg, iter, size);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        TCollection col;
        auto dataWriteIter = std::back_inserter(col);
        auto dataWriteEs = write(msg, dataWriteIter, size);
        GASSERT(dataWriteEs == comms::ErrorStatus::Success);
        static_cast<void>(dataWriteEs);

        auto dataReadIter = col.cbegin();
        auto dataReadEs = field.read(dataReadIter, col.size());
        GASSERT(dataReadEs == comms::ErrorStatus::Success);
        static_cast<void>(dataReadEs);

        return comms::ErrorStatus::Success;
    }
};

}  // namespace protocol

}  // namespace comms



