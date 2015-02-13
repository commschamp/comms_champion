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

#include <string>
#include <cassert>

#include <QtCore/QObject>
#include <QtCore/QtPlugin>

#include "PluginControlInterface.h"

namespace comms_champion
{

class Plugin : public QObject
{
public:
    virtual ~Plugin() = default;

    bool isApplied() const
    {
        return m_applied;
    }

    void apply(const PluginControlInterface& controlInterface)
    {
        assert(!isApplied());
        applyImpl(controlInterface);
        m_applied = true;
    }

protected:
    virtual void applyImpl(const PluginControlInterface& controlInterface) = 0;

private:
    bool m_applied = false;
};

}  // namespace comms_champion

Q_DECLARE_INTERFACE(comms_champion::Plugin, "cc.Plugin")


