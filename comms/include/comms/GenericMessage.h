//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
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

/// @file
/// Provides implementation of @ref comms::GenericMessage class

#pragma once

#include <tuple>
#include <cstdint>

#include "options.h"
#include "MessageBase.h"
#include "field/ArrayList.h"

namespace comms
{

template <typename TFieldBase, typename TExtraOpts = comms::option::EmptyOption>
using GenericMessageFields =
    std::tuple<
        comms::field::ArrayList<
            TFieldBase,
            std::uint8_t,
            TExtraOpts
        >
    >;

/// @brief Generic Message
template <
    typename TMessage,
    typename TFieldOpts = comms::option::EmptyOption,
    typename TExtraOpts = comms::option::EmptyOption
>
class GenericMessage : public
    comms::MessageBase<
        TMessage,
        comms::option::FieldsImpl<GenericMessageFields<typename TMessage::Field, TFieldOpts> >,
        comms::option::MsgType<GenericMessage<TMessage, TFieldOpts, TExtraOpts> >,
        comms::option::HasDoGetId,
        TExtraOpts
    >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::FieldsImpl<GenericMessageFields<typename TMessage::Field, TFieldOpts> >,
            comms::option::MsgType<GenericMessage<TMessage, TFieldOpts, TExtraOpts> >,
            comms::option::HasDoGetId,
            TExtraOpts
        >;
public:
    using MsgIdType = typename Base::MsgIdType;
    using MsgIdParamType = typename Base::MsgIdParamType;

    explicit GenericMessage(MsgIdParamType id) : m_id(id) {}
    GenericMessage(const GenericMessage&) = default;
    GenericMessage(GenericMessage&&) = default;
    ~GenericMessage() = default;
    GenericMessage& operator=(const GenericMessage&) = default;
    GenericMessage& operator=(GenericMessage&&) = default;

    COMMS_MSG_FIELDS_ACCESS(data);

public:
    MsgIdParamType doGetId() const
    {
        return m_id;
    }
private:
    MsgIdType m_id;
};

} // namespace comms
