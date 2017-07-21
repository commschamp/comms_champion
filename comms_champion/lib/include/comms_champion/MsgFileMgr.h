//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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
#include <memory>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
#include <QtCore/QVariantList>
#include <QtCore/QFile>
CC_ENABLE_WARNINGS()

#include "Api.h"
#include "Message.h"
#include "Protocol.h"

namespace comms_champion
{

class CC_API MsgFileMgr
{
public:

    typedef Protocol::MessagesList MessagesList;

    enum class Type
    {
        Recv,
        Send
    };

    MsgFileMgr();
    ~MsgFileMgr() noexcept;
    MsgFileMgr(const MsgFileMgr&);
    MsgFileMgr(MsgFileMgr&&);

    MsgFileMgr& operator=(const MsgFileMgr&);
    MsgFileMgr& operator=(MsgFileMgr&&);


    const QString& getLastFile() const;
    static const QString& getFilesFilter();

    MessagesList load(Type type, const QString& filename, Protocol& protocol);
    bool save(Type type, const QString& filename, const MessagesList& msgs);

    typedef std::shared_ptr<QFile> FileSaveHandler;
    static FileSaveHandler startRecvSave(const QString& filename);
    static void addToRecvSave(FileSaveHandler handler, const Message& msg, bool flush = false);
    static void flushRecvFile(FileSaveHandler handler);

private:
    QString m_lastFile;
};

}  // namespace comms_champion


