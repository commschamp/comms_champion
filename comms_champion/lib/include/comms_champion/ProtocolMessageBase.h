//
// Copyright 2015 - 2018 (C). Alex Robenko. All rights reserved.
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
#include <string>
#include <cassert>

#include "comms_champion/Message.h"
#include "comms/MessageBase.h"

namespace comms_champion
{

/// @brief Helper class used to implement several pure
///     virtual functions defined in comms_champion::Message interface
///     class.
/// @details It can be used to eliminate boilerplate code for implementing
///     resetImpl() and assignImpl() function. The code looks the same for
///     every message class.
/// @tparam TMsgBase Type of the actual message, implemented as part of
///     the protocol
/// @tparam TActualMessage Type of the actual message class inheriting from
///     this one
/// @headerfile comms_champion/ProtocolMessageBase.h
template <typename TMsgBase, typename TActualMsg>
class ProtocolMessageBase : public TMsgBase
{
    using Base = TMsgBase;
    using ActualMsg = TActualMsg;

protected:

    /// @brief Default constructor
    ProtocolMessageBase() = default;

    /// @brief Copy constructor
    ProtocolMessageBase(const ProtocolMessageBase&) = default;

    /// @brief Move constructor
    ProtocolMessageBase(ProtocolMessageBase&&) = default;

    /// @brief Destructor
    ~ProtocolMessageBase() noexcept = default;

    /// @brief Copy assignment
    ProtocolMessageBase& operator=(const ProtocolMessageBase&) = default;

    /// @brief Move assignment
    ProtocolMessageBase& operator=(ProtocolMessageBase&&) = default;

    /// @brief Overriding implementation to comms_champion::Message::resetImpl()
    virtual void resetImpl() override
    {
        auto& actObj = static_cast<ActualMsg&>(*this);
        actObj = ActualMsg();
    }

    /// @brief Overriding implementation to @ref comms_champion::Message::assignImpl()
    virtual bool assignImpl(const comms_champion::Message& other) override
    {
        auto* castedOther = dynamic_cast<const ActualMsg*>(&other);
        if (castedOther == nullptr) {
            return false;
        }

        assert(other.idAsString() == Base::idAsString());
        auto& actObj = static_cast<ActualMsg&>(*this);
        actObj = *castedOther;
        return true;
    }

    /// @brief Overriding implementation to @ref comms_champion::Message::nameImpl()
    virtual const char* nameImpl() const override
    {
        static_assert(comms::isMessageBase<TMsgBase>(), "TMsgBase is expected to be proper message");

        using Tag = typename std::conditional<
                TMsgBase::ImplOptions::HasName,
                HasNameTag,
                NoNameTag
            >::type;
        return nameInternal(Tag());
    }

private:
    struct HasNameTag {};
    struct NoNameTag {};

    const char* nameInternal(HasNameTag) const
    {
        return TMsgBase::doName();
    }

    const char* nameInternal(NoNameTag) const
    {
        assert(!"nameImpl() needs to be overriden with proper value");
        return nullptr;
    }

};

}  // namespace comms_champion


