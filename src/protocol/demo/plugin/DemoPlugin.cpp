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


#include "DemoPlugin.h"
#include "Protocol.h"

#include <iostream>

namespace demo
{

namespace plugin
{

void DemoPlugin::initializeImpl()
{
    std::cout << "Plugin initialized!!!" << std::endl;
}

void DemoPlugin::finalizeImpl()
{
    std::cout << "Plugin finalized!!!" << std::endl;
}

void DemoPlugin::configureImpl(const std::string& config)
{
    static_cast<void>(config);
    std::cout << "Plugin configured!!!" << std::endl;
}

cc::ProtocolPtr DemoPlugin::allocImpl()
{
    return cc::ProtocolPtr(new Protocol());
}

}  // namespace plugin

}  // namespace demo

