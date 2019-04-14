//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
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
#include <memory>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtCore/QVariantMap>
#include <QtWidgets/QWidget>
CC_ENABLE_WARNINGS()

#include "Api.h"
#include "PluginProperties.h"

namespace comms_champion
{

/// @brief Interface class for plugin definition
/// @headerfile comms_champion/Plugin.h
class CC_API Plugin : public QObject
{
public:
    /// @brief Pointer to widget object
    using WidgetPtr = std::unique_ptr<QWidget>;

    /// @brief List of filter objects
    using ListOfFilters = PluginProperties::ListOfFilters;

    /// @brief List of GUI action buttons
    using ListOfGuiActions = PluginProperties::ListOfGuiActions;

    /// @brief Constructor
    Plugin();

    /// @brief Destructor
    virtual ~Plugin() noexcept;

    /// @brief Get current configuration.
    /// @details Invokes getCurrentConfigImpl(), which can be overridden by
    ///     the derived class. It is responsible to update the provided
    ///     configuration with new entries relevant to the plugin.
    /// @param[in, out] config Configuration map
    void getCurrentConfig(QVariantMap& config);

    /// @brief Get currentConfiguration.
    /// @details Invokes getCurrentConfigImpl(), which can be overridden by
    ///     the derived class. It is responsible to update the provided
    ///     configuration with new entries relevant to the plugin.
    /// @return Configuration map.
    QVariantMap getCurrentConfig();

    /// @brief Request to perform reconfigure.
    /// @details Invokes reconfigureImpl(), which can be overridden by the
    ///     derived class. The latter must retrieve appropriate values from
    ///     the configuration map and apply requested updates.
    void reconfigure(const QVariantMap& config);

    /// @brief Create socket
    /// @details This function will be called if it is @b socket plugin. It
    ///     will invoke socket creation callback function assigned by the
    ///     derived class to pluginProperties().
    /// @return Allocated socket object
    SocketPtr createSocket() const;

    /// @brief Create filters
    /// @details This function will be called if it is @b filter plugin. It
    ///     will invoke filters creation callback function assigned by the
    ///     derived class to pluginProperties().
    /// @return List of allocated plugin objects
    ListOfFilters createFilters() const;

    /// @brief Create protocol
    /// @details This function will be called if it is @b protocol plugin. It
    ///     will invoke protocol creation callback function assigned by the
    ///     derived class to pluginProperties().
    /// @return Allocated protocol object
    ProtocolPtr createProtocol() const;

    /// @brief Create GUI actions relevant to the plugin.
    /// @details This function will invoke the relevant callback function
    ///     assigned by the derived class to pluginProperties(). The callback
    ///     function is responsible to allocate and return a list of
    ///     @b QAction objects, which will appear to the main toolbar of the
    ///     GUI application. Note, that the main application will @b own the
    ///     allocated @b QAction objects and will delete them later. The plugin
    ///     doesn't need to do it explicityly.
    /// @return List of GUI QAction objects.
    ListOfGuiActions createGuiActions() const;

    /// @brief Create a widget to perform plugin configuration in GUI application.
    /// @details Sometimes there is a need to provide a way to configure the
    ///     plugin. A GUI application will call this function to retrieve a widget
    ///     which will control the configuration. The returned widged is
    ///     expected to be dynamically allocated and will be owned by the
    ///     GUI application, i.e. the plugin doesn't need to perform any
    ///     delete operation on it.
    /// @return Dynamically allocated widget object
    QWidget* createConfiguarionWidget() const;

    /// @brief Retrieve custom property assigned by the derived class.
    /// @param[in] name Name of the property
    /// @return Property value. The value needs to be converted to
    ///     proper type by the caller.
    QVariant getCustomProperty(const QString& name);

protected:
    /// @brief Polymorphic call to retrieve current configuration
    /// @details Default implementation does nothing. The derived class
    ///     needs to override this function if the plugin requires or supports
    ///     configuration and update the provided map with relevant values.
    /// @param[in, out] config Configuration map
    virtual void getCurrentConfigImpl(QVariantMap& config);

    /// @brief Polymorphic call to perform plugin (re)configuration
    /// @details Default implementation does nothing. The derived class
    ///     needs to override byt function if the plugin requires or supports
    ///     (re)configuration.
    /// @param[in] config Configuration map
    virtual void reconfigureImpl(const QVariantMap& config);

    /// @brief Get access to plugin properties
    /// @details Expected to be called by the derived class to get an access
    ///     to the properties accumulation objects and provide appropriate
    ///     callbacks and/or other custom properties.
    /// @return Reference to plugin properties object
    PluginProperties& pluginProperties();

private:
    PluginProperties m_props;
};

}  // namespace comms_champion

Q_DECLARE_INTERFACE(comms_champion::Plugin, "cc.Plugin")


