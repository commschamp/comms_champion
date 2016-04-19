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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

#include "comms_champion/PluginMgr.h"
#include "comms_champion/MsgMgr.h"
#include "comms_champion/MsgFileMgr.h"
#include "comms_champion/MsgSendMgr.h"

namespace comms_dump
{

class AppMgr // : public QObject
{
    //Q_OBJECT
public:
    struct Config
    {
        QString m_pluginsDir;
        QString m_pluginConfigFile;
        QString m_outMsgsFile;
        QString m_inMsgsFile;
        unsigned m_lastWait;
    };

    AppMgr();
    ~AppMgr();

    bool start(const Config& config);

private:
    typedef comms_champion::PluginMgr::ListOfPluginInfos ListOfPluginInfos;

    bool applyPlugins(const ListOfPluginInfos& plugins);

    comms_champion::PluginMgr m_pluginMgr;
    comms_champion::MsgMgr m_msgMgr;
    comms_champion::MsgFileMgr m_msgFileMgr;
    comms_champion::MsgSendMgr m_msgSendMgr;
    unsigned m_lastWait = 0;
};

} /* namespace comms_dump */
