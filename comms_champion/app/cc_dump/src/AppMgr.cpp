//
// Copyright 2016 - 2020 (C). Alex Robenko. All rights reserved.
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

#include "AppMgr.h"

#include <iostream>
#include <type_traits>
#include <string>

CC_DISABLE_WARNINGS()
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
CC_ENABLE_WARNINGS()

#include "comms_champion/property/message.h"

namespace cc = comms_champion;

namespace comms_dump
{

namespace
{

const std::string Sep(", ");
const int FlushInterval = 1000;

}  // namespace

AppMgr::AppMgr()
{
    m_msgMgr.setMsgAddedCallbackFunc(
        [this](cc::MessagePtr msg)
        {
            if (!msg) {
                static constexpr bool Application_message_was_not_provided = false;
                static_cast<void>(Application_message_was_not_provided);
                assert(Application_message_was_not_provided);
                return;
            }

            auto type = cc::property::message::Type().getFrom(*msg);
            assert((type == cc::Message::Type::Sent) ||
                   (type == cc::Message::Type::Received));
            if ((type == cc::Message::Type::Sent) &&
                (!m_config.m_recordOutgoing)) {
                return;
            }

            dispatchMsg(*msg);
        });

    m_msgSendMgr.setSendMsgsCallbackFunc(
        [this](cc::MsgSendMgr::MessagesList&& msgs)
        {
            m_msgMgr.sendMsgs(std::move(msgs));
        });

    m_msgSendMgr.setSendCompeteCallbackFunc(
        [this]()
        {
            if (m_config.m_lastWait == 0U) {
                return;
            }

            QTimer::singleShot(static_cast<int>(m_config.m_lastWait), qApp, SLOT(quit()));
        });

    connect(
        &m_flushTimer, SIGNAL(timeout()),
        this, SLOT(flushOutput()));
}

AppMgr::~AppMgr() noexcept = default;

bool AppMgr::start(const Config& config)
{
    if (config.m_pluginsDir.isEmpty()) {
        std::cerr << "ERROR: Unknown plugins directory!" << std::endl;
        return false;
    }
    m_pluginMgr.setPluginsDir(config.m_pluginsDir);

    if (config.m_pluginConfigFile.isEmpty()) {
        std::cerr << "ERROR: The plugins configuration file wasn't provided" << std::endl;
        return false;
    }
    auto plugins = m_pluginMgr.loadPluginsFromConfigFile(config.m_pluginConfigFile);
    if (plugins.empty()) {
        std::cerr << "ERROR: No plugins were loaded" << std::endl;
        return false;
    }

    if (!applyPlugins(plugins)) {
        std::cerr << "ERROR: Failed to apply plugins" << std::endl;
        return false;
    }

    m_config = config;
    if (!m_config.m_quiet) {
        m_csvDump.reset(new CsvDumpMessageHandler(std::cout, Sep));

        if (m_config.m_recordOutgoing) {
            m_csvDump->setShowType(m_config.m_recordOutgoing);
        }
    }

    if (!m_config.m_inMsgsFile.isEmpty()) {
        m_record.reset(new RecordMessageHandler(m_config.m_inMsgsFile));
    }

    m_msgMgr.setRecvEnabled(true);
    m_msgMgr.start();

    auto socket = m_msgMgr.getSocket();
    if (!socket) {
        std::cerr << "ERROR: Socket plugin hasn't been chosen or doesn't exist" << std::endl;
        return false;
    }

    if (!socket->socketConnect()) {
        std::cerr << "WARNING: Socket failed to connect!" << std::endl;
    }

    if (!config.m_outMsgsFile.isEmpty()) {
        auto protocol = m_msgMgr.getProtocol();
        assert(protocol);

        auto msgsToSend =
            m_msgFileMgr.load(
                cc::MsgFileMgr::Type::Send,
                config.m_outMsgsFile,
                *protocol);

        if (!msgsToSend.empty()) {
            m_msgSendMgr.start(protocol, msgsToSend);
        }
    }
    else if (0 < m_config.m_lastWait) {
        QTimer::singleShot(static_cast<int>(m_config.m_lastWait), qApp, SLOT(quit()));
    }

    m_flushTimer.start(FlushInterval);
    return true;
}

void AppMgr::flushOutput()
{
    if (m_csvDump) {
        m_csvDump->flush();
    }

    if (m_record) {
        m_record->flush();
    }
}

bool AppMgr::applyPlugins(const ListOfPluginInfos& plugins)
{
    typedef cc::Plugin::ListOfFilters ListOfFilters;

    struct ApplyInfo
    {
        cc::SocketPtr m_socket;
        ListOfFilters m_filters;
        cc::ProtocolPtr m_protocol;
    };

    auto applyInfo = ApplyInfo();
    for (auto& info : plugins) {
        cc::Plugin* plugin = m_pluginMgr.loadPlugin(*info);
        if (plugin == nullptr) {
            static constexpr bool Failed_to_load_plugin = false;
            static_cast<void>(Failed_to_load_plugin);
            assert(Failed_to_load_plugin);
            continue;
        }

        if (!applyInfo.m_socket) {
            applyInfo.m_socket = plugin->createSocket();
        }

        applyInfo.m_filters.append(plugin->createFilters());

        if (!applyInfo.m_protocol) {
            applyInfo.m_protocol = plugin->createProtocol();
        }
    }

    if (!applyInfo.m_socket) {
        std::cerr << "ERROR: Socket hasn't been set!" << std::endl;
        return false;
    }

    if (!applyInfo.m_protocol) {
        std::cerr << "ERROR: Protocol hasn't been set!" << std::endl;
        return false;
    }

    m_msgMgr.setSocket(std::move(applyInfo.m_socket));

    for (auto& filter : applyInfo.m_filters) {
        m_msgMgr.addFilter(std::move(filter));
    }

    m_msgMgr.setProtocol(std::move(applyInfo.m_protocol));

    m_pluginMgr.setAppliedPlugins(plugins);
    return true;
}

void AppMgr::dispatchMsg(comms_champion::Message& msg)
{
    if (m_csvDump) {
        msg.dispatch(*m_csvDump);
    }

    if (m_record) {
        msg.dispatch(*m_record);
    }
}

} /* namespace comms_dump */
