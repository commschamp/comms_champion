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

/// @brief Accumulation of various properties of @ref Plugin.
/// @headerfile comms_champion/PluginProperties.h
class CC_API PluginProperties
{
public:
    /// @brief List of @ref Filter objects
    using ListOfFilters = QList<FilterPtr>;

    /// @brief List of dynamically allocated @b QAction objects
    using ListOfGuiActions = QList<QAction*>;

    /// @brief Constructor
    PluginProperties();

    /// @brief Destructor
    ~PluginProperties() noexcept;

    /// @brief Type of callback to be used to allocate a @ref Socket object
    using SocketCreateFunc = std::function<SocketPtr ()>;

    /// @brief Type of callback to be used to allocate a @ref Protocol object
    using ProtocolCreateFunc = std::function<ProtocolPtr ()>;

    /// @brief Type of callback to be used to allocate a @ref Filter objects
    using FiltersCreateFunc = std::function<ListOfFilters ()>;

    /// @brief Type of callback to be used to allocate @b QAction to be
    ///     displayed in the main toolbar of GUI application.
    /// @details The allocated @b QAction objects will be owned by the
    ///     application. The plugin won't need to delete them explicitly.
    using GuiActionsCreateFunc = std::function<ListOfGuiActions ()>;

    /// @brief Type of callback to be used when a widget responsible to configure
    ///     the plugin needs to be allocated. The allocated widget will be
    ///     owned by the application. The plugin won't need to delete it
    ///     explicitly.
    using ConfigWidgetCreateFunc = std::function<QWidget* ()>;

    /// @brief Assign callback for @ref Socket allocation.
    /// @param[in] func Callback function
    /// @return `*this`
    PluginProperties& setSocketCreateFunc(SocketCreateFunc&& func);

    /// @brief Assign callback for @ref Protocol allocation.
    /// @param[in] func Callback function
    /// @return `*this`
    PluginProperties& setProtocolCreateFunc(ProtocolCreateFunc&& func);

    /// @brief Assign callback for @ref Filter allocation.
    /// @param[in] func Callback function
    /// @return `*this`
    PluginProperties& setFiltersCreateFunc(FiltersCreateFunc&& func);

    /// @brief Assign callback for list of @b QAction allocation.
    /// @param[in] func Callback function
    /// @return `*this`
    PluginProperties& setGuiActionsCreateFunc(GuiActionsCreateFunc&& func);

    /// @brief Assign callback for configuration widget creation.
    /// @param[in] func Callback function
    /// @return `*this`
    PluginProperties& setConfigWidgetCreateFunc(ConfigWidgetCreateFunc&& func);

    /// @brief Set custom property
    /// @param[in] name Name of the property
    /// @param[out] val Property value
    PluginProperties& setCustomProperty(const QString& name, QVariant&& val);

    /// @brief Retrieve @ref Socket creation callback.
    SocketCreateFunc getSocketCreateFunc() const;

    /// @brief Retrieve @ref Protocol creation callback.
    ProtocolCreateFunc getProtocolCreateFunc() const;

    /// @brief Retrieve list of @ref Filter creation callback.
    FiltersCreateFunc getFiltersCreateFunc() const;

    /// @brief Retrieve GUI Actions creation callback.
    GuiActionsCreateFunc getGuiActionsCreateFunc() const;

    /// @brief Retrieve plugin configuration widget creation callback.
    ConfigWidgetCreateFunc getConfigWidgetCreateFunc() const;

    /// @brief Get custom property
    /// @param[in] name Property name
    /// @return Value of the property
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


