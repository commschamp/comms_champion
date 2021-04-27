//
// Copyright 2016 - 2021 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/MessageHandler.h"

namespace comms_champion
{

MessageHandler::~MessageHandler() noexcept = default;

void MessageHandler::beginMsgHandlingImpl(Message& msg)
{
    static_cast<void>(msg);
}

void MessageHandler::addExtraTransportFieldImpl(FieldWrapperPtr wrapper)
{
    static_cast<void>(wrapper);
}

void MessageHandler::addFieldImpl(FieldWrapperPtr wrapper)
{
    static_cast<void>(wrapper);
}

void MessageHandler::endMsgHandlingImpl()
{
}

}  // namespace comms_champion


