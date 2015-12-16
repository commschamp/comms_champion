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

#include <utility>
#include <list>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
#include <QtCore/QVariantList>
CC_ENABLE_WARNINGS()

#include "comms_champion/MessageInfo.h"
#include "comms_champion/Protocol.h"

namespace comms_champion
{

class MsgFileMgr
{
public:

    enum class Type
    {
        Recv,
        Send
    };

    static MsgFileMgr* instance();
    static MsgFileMgr& instanceRef();

    const QString& getLastFile() const;
    static const QString& getFilesFilter();

    MsgInfosList load(Type type, const QString& filename, Protocol& protocol);
    bool save(Type type, const QString& filename, const MsgInfosList& msgs);

private:
    MsgFileMgr() = default;
    static QVariantList convertMsgList(Type type, const MsgInfosList& msgs);
    static MsgInfosList convertMsgList(Type type, const QVariantList& msgs, Protocol& protocol);

    QString m_lastFile;
};

}  // namespace comms_champion


