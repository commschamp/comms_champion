//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

#include "comms/options.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename TField, typename... TOptions>
class BitfieldBase;

template <typename TField>
class BitfieldBase<TField> : public TField
{
protected:
    static const bool IndexingFromMsb = false;
};

template <typename TField, typename... TOptions>
class BitfieldBase<TField, comms::option::BitIndexingStartsFromMsb, TOptions...> :
    public BitfieldBase<TField, TOptions...>
{
protected:
    static const bool IndexingFromMsb = true;
};


}  // namespace details

}  // namespace field

}  // namespace comms


