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
};

}  // namespace comms_champion


