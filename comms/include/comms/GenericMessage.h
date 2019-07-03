//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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

/// @brief Definition of fields for @ref comms::GenericMessage message
/// @details Defined as single variable length raw bytes sequence
///     (@ref comms::field::ArrayList).
/// @tparam Base class for the sequence field definition, expected to be a
///     variant of @ref comms::Field
/// @tparam Extra option(s) (bundled as @b std::tuple if multiple) to be
///     passed to @ref comms::field::ArrayList field definition.
template <typename TFieldBase, typename TExtraOpts = comms::option::app::EmptyOption>
using GenericMessageFields =
    std::tuple<
        comms::field::ArrayList<
            TFieldBase,
            std::uint8_t,
            TExtraOpts
        >
    >;

/// @brief Generic Message
/// @details Generic message is there to substitute definition of actual message
///     when contents of the latter are not important. It defines single @b data
///     field as variable length sequence of raw bytes (see @ref GenericMessageFields).
///     The GenericMessage can be useful when implementing some kind of
///     "bridge" or "firewall", that requires knowledge only about message
///     ID and doesn't care much about message contents. The
///     @ref comms::protocol::MsgIdLayer support creation of the
///     GenericMessage in case the received message ID is not known (supported
///     by using @ref comms::option::app::SupportGenericMessage option).
/// @tparam TMessage Common message interface class, becomes one of the
///     base classes.
/// @tparam TFieldOpts Extra option(s) (multiple options need to be bundled in
///     @b std::tuple) to be passed to the definition of the @b data
///     field (see @ref GenericMessageFields).
/// @tparam TExtraOpts Extra option(s) (multple options need to be bundled in
///     @b std::tuple) to be passed to @ref comms::MessageBase which is base
///     to this one.
/// @pre Requires the common message interface (@b TMessage) to define
///     inner @b MsgIdType and @b MsgIdParamType types (expected to use
///     @ref comms::option::def::MsgIdType, see @ref page_use_prot_transport_generic_msg)
/// @headerfile comms/GenericMessage.h
template <
    typename TMessage,
    typename TFieldOpts = comms::option::app::EmptyOption,
    typename TExtraOpts = comms::option::app::EmptyOption
>
class GenericMessage : public
    comms::MessageBase<
        TMessage,
        comms::option::def::FieldsImpl<GenericMessageFields<typename TMessage::Field, TFieldOpts> >,
        comms::option::def::MsgType<GenericMessage<TMessage, TFieldOpts, TExtraOpts> >,
        comms::option::def::HasDoGetId,
        comms::option::def::HasName,
        TExtraOpts
    >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::def::FieldsImpl<GenericMessageFields<typename TMessage::Field, TFieldOpts> >,
            comms::option::def::MsgType<GenericMessage<TMessage, TFieldOpts, TExtraOpts> >,
            comms::option::def::HasDoGetId,
            comms::option::def::HasName,
            TExtraOpts
        >;
public:
    /// @brief Type of the message ID
    /// @details The same as comms::Message::MsgIdType;
    using MsgIdType = typename Base::MsgIdType;

    /// @brief Type of the message ID passed as parameter
    /// @details The same as comms::Message::MsgIdParamType;
    using MsgIdParamType = typename Base::MsgIdParamType;

    /// @brief Default constructor is deleted
    GenericMessage() = delete;

    /// @brief Constructor
    /// @param[in] id ID of the message
    explicit GenericMessage(MsgIdParamType id) : m_id(id) {}

    /// @brief Copy constructor
    GenericMessage(const GenericMessage&) = default;

    /// @brief Move constructor
    GenericMessage(GenericMessage&&) = default;

    /// @brief Destructor
    ~GenericMessage() noexcept = default;

    /// @brief Copy assignment
    GenericMessage& operator=(const GenericMessage&) = default;

    /// @brief Move assignment
    GenericMessage& operator=(GenericMessage&&) = default;

    /// @brief Allow access to internal fields.
    /// @details See definition of @ref COMMS_MSG_FIELDS_ACCESS() macro
    ///     related to @b comms::MessageBase class for details.
    ///
    COMMS_MSG_FIELDS_ACCESS(data);

    /// @brief Get message ID information
    /// @details The comms::MessageBase::getIdImpl() will invoke this
    ///     function.
    MsgIdParamType doGetId() const
    {
        return m_id;
    }

    /// @brief Get message name information.
    /// @details The comms::MessageBase::nameImpl() will invoke this
    ///     function.
    const char* doName() const
    {
        return "Generic Message";
    }

private:
    MsgIdType m_id;
};

} // namespace comms
