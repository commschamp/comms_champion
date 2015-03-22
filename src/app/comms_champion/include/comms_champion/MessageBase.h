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

#include <type_traits>
#include <cassert>
#include <iterator>
#include <algorithm>

#include "comms/comms.h"

#include "Message.h"
#include "DefaultMessageDisplayHandler.h"

namespace comms_champion
{

template <typename TTraits, typename THandler = DefaultMessageDisplayHandler>
class MessageBase : public Message, public comms::Message<TTraits, THandler>
{
    using CCBase = Message;
    using CommsBase = comms::Message<TTraits, THandler>;
public:
    MessageBase() = default;
    MessageBase(const MessageBase&) = default;
    MessageBase(MessageBase&&) = default;
    ~MessageBase() = default;
    MessageBase& operator=(const MessageBase&) = default;
    MessageBase& operator=(MessageBase&&) = default;
protected:
    virtual void displayImpl(MessageDisplayHandler& handler) override
    {
        auto* castedHandler = dynamic_cast<THandler*>(&handler);
        if (castedHandler != nullptr) {
            CommsBase::dispatch(*castedHandler);
        }
    }

    virtual bool isValidImpl() const override
    {
        return CommsBase::valid();
    }

    virtual DataSeq serialiseDataImpl() const override
    {
        typedef typename CommsBase::WriteIterator WriteIterator;
        typedef typename std::iterator_traits<WriteIterator>::iterator_category Tag;

        static_assert(
            std::is_base_of<std::random_access_iterator_tag, Tag>::value ||
            std::is_base_of<std::output_iterator_tag, Tag>::value,
            "Only random access or output iterator is supported for data serialisation.");

        return serialiseDataIntenal(Tag());
    }

private:
    struct UseBackInserterTag {};
    struct WriteThanCopyTag {};

    DataSeq serialiseDataIntenal(std::random_access_iterator_tag) const
    {
        return serialiseDataRandomAccess();
    }

    DataSeq serialiseDataIntenal(std::output_iterator_tag) const
    {
        typedef typename CommsBase::WriteIterator WriteIterator;
        typedef typename
            std::conditional<
                std::is_same<std::back_insert_iterator<DataSeq>, WriteIterator>::value,
                UseBackInserterTag,
                WriteThanCopyTag
            >::type Tag;

        static_assert(
            std::is_same<Tag, UseBackInserterTag>::value,
            "Currently only back_insert_iterator is supported.");
        return serialiseDataWithOutputIter(Tag());
    }

    DataSeq serialiseDataRandomAccess() const
    {
        DataSeq data;
        try {
            do {
                data.resize(CommsBase::length());
                typename CommsBase::ReadIterator iter = &data[0];
                auto es = CommsBase::write(iter, data.size());
                if (es != comms::ErrorStatus::Success) {
                    assert(!"Data serialisation failed");
                    data.clear();
                    break;
                }

                typename CommsBase::ReadIterator begIter = &data[0];
                data.resize(
                    static_cast<std::size_t>(std::distance(begIter, iter)));
            } while (false);
        }
        catch (...) {
            data.clear();
        }
        return data;
    }

    DataSeq serialiseDataWithOutputIter(UseBackInserterTag) const
    {
        DataSeq data;
        auto iter = std::back_inserter(data);
        auto es = CommsBase::write(iter, data.max_size());
        assert(es == comms::ErrorStatus::Success);
        static_cast<void>(es);
        return data;
    }
};

}  // namespace comms_champion


