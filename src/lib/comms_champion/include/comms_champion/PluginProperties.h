//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

#include <functional>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QtCore/QList>
CC_ENABLE_WARNINGS()

#include "Socket.h"
#include "Protocol.h"
#include "Filter.h"
#include "Api.h"

class QAction;
class QWidget;

namespace comms_champion
{

class CC_API PluginProperties
{
public:
    typedef QList<FilterPtr> ListOfFilters;
    typedef QList<QAction*> ListOfGuiActions;

    PluginProperties();
    ~PluginProperties();

    typedef std::function<SocketPtr ()> SocketCreateFunc;
    typedef std::function<ProtocolPtr ()> ProtocolCreateFunc;
    typedef std::function<ListOfFilters ()> FiltersCreateFunc;
    typedef std::function<ListOfGuiActions ()> GuiActionsCreateFunc;
    typedef std::function<QWidget* ()> ConfigWidgetCreateFunc;

    PluginProperties& setSocketCreateFunc(SocketCreateFunc&& func);
    PluginProperties& setProtocolCreateFunc(ProtocolCreateFunc&& func);
    PluginProperties& setFiltersCreateFunc(FiltersCreateFunc&& func);
    PluginProperties& setGuiActionsCreateFunc(GuiActionsCreateFunc&& func);
    PluginProperties& setConfigWidgetCreateFunc(ConfigWidgetCreateFunc&& func);
    PluginProperties& setCustomProperty(const QString& name, QVariant&& val);

    SocketCreateFunc getSocketCreateFunc() const;
    ProtocolCreateFunc getProtocolCreateFunc() const;
    FiltersCreateFunc getFiltersCreateFunc() const;
    GuiActionsCreateFunc getGuiActionsCreateFunc() const;
    ConfigWidgetCreateFunc getConfigWidgetCreateFunc() const;
    QVariant getCustomProperty(const QString& name) const;

private:
    QVariantMap m_props;
};

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::PluginProperties::SocketCreateFunc);
Q_DECLARE_METATYPE(comms_champion::PluginProperties::ProtocolCreateFunc);
Q_DECLARE_METATYPE(comms_champion::PluginProperties::FiltersCreateFunc);
Q_DECLARE_METATYPE(comms_champion::PluginProperties::GuiActionsCreateFunc);
Q_DECLARE_METATYPE(comms_champion::PluginProperties::ConfigWidgetCreateFunc);


