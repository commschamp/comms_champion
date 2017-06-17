//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include <cassert>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

#include "comms/comms.h"
#include "ProtocolMessageBase.h"

namespace comms_champion
{

namespace details
{

template <typename TMsgBase, typename TFields>
class TransportMessageImpl : public
    comms::MessageBase<
        TMsgBase,
        comms::option::NoIdImpl,
        comms::option::FieldsImpl<TFields>,
        comms::option::MsgType<TransportMessageImpl<TMsgBase, TFields> >
    >
{

};

}  // namespace details

/// @breif Base class for @b TransportMessage definition in @b protocol
///     plugin.
/// @tparam TMessage Common interface class for @b protocol plugin.
/// @tparam TAllFields All transport fields difined by <b>protocol stack</b>.
/// @headerfile comms_champion/TransportMessageBase.h
template <typename TMessage, typename TAllFields>
class TransportMessageBase : public
    ProtocolMessageBase<
        details::TransportMessageImpl<TMessage, TAllFields>,
        TransportMessageBase<TMessage, TAllFields>
    >
{
public:
    /// @brief Destructor
    virtual ~TransportMessageBase() = default;

protected:
    /// @brief Overriding virtual comms_champion::Message::nameImpl()
    virtual const char* nameImpl() const override
    {
        static const char* Str = "Generic Transport Message";
        return Str;
    }

    /// @brief Overriding virtual comms_champion::Message::idAsStringImpl()
    virtual QString idAsStringImpl() const override
    {
        assert(!"Mustn't be called");
        return QString();
    }

    /// @brief Overriding virtual comms_champion::Message::resetImpl()
    virtual void resetImpl() override
    {
        assert(!"Mustn't be called");
    }

    /// @brief Overriding virtual comms_champion::Message::assignImpl()
    virtual bool assignImpl(const comms_champion::Message& other) override
    {
        static_cast<void>(other);
        assert(!"Mustn't be called");
        return false;
    }
};

}  // namespace comms_champion


