//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include <string>
#include <cassert>
#include <memory>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtCore/QVariantMap>
#include <QtWidgets/QWidget>
CC_ENABLE_WARNINGS()

#include "PluginControlInterface.h"

namespace comms_champion
{

class Plugin : public QObject
{
public:
    typedef std::unique_ptr<QWidget> WidgetPtr;

    Plugin();
    virtual ~Plugin();

    bool isApplied() const;

    void apply(PluginControlInterface& controlInterface);

    void getCurrentConfig(QVariantMap& config);

    void reconfigure(const QVariantMap& config);

    WidgetPtr getConfigWidget();

protected:
    virtual void applyImpl() = 0;
    virtual void getCurrentConfigImpl(QVariantMap& config);
    virtual void reconfigureImpl(const QVariantMap& config);
    virtual WidgetPtr getConfigWidgetImpl();

    PluginControlInterface& getCtrlInterface();

private:
    PluginControlInterface* m_ctrlInterface = nullptr;
    bool m_applied = false;
};

}  // namespace comms_champion

Q_DECLARE_INTERFACE(comms_champion::Plugin, "cc.Plugin")


