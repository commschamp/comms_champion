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

template <typename TField, typename TNextLayer>
class ProtocolLayerBase
{
public:
    typedef TField Field;

    typedef TNextLayer NextLayer;

    typedef typename std::decay<
        decltype(
            std::tuple_cat(
                std::declval<std::tuple<Field> >(),
                std::declval<typename TNextLayer::AllFields>())
            )
        >::type AllFields;

    typedef typename NextLayer::MsgPtr MsgPtr;

    typedef typename NextLayer::Message Message;

    typedef typename Message::MsgIdType MsgIdType;

    typedef typename Message::MsgIdParamType MsgIdParamType;

    typedef typename NextLayer::ReadIterator ReadIterator;

    typedef typename NextLayer::WriteIterator WriteIterator;

    ProtocolLayerBase() = default;
    ProtocolLayerBase(const ProtocolLayerBase&) = default;

    template <typename... TArgs>
    ProtocolLayerBase(TArgs&&... args)
      : nextLayer_(std::forward<TArgs>(args)...)
    {
    }

    ~ProtocolLayerBase() = default;
    ProtocolLayerBase& operator=(const ProtocolLayerBase&) = default;

    NextLayer& nextLayer()
    {
        return nextLayer_;
    }

    const NextLayer& nextLayer() const
    {
        return nextLayer_;
    }

    constexpr std::size_t length() const
    {
        return Field().length() + nextLayer_.length();
    }

    template <typename TMsg>
    constexpr std::size_t length(const TMsg& msg) const
    {
        return Field().length() + nextLayer_.length(msg);
    }

    template <typename TIter>
    comms::ErrorStatus update(TIter& iter, std::size_t size) const
    {
        auto len = length();
        std::advance(iter, len);
        return nextLayer_.update(iter, size - len);
    }

    MsgPtr createMsg(MsgIdParamType id)
    {
        return nextLayer_.createMsg(id);
    }

protected:

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

    void updateMissingSize(std::size_t size, std::size_t* missingSize)
    {
        if (missingSize != nullptr) {
            GASSERT(size <= length());
            *missingSize = std::max(std::size_t(1U), length() - size);
        }
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

private:

    static_assert (comms::util::IsTuple<AllFields>::Value, "Must be tuple");
    NextLayer nextLayer_;
};

}  // namespace protocol

}  // namespace comms
