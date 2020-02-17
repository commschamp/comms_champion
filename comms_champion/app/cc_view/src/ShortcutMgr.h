//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
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

#include <array>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
#include <QtGui/QKeySequence>
#include <QtWidgets/QAction>
CC_ENABLE_WARNINGS()

namespace comms_champion
{

class ShortcutMgr : public QObject
{
    Q_OBJECT
    typedef QObject Base;
public:

    enum Key
    {
        Key_Invalid,
        Key_AddMessage,
        Key_EditMessage,
        Key_Delete,
        Key_DupMessage,
        Key_Up,
        Key_Down,
        Key_Top,
        Key_Bottom,
        Key_Plugins,
        Key_ClearSend,
        Key_ClearRecv,
        Key_Comment,
        Key_Send,
        Key_SendAll,
        Key_AddRaw,
        Key_Connect,
        Key_Disconnect,
        Key_LoadSend,
        Key_LoadRecv,
        Key_SaveSend,
        Key_SaveRecv,
        Key_Receive,
        Key_NumOfValues // Must be last
    };

    ~ShortcutMgr() noexcept;
    
    QKeySequence getKeySeq(Key k) const;
    
    static ShortcutMgr* instance();
    static ShortcutMgr& instanceRef();

    void updateShortcut(QAction& action, ShortcutMgr::Key key);
    
signals:
    void sigKeysUpdated();    
    
private: 
    using KeyMap = std::array<QKeySequence, Key_NumOfValues>;

    ShortcutMgr();  
    
    void createInitialMap();
    
    KeyMap m_map;
};

}  // namespace comms_champion

