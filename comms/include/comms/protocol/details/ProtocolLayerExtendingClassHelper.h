//
// Copyright 2019 (C). Alex Robenko. All rights reserved.
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

namespace comms
{

namespace protocol
{

namespace details    
{

template <bool THasExtendingClass>
struct ProtocolLayerExtendingClassHelper;

template <>
struct ProtocolLayerExtendingClassHelper<false>
{
    template <typename TLayer, typename TParsedOptions>
    using Type = TLayer;
};

template <>
struct ProtocolLayerExtendingClassHelper<true>
{
    template <typename TLayer, typename TParsedOptions>
    using Type = typename TParsedOptions::ExtendingClass;
};

template <typename TLayer, typename TParsedOptions>
using ProtocolLayerExtendingClassT = 
    typename ProtocolLayerExtendingClassHelper<TParsedOptions::HasExtendingClass>::
        template Type<TLayer, TParsedOptions>;

} // namespace details

} // namespace protocol

} // namespace comms
