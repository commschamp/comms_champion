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

#include <tuple>
#include <iterator>
#include "comms/Assert.h"
#include "comms/Field.h"
#include "comms/util/Tuple.h"
#include "comms/field/ArrayList.h"
#include "comms/field/IntValue.h"
#include "ProtocolLayerBase.h"

namespace comms
{

namespace protocol
{

/// @brief Message data layer.
/// @details Must always be the last layer in protocol stack.
/// @tparam TField Field that can be used to store raw data of the message.
/// @headerfile comms/protocol/MsgDataLayer.h
template <
    typename TField =
        comms::field::ArrayList<
            comms::Field<comms::option::BigEndian>,
            std::uint8_t
        >
>
class MsgDataLayer
{
public:

    /// @brief Raw data field type.
    using Field = TField;

    /// @brief All fields of the remaining transport layers, contains only @ref Field.
    using AllFields = std::tuple<Field>;

    /// @brief Static constant indicating amount of transport layers used.
    static const std::size_t NumOfLayers = 1;

    /// @brief Default constructor
    MsgDataLayer() = default;

    /// @brief Copy constructor
    MsgDataLayer(const MsgDataLayer&) = default;

    /// @brief Move constructor
    MsgDataLayer(MsgDataLayer&&) = default;

    /// @brief Destructor
    ~MsgDataLayer() noexcept = default;

    /// @brief Copy assignment operator
    MsgDataLayer& operator=(const MsgDataLayer&) = default;

    /// @brief Move assignment operator
    MsgDataLayer& operator=(MsgDataLayer&&) = default;

    /// @brief Compile time check whether split read "until" and "from" data
    ///     layer is allowed.
    /// @return Always @b true.
    static constexpr bool canSplitRead()
    {
        return true;
    }

    /// @brief Read the message contents.
    /// @details Calls the read() member function of the message object.
    /// @tparam TMsgPtr Type of the smart pointer to the allocated message object.
    /// @tparam TIter Type of iterator used for reading.
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
    template <typename TMsgPtr, typename TIter>
    static ErrorStatus read(
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        using MsgPtrType = typename std::decay<decltype(msgPtr)>::type;
        using MsgType = typename MsgPtrType::element_type;

        static_assert(
            details::ProtocolLayerHasInterfaceOptions<MsgType>::Value,
            "The provided message object must inherit from comms::Message");

        static_assert(MsgType::InterfaceOptions::HasReadIterator,
            "Message interface must support polymorphic read operation");

        using IterType = typename std::decay<decltype(iter)>::type;

        static_assert(std::is_convertible<IterType, typename MsgType::ReadIterator>::value,
            "The provided iterator is not convertible to ReadIterator defined by Message class");

        using ReadIter = typename std::add_lvalue_reference<typename MsgType::ReadIterator>::type;

        GASSERT(msgPtr);
        auto result = msgPtr->read(static_cast<ReadIter>(iter), size);
        if ((result == ErrorStatus::NotEnoughData) &&
            (missingSize != nullptr)) {
            using Tag = typename std::conditional<
                MsgType::InterfaceOptions::HasLength,
                MsgHasLengthTag,
                MsgNoLengthTag>::type;

            auto msgLen = getMsgLength(*msgPtr, Tag());
            if (size < msgLen) {
                *missingSize = msgLen - size;
            }
            else {
                *missingSize = 1;
            }
        }
        return result;
    }

    /// @brief Read transport fields until data layer.
    /// @details Does nothing because it is data layer.
    /// @return @ref comms::ErrorStatus::Success;
    template <typename TMsgPtr, typename TIter>
    static ErrorStatus readUntilData(
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        static_cast<void>(msgPtr);
        static_cast<void>(iter);
        static_cast<void>(size);
        static_cast<void>(missingSize);
        return comms::ErrorStatus::Success;
    }

    /// @brief Same as @ref read().
    /// @details Expected to be called by the privous layers to properly
    ///     finalise read operation after the call to @ref readUntilData();
    /// @return @ref comms::ErrorStatus::Success;
    template <typename TMsgPtr, typename TIter>
    static ErrorStatus readFromData(
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        return read(msgPtr, iter, size, missingSize);
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
    /// @tparam TIter Type of iterator used for reading.
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
    /// @return Status of the read operation.
    template <std::size_t TIdx, typename TAllFields, typename TMsgPtr, typename TIter>
    static ErrorStatus readFieldsCached(
        TAllFields& allFields,
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        using MsgPtrType = typename std::decay<decltype(msgPtr)>::type;
        using MsgType = typename MsgPtrType::element_type;

        static_assert(
            details::ProtocolLayerHasInterfaceOptions<MsgType>::Value,
            "The provided message object must inherit from comms::Message");

        static_assert(MsgType::InterfaceOptions::HasReadIterator,
            "Message interface must support polymorphic read operation");

        using IterType = typename std::decay<decltype(iter)>::type;

        static_assert(std::is_convertible<IterType, typename MsgType::ReadIterator>::value,
            "The provided iterator is not convertible to ReadIterator defined by Message class");

        static_assert(comms::util::IsTuple<TAllFields>::Value,
                                        "Expected TAllFields to be tuple.");

        static_assert((TIdx + 1) == std::tuple_size<TAllFields>::value,
                    "All fields must be read when MsgDataLayer is reached");

        using IterTag = typename std::iterator_traits<IterType>::iterator_category;
        auto& dataField = std::get<TIdx>(allFields);

        using FieldType = typename std::decay<decltype(dataField)>::type;
        static_assert(
            std::is_same<Field, FieldType>::value,
            "Field has wrong type");
        return readWithFieldCachedInternal(dataField, msgPtr, iter, size, missingSize, IterTag());
    }

    /// @brief Read transport fields with caching until data layer.
    /// @details Does nothing because it is data layer.
    /// @return @ref comms::ErrorStatus::Success;
    template <std::size_t TIdx, typename TAllFields, typename TMsgPtr, typename TIter>
    static ErrorStatus readUntilDataFieldsCached(
        TAllFields& allFields,
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        static_cast<void>(allFields);
        static_cast<void>(msgPtr);
        static_cast<void>(iter);
        static_cast<void>(size);
        static_cast<void>(missingSize);
        return comms::ErrorStatus::Success;
    }

    /// @brief Same as @ref readFieldsCached().
    /// @details Expected to be called by the privous layers to properly
    ///     finalise read operation after the call to @ref readUntilDataFieldsCached();
    /// @return @ref comms::ErrorStatus::Success;
    template <std::size_t TIdx, typename TAllFields, typename TMsgPtr, typename TIter>
    static ErrorStatus readFromDataFeildsCached(
        TAllFields& allFields,
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        return readFieldsCached(allFields, msgPtr, iter, size, missingSize);
    }


    /// @brief Write the message contents.
    /// @details The way the message contents are written is determined by the
    ///     type of the message. If TMsg type is recognised to be actual message
    ///     inheriting from comms::MessageBase with its fields provided using
    ///     comms::option::FieldsImpl option, the function calls @b doWrite
    ///     non-virtual function defined by comms::MessageBase
    ///     (see comms::MessageBase::doWrite) or redefined by the actual
    ///     message itself. Otherwise, TMsg type is expected to be the used
    ///     interface which allows polymorphic write functionality.
    ///     It must define @b write() member function which will be
    ///     called.
    /// @tparam TMsg Type of the message.
    /// @tparam TIter Type of the iterator used for writing.
    /// @param[in] msg Reference to the message object,
    /// @param[in, out] iter Iterator used for writing.
    /// @param[in] size Max number of bytes that can be written.
    /// @return Status of the write operation.
    template <typename TMsg, typename TIter>
    static ErrorStatus write(
        const TMsg& msg,
        TIter& iter,
        std::size_t size)
    {
        using MsgType = typename std::decay<decltype(msg)>::type;

        static_assert(
            details::ProtocolLayerHasInterfaceOptions<MsgType>::Value,
            "The provided message object must inherit from comms::Message");

        using Tag =
            typename std::conditional<
                details::ProtocolLayerHasFieldsImpl<MsgType>::Value,
                DirectWriteTag,
                PolymorphicWriteTag
            >::type;

        return writeInternal(msg, iter, size, Tag());
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
    /// @tparam TMsg Type of the message.
    /// @tparam TIter Type of the iterator used for writing.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in] msg Reference to the message object that is being written,
    /// @param[in, out] iter Iterator used for writing.
    /// @param[in] size Max number of bytes that can be written.
    /// @return Status of the write operation.
    template <std::size_t TIdx, typename TAllFields, typename TMsg, typename TIter>
    static ErrorStatus writeFieldsCached(
        TAllFields& allFields,
        const TMsg& msg,
        TIter& iter,
        std::size_t size)
    {
        static_assert(comms::util::IsTuple<TAllFields>::Value,
                                        "Expected TAllFields to be tuple.");

        static_assert((TIdx + 1) == std::tuple_size<TAllFields>::value,
                    "All fields must be written when MsgDataLayer is reached");

        auto& dataField = std::get<TIdx>(allFields);
        using FieldType = typename std::decay<decltype(dataField)>::type;
        static_assert(
            std::is_same<Field, FieldType>::value,
            "Field has wrong type");

        using IterType = typename std::decay<decltype(iter)>::type;
        using IterTag = typename std::iterator_traits<IterType>::iterator_category;
        return writeWithFieldCachedInternal(dataField, msg, iter, size, IterTag());
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
    /// @tparam TIter Type of the random access iterator.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in, out] iter Random access iterator to the written data.
    /// @param[in] size Number of bytes that have been written using writeFieldsCached().
    /// @return Status of the update operation.
    template <std::size_t TIdx, typename TAllFields, typename TIter>
    static ErrorStatus updateFieldsCached(
        TAllFields& allFields,
        TIter& iter,
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
    template <typename TMsg>
    static constexpr std::size_t length(const TMsg& msg)
    {
        using MsgType = typename std::decay<decltype(msg)>::type;

        static_assert(
            details::ProtocolLayerHasInterfaceOptions<MsgType>::Value,
            "The provided message object must inherit from comms::Message");

        using Tag = typename
            std::conditional<
                details::ProtocolLayerHasFieldsImpl<MsgType>::Value,
                MsgDirectLengthTag,
                MsgHasLengthTag
            >::type;
        return getMsgLength(msg, Tag());
    }

private:

    struct MsgHasLengthTag {};
    struct MsgNoLengthTag {};
    struct MsgDirectLengthTag {};

    struct PolymorphicWriteTag{};
    struct DirectWriteTag{};

    template <typename TMsgPtr, typename TIter>
    static ErrorStatus readWithFieldCachedInternal(
        Field& field,
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize,
        std::random_access_iterator_tag)
    {
        return readWithFieldCachedRandomAccess(field, msgPtr, iter, size, missingSize);
    }

    template <typename TMsgPtr, typename TIter>
    static ErrorStatus readWithFieldCachedInternal(
        Field& field,
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize,
        std::input_iterator_tag)
    {
        static_cast<void>(field);
        static_cast<void>(msgPtr);
        static_cast<void>(iter);
        static_cast<void>(size);
        static_cast<void>(missingSize);
        GASSERT(!"Not supported yet");
        return ErrorStatus::NotSupported;
    }

    template <typename TMsgPtr, typename TIter>
    static ErrorStatus readWithFieldCachedRandomAccess(
        Field& field,
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize)
    {
        using MsgPtrType = typename std::decay<decltype(msgPtr)>::type;
        using MsgType = typename MsgPtrType::element_type;

        typename MsgType::ReadIterator dataIter = iter;
        auto es = read(msgPtr, iter, size, missingSize);
        auto dataSize = static_cast<std::size_t>(std::distance(dataIter, iter));
        auto dataEs = field.read(dataIter, dataSize);
        static_cast<void>(dataEs);
        GASSERT(dataEs == comms::ErrorStatus::Success);
        GASSERT((es != ErrorStatus::Success) || (!msgPtr) || (dataIter == iter));
        return es;
    }

    template <typename TMsg, typename TIter>
    static ErrorStatus writeWithFieldCachedInternal(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        std::random_access_iterator_tag)
    {
        return writeWithFieldCachedRandomAccess(field, msg, iter, size);
    }

    template <typename TMsg, typename TIter>
    static ErrorStatus writeWithFieldCachedInternal(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        std::output_iterator_tag)
    {
        return writeWithFieldCachedOutput(field, msg, iter, size);
    }

    template <typename TMsg, typename TIter>
    static ErrorStatus writeWithFieldCachedRandomAccess(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size)
    {
        auto dataReadIter = iter;
        auto es = write(msg, iter, size);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto writtenCount = static_cast<std::size_t>(std::distance(dataReadIter, iter));
        auto dataEs = field.read(dataReadIter, writtenCount);
        GASSERT(dataEs == comms::ErrorStatus::Success);
        static_cast<void>(dataEs);
        return comms::ErrorStatus::Success;
    }

    template <typename TMsg, typename TCollection>
    static ErrorStatus writeWithFieldCachedOutput(
        Field& field,
        const TMsg& msg,
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

    template <typename TMsg>
    static std::size_t getMsgLength(const TMsg& msg, MsgHasLengthTag)
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(MsgType::InterfaceOptions::HasLength, "Message interface must define length()");
        return msg.length();
    }

    template <typename TMsg>
    static constexpr std::size_t getMsgLength(const TMsg& msg, MsgDirectLengthTag)
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(MsgType::ImplOptions::HasFieldsImpl, "FieldsImpl option hasn't been used");
        return msg.doLength();
    }

    template <typename TMsg>
    static constexpr std::size_t getMsgLength(const TMsg&, MsgNoLengthTag)
    {
        return 0U;
    }

    template <typename TMsg, typename TIter>
    static ErrorStatus writeInternal(
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        PolymorphicWriteTag)
    {
        return writeInternalPolymorhpic(msg, iter, size);
    }

    template <typename TMsg, typename TIter>
    static ErrorStatus writeInternal(
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        DirectWriteTag)
    {
        return writeInternalDirect(msg, iter, size);
    }

    template <typename TMsg, typename TIter>
    static ErrorStatus writeInternalPolymorhpic(
        const TMsg& msg,
        TIter& iter,
        std::size_t size)
    {
        using MsgType = typename std::decay<decltype(msg)>::type;

        static_assert(MsgType::InterfaceOptions::HasWriteIterator,
            "Message interface must support polymorphic write operation");

        using IterType = typename std::decay<decltype(iter)>::type;

        static_assert(std::is_convertible<IterType, typename MsgType::WriteIterator>::value,
            "The provided iterator is not convertible to WriteIterator defined by Message class");

        using WriteIter = typename std::add_lvalue_reference<typename MsgType::WriteIterator>::type;

        return msg.write(static_cast<WriteIter>(iter), size);
    }

    template <typename TMsg, typename TIter>
    static ErrorStatus writeInternalDirect(
        const TMsg& msg,
        TIter& iter,
        std::size_t size)
    {
        return msg.doWrite(iter, size);
    }
};

}  // namespace protocol

}  // namespace comms



