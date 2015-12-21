//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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
#include <utility>
#include <algorithm>

#include "comms/ErrorStatus.h"
#include "comms/util/Tuple.h"
#include "comms/Assert.h"

namespace comms
{

namespace protocol
{

namespace details
{

template <class T, class R = void>
struct EnableIfHasAllMessages { typedef R Type; };

template <class T, class Enable = void>
struct AllMessagesHelper
{
    typedef void Type;
};

template <class T>
struct AllMessagesHelper<T, typename EnableIfHasAllMessages<typename T::AllMessages>::Type>
{
    typedef typename T::AllMessages Type;
};

template <class T>
using AllMessagesType = typename AllMessagesHelper<T>::Type;

}  // namespace details

/// @brief Base class for all the middle (non @ref MsgDataLayer) protocol transport layers.
/// @details Provides all the default and/or common functionality for the
///     middle transport layer. The inheriting actual layer class may
///     use and/or override the provided functionality by redefining member
///     function with the same signature. The @ref NextLayer is stored as a private
///     data meber.
/// @tparam TField Every middle layer will have a field containing extra
///     information for this layer. This template parameter is a type of such
///     field.
/// @tparam TNextLayer Next layer this one wraps and forwards the calls to.
template <typename TField, typename TNextLayer>
class ProtocolLayerBase
{
public:
    /// @brief Type of the field used for this layer.
    typedef TField Field;

    /// @brief Type of the next transport layer
    typedef TNextLayer NextLayer;

    /// @brief Type of all the fields of all the transport layers
    ///     wrapped in std::tuple.
    /// @details The @ref Field type is prepended to the @ref AllFields type
    ///     of the @ref NextLayer and reported as @ref AllFields of this one.
    typedef typename std::decay<
        decltype(
            std::tuple_cat(
                std::declval<std::tuple<Field> >(),
                std::declval<typename TNextLayer::AllFields>())
            )
        >::type AllFields;

    /// @brief All supported messages.
    /// @details Same as NextLayer::AllMessages or void if such doesn't exist.
    typedef details::AllMessagesType<NextLayer> AllMessages;

    /// @brief Type of pointer to the message.
    /// @details Same as NextLayer::MsgPtr.
    typedef typename NextLayer::MsgPtr MsgPtr;

    /// @brief Type of the custom message interface.
    /// @details Same as NextLayer::Message.
    typedef typename NextLayer::Message Message;

    /// @brief Type used for message ID.
    typedef typename Message::MsgIdType MsgIdType;

    /// @brief Type used for pas message ID as a parameter.
    typedef typename Message::MsgIdParamType MsgIdParamType;

    /// @brief Type of the iterator used for reading.
    /// @details Same as NextLayer::ReadIterator.
    typedef typename NextLayer::ReadIterator ReadIterator;

    /// @brief Type of the iterator used for writing.
    /// @details Same as NextLayer::WriteIterator.
    typedef typename NextLayer::WriteIterator WriteIterator;

    /// @copydoc MsgDataLayer::NumOfLayers
    static const std::size_t NumOfLayers = 1 + NextLayer::NumOfLayers;

    /// @brief Copy constructor
    ProtocolLayerBase(const ProtocolLayerBase&) = default;

    /// @brief Move constructor
    ProtocolLayerBase(ProtocolLayerBase&&) = default;

    /// @brief Constructor.
    /// @details Forwards all the parameters to the constructor of the embedded
    ///     @ref NextLayer object.
    template <typename... TArgs>
    explicit ProtocolLayerBase(TArgs&&... args)
      : nextLayer_(std::forward<TArgs>(args)...)
    {
    }

    /// @brief Desctructor
    ~ProtocolLayerBase() = default;

    /// @brief Copy assignment
    ProtocolLayerBase& operator=(const ProtocolLayerBase&) = default;

    /// @brief Get access to the next layer object.
    NextLayer& nextLayer()
    {
        return nextLayer_;
    }

    /// @brief Get "const" access to the next layer object.
    const NextLayer& nextLayer() const
    {
        return nextLayer_;
    }

    /// @brief Get remaining length of wrapping transport information.
    /// @details The message data always get wrapped with transport information
    ///     to be successfully delivered to and unpacked on the other side.
    ///     This function return remaining length of the transport information.
    /// @return length of the field + length reported by the next layer.
    constexpr std::size_t length() const
    {
        return Field::minLength() + nextLayer_.length();
    }

    /// @brief Get remaining length of wrapping transport information + length
    ///     of the provided message.
    /// @details This function usually gets called when there is a need to
    ///     identify the size of the buffer required to write provided message
    ///     wrapped in the transport information. This function is very similar
    ///     to length(), but adds also length of the message.
    /// @param[in] msg Message
    /// @return length of the field + length reported by the next layer.
    template <typename TMsg>
    constexpr std::size_t length(const TMsg& msg) const
    {
        return Field::minLength() + nextLayer_.length(msg);
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
    ///     by the length of the @ref Field.
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Number of bytes that have been written using write().
    /// @return Status of the update operation.
    template <typename TIter>
    comms::ErrorStatus update(TIter& iter, std::size_t size) const
    {
        return updateInternal(iter, size, LengthTag());
    }

    /// @brief Update recently written (using writeFieldsCached()) message data as
    ///     well as cached transport information fields.
    /// @details Very similar to update() member function, but adds "allFields"
    ///     parameter to store raw data of the message.
    /// @tparam TIdx Index of the data field in TAllFields.
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
    ErrorStatus updateFieldsCached(
        TAllFields& allFields,
        TUpdateIter& iter,
        std::size_t size) const
    {
        return updateFieldsCachedInternal<TIdx>(allFields, iter, size, LengthTag());
    }

    /// @brief Create message object given the ID.
    /// @details The default implementation is to forwards this call to the next
    ///     layer. One of the layers (usually comms::protocol::MsgIdLayer)
    ///     hides and overrides this implementation.
    /// @param id ID of the message.
    /// @param idx Relative index of the message with the same ID.
    /// @return Smart pointer (variant of std::unique_ptr) to allocated message
    ///     object
    MsgPtr createMsg(MsgIdParamType id, unsigned idx = 0)
    {
        return nextLayer_.createMsg(id, idx);
    }

protected:

    /// @cond SKIP_DOC
    struct FixedLengthTag {};
    struct VarLengthTag {};
    typedef typename std::conditional<
        (Field::minLength() == Field::maxLength()),
        FixedLengthTag,
        VarLengthTag
    >::type LengthTag;

    class NextLayerReader
    {
    public:
        explicit NextLayerReader(NextLayer& nextLayer)
          : nextLayer_(nextLayer)
        {
        }

        template <typename TMsgPtr>
        ErrorStatus read(
            TMsgPtr& msg,
            ReadIterator& iter,
            std::size_t size,
            std::size_t* missingSize)
        {
            return nextLayer_.read(msg, iter, size, missingSize);
        }
    private:
        NextLayer& nextLayer_;
    };

    template <std::size_t TIdx, typename TAllFields>
    class NextLayerCachedFieldsReader
    {
    public:
        NextLayerCachedFieldsReader(
            NextLayer& nextLayer,
            TAllFields& allFields)
          : nextLayer_(nextLayer),
            allFields_(allFields)
        {
        }

        template<typename TMsgPtr>
        ErrorStatus read(
            TMsgPtr& msg,
            ReadIterator& iter,
            std::size_t size,
            std::size_t* missingSize)
        {
            return nextLayer_.template readFieldsCached<TIdx + 1>(allFields_, msg, iter, size, missingSize);
        }

    private:
        NextLayer& nextLayer_;
        TAllFields& allFields_;
    };

    class NextLayerWriter
    {
    public:

        explicit NextLayerWriter(const NextLayer& nextLayer)
          : nextLayer_(nextLayer)
        {
        }

        ErrorStatus write(
            const Message& msg,
            WriteIterator& iter,
            std::size_t size) const
        {
            return nextLayer_.write(msg, iter, size);
        }

    private:
        const NextLayer& nextLayer_;
    };

    template <std::size_t TIdx, typename TAllFields>
    class NextLayerCachedFieldsWriter
    {
    public:
        NextLayerCachedFieldsWriter(
            const NextLayer nextLayer,
            TAllFields& allFields)
          : nextLayer_(nextLayer),
            allFields_(allFields)
        {
        }

        ErrorStatus write(
            const Message& msg,
            WriteIterator& iter,
            std::size_t size) const
        {
            return nextLayer_.template writeFieldsCached<TIdx + 1>(allFields_, msg, iter, size);
        }

    private:
        const NextLayer& nextLayer_;
        TAllFields& allFields_;
    };

    class NextLayerUpdater
    {
    public:

        explicit NextLayerUpdater(const NextLayer& nextLayer)
          : nextLayer_(nextLayer)
        {
        }

        template <typename TUpdateIter>
        ErrorStatus update(
            TUpdateIter& iter,
            std::size_t size) const
        {
            return nextLayer_.update(iter, size);
        }

    private:
        const NextLayer& nextLayer_;
    };

    template <std::size_t TIdx, typename TAllFields>
    class NextLayerCachedFieldsUpdater
    {
    public:
        NextLayerCachedFieldsUpdater(
            const NextLayer nextLayer,
            TAllFields& allFields)
          : nextLayer_(nextLayer),
            allFields_(allFields)
        {
        }

        template <typename TUpdateIter>
        ErrorStatus update(
            TUpdateIter& iter,
            std::size_t size) const
        {
            return nextLayer_.template updateFieldsCached<TIdx + 1>(allFields_, iter, size);
        }

    private:
        const NextLayer& nextLayer_;
        TAllFields& allFields_;
    };

    void updateMissingSize(std::size_t size, std::size_t* missingSize) const
    {
        if (missingSize != nullptr) {
            GASSERT(size <= length());
            *missingSize = std::max(std::size_t(1U), length() - size);
        }
    }

    void updateMissingSize(
        const Field& field,
        std::size_t size,
        std::size_t* missingSize) const
    {
        if (missingSize != nullptr) {
            auto totalLen = field.length() + nextLayer_.length();
            GASSERT(size <= totalLen);
            *missingSize = std::max(std::size_t(1U), totalLen - size);
        }
    }

    template <std::size_t TIdx, typename TAllFields>
    static Field& getField(TAllFields& allFields)
    {
        static_assert(comms::util::IsTuple<TAllFields>::Value,
                                        "Expected TAllFields to be a tuple");
        static_assert(TIdx < std::tuple_size<TAllFields>::value,
                                        "Invalid tuple access index");

        auto& field = std::get<TIdx>(allFields);

        typedef typename std::decay<decltype(field)>::type FieldType;
        static_assert(
            std::is_same<Field, FieldType>::value,
            "Field has wrong type");

        return field;
    }

    NextLayerReader createNextLayerReader()
    {
        return NextLayerReader(nextLayer_);
    }

    template <std::size_t TIdx, typename TAllFields>
    NextLayerCachedFieldsReader<TIdx, TAllFields>
    createNextLayerCachedFieldsReader(TAllFields& fields)
    {
        return NextLayerCachedFieldsReader<TIdx, TAllFields>(nextLayer_, fields);
    }

    NextLayerWriter createNextLayerWriter() const
    {
        return NextLayerWriter(nextLayer_);
    }

    template <std::size_t TIdx, typename TAllFields>
    NextLayerCachedFieldsWriter<TIdx, TAllFields>
    createNextLayerCachedFieldsWriter(TAllFields& fields) const
    {
        return NextLayerCachedFieldsWriter<TIdx, TAllFields>(nextLayer_, fields);
    }

    NextLayerUpdater createNextLayerUpdater() const
    {
        return NextLayerUpdater(nextLayer_);
    }

    template <std::size_t TIdx, typename TAllFields>
    NextLayerCachedFieldsUpdater<TIdx, TAllFields>
    createNextLayerCachedFieldsUpdater(TAllFields& fields) const
    {
        return NextLayerCachedFieldsUpdater<TIdx, TAllFields>(nextLayer_, fields);
    }

    /// @endcond
private:

    template <typename TIter>
    comms::ErrorStatus updateInternal(TIter& iter, std::size_t size, FixedLengthTag) const
    {
        auto len = Field().length();
        GASSERT(len <= size);
        std::advance(iter, len);
        return nextLayer_.update(iter, size - len);
    }

    template <typename TIter>
    comms::ErrorStatus updateInternal(TIter& iter, std::size_t size, VarLengthTag) const
    {
        Field field;
        auto es = field.read(iter, size);
        if (es == comms::ErrorStatus::Success) {
            es = nextLayer_.update(iter, size - field.length());
        }
        return es;
    }

    template <std::size_t TIdx, typename TAllFields, typename TUpdateIter>
    ErrorStatus updateFieldsCachedInternal(
        TAllFields& allFields,
        TUpdateIter& iter,
        std::size_t size,
        FixedLengthTag) const
    {
        auto len = Field().length();
        GASSERT(len <= size);
        std::advance(iter, len);
        return nextLayer_.updateFieldsCached<TIdx + 1>(allFields, iter, size - len);
    }

    template <std::size_t TIdx, typename TAllFields, typename TUpdateIter>
    ErrorStatus updateFieldsCachedInternal(
        TAllFields& allFields,
        TUpdateIter& iter,
        std::size_t size,
        VarLengthTag) const
    {
        Field field;
        auto es = field.read(iter, size);
        if (es == comms::ErrorStatus::Success) {
            es = nextLayer_.updateFieldsCached<TIdx + 1>(allFields, iter, size - field.length());
        }
        return es;
    }


    static_assert (comms::util::IsTuple<AllFields>::Value, "Must be tuple");
    NextLayer nextLayer_;
};

}  // namespace protocol

}  // namespace comms
