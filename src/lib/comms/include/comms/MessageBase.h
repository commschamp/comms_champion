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

#include <utility>
#include <cstdint>
#include <algorithm>
#include <type_traits>

#include "util/Tuple.h"
#include "util/access.h"

#include "ErrorStatus.h"
#include "Assert.h"
#include "options.h"

namespace comms
{

template <typename TMessage,
          typename... TOptions>
class MessageBase;

template <typename TMessage>
class MessageBase<TMessage> : public TMessage
{
    typedef TMessage Base;
protected:
    template <typename... TParams>
    MessageBase(TParams&& args) : Base(std::forward<TParams>(args)...) {}
};

template <typename TMessage,
          typename TMessage::MsgIdType TId,
          typename... TRest>
class MessageBase<TMessage, TId, TRest...> : public MessageBase<TMessage, TRest...>
{
    typedef MessageBase<TMessage, TRest...> Base;
    typedef typename Base::MsgIdReturnType MsgIdReturnType;

protected:
    template <typename... TParams>
    MessageBase(TParams&& args) : Base(std::forward<TParams>(args)...) {}

    virtual MsgIdReturnType getIdImpl() const override
    {
        return TId;
    }
};

template <typename TMessage,
          typename TActual,
          typename... TRest>
class MessageBase<TMessage, option::DispatchImpl<TActual>, TRest...> :
                                        public MessageBase<TMessage, TRest...>
{
    typedef MessageBase<TMessage, TRest...> Base;
    typedef typename Base::Handler Handler;
    typedef option::DispatchImpl<TActual> DispatchOption;
    typedef typename DispatchOption::MsgType MsgType;

protected:
    template <typename... TParams>
    MessageBase(TParams&& args) : Base(std::forward<TParams>(args)...) {}

#ifndef COMMS_NO_DISPATCH
    virtual void dispatchImpl(Handler& handler) override
    {
        handler.handle(static_cast<MsgType&>(*this));
    }
#endif // #ifndef COMMS_NO_DISPATCH
};

template <typename TMessage,
          typename TFields,
          typename... TRest>
class MessageBase<TMessage, option::FieldsImpl<TFields>, TRest...> :
                                        public MessageBase<TMessage, TRest...>
{
    typedef MessageBase<TMessage, TRest...> Base;
    typedef option::FieldsImpl<TActual> FieldsOption;
    typedef typename FieldsOption::Fields Fields;

public:
    Fields& getFields()
    {
        return fields_;
    }

    const Fields& getFields() const
    {
        return fields_;
    }

    template <typename TNewFields>
    void setFields(TNewFields&& fields)
    {
        fields_ = std::forward<TNewFields>(fields);
    }

protected:
    template <typename... TParams>
    MessageBase(TParams&& args) : Base(std::forward<TParams>(args)...) {}

#ifndef COMMS_NO_READ
    virtual ErrorStatus readImpl(ReadIterator& iter, std::size_t size) override
    {
        ErrorStatus status = ErrorStatus::Success;
        std::size_t remainingSize = size;
        util::tupleForEach(fields_, FieldReader(iter, status, remainingSize));
        return status;
    }
#endif // #ifndef COMMS_NO_READ

#ifndef COMMS_NO_WRITE
    virtual ErrorStatus writeImpl(WriteIterator& iter, std::size_t size) const override
    {
        ErrorStatus status = ErrorStatus::Success;
        std::size_t remainingSize = size;
        util::tupleForEach(fields_, FieldWriter(iter, status, remainingSize));
        return status;
    }
#endif // #ifndef COMMS_NO_WRITE

#if !defined(COMMS_NO_READ) || !defined(COMMS_NO_WRITE) || !defined(COMMS_NO_LENGTH)
    virtual std::size_t lengthImpl() const override
    {
        return util::tupleAccumulate(fields_, 0U, FieldLengthRetriever());
    }
#endif // #if !defined(COMMS_NO_READ) || !defined(COMMS_NO_WRITE) || !defined(COMMS_NO_LENGTH)


private:

    /// @cond DOCUMENT_FIELD_READER_WRITER_SIZE_GETTER
    class FieldReader
    {
    public:
        FieldReader(ReadIterator& iter, ErrorStatus& status, std::size_t& size)
            : iter_(iter),
              status_(status),
              size_(size)
        {

        }

        template <typename TField>
        void operator()(TField& field) {
            if (status_ == ErrorStatus::Success) {
                status_ = field.read(iter_, size_);
                if (status_ == ErrorStatus::Success) {
                    GASSERT(field.length() <= size_);
                    size_ -= field.length();
                }
            }
        }
    private:
        ReadIterator& iter_;
        ErrorStatus status_;
        std::size_t& size_;
    };

    class FieldWriter
    {
    public:
        FieldWriter(WriteIterator& iter, ErrorStatus& status, std::size_t& size)
            : iter_(iter),
              status_(status),
              size_(size)
        {
        }

        template <typename TField>
        void operator()(TField& field) {
            if (status_ == ErrorStatus::Success) {
                status_ = field.write(iter_, size_);
                if (status_ == ErrorStatus::Success) {
                    GASSERT(field.length() <= size_);
                    size_ -= field.length();
                }
            }
        }

    private:
        WriteIterator& iter_;
        ErrorStatus status_;
        std::size_t& size_;
    };


    struct FieldLengthRetriever
    {
        template <typename TField>
        std::size_t operator()(std::size_t size, const TField& field)
        {
            return size + field.length();
        }
    };
    /// @endcond

    Fields fields_;
};

template <typename TMessage,
          typename... TRest>
class MessageBase<TMessage, option::NoFieldsImpl, TRest...> :
            public MessageBase<TMessage, option::FieldsImpl<std::tuple<> >, TRest...>
{
    typedef MessageBase<TMessage, option::FieldsImpl<std::tuple<> >, TRest...> Base;

protected:
    template <typename... TParams>
    MessageBase(TParams&& args) : Base(std::forward<TParams>(args)...) {}
};



}  // namespace comms


