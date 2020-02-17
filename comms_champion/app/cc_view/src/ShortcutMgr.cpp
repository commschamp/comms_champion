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

#include "ShortcutMgr.h"

#include <type_traits>


namespace comms_champion
{

ShortcutMgr::~ShortcutMgr() noexcept = default;

QKeySequence ShortcutMgr::getKeySeq(Key k) const
{
    if ((Key_NumOfValues <= k) || (m_map[k].isEmpty())) {
        return QKeySequence();
    }

    return m_map[k];
}

ShortcutMgr* ShortcutMgr::instance()
{
    return &instanceRef();
}

ShortcutMgr& ShortcutMgr::instanceRef()
{
    static ShortcutMgr Mgr;
    return Mgr;
}

void ShortcutMgr::updateShortcut(QAction& action, ShortcutMgr::Key key)
{
    auto keySeq = getKeySeq(key);
    if (keySeq.isEmpty()) {
        return;
    }

    auto text = action.text();
    static const QString ShotrcutStart(" [");
    auto shortcutPos = text.indexOf(ShotrcutStart);
    if (0 <= shortcutPos) {
        text = text.left(shortcutPos);
    }

    action.setText(text + ShotrcutStart + keySeq.toString(QKeySequence::NativeText) + ']');
    action.setShortcut(keySeq);
}

ShortcutMgr::ShortcutMgr()
{
    createInitialMap();
}

void ShortcutMgr::createInitialMap()
{
    static const char* Keys[] = {
        /* Key_Invalid */ nullptr,
        /* Key_AddMessage */ "Ctrl+N",
        /* Key_EditMessage */ "Ctrl+E",
        /* Key_Delete */ "Delete",
        /* Key_DupMessage */ "Ctrl+D",
        /* Key_Up */ "Ctrl+Up",
        /* Key_Down */ "Ctrl+Down",
        /* Key_Top */ "Ctrl+Shift+Up",
        /* Key_Bottom */ "Ctrl+Shift+Down",
        /* Key_Plugins */ "Ctrl+P",
        /* Key_ClearSend */ "Ctrl+L,S",
        /* Key_ClearRecv */ "Ctrl+L,R",
        /* Key_Comment */ "Ctrl+M",
        /* Key_Send */ "Ctrl+S",
        /* Key_SendAll */ "Ctrl+Shift+Alt+S",
        /* Key_AddRaw */ "Ctrl+R",
        /* Key_Connect */ "Ctrl+K",
        /* Key_Disconnect */ "Ctrl+Alt+K",
        /* Key_LoadSend */ "Ctrl+O,S",
        /* Key_LoadRecv */ "Ctrl+O,R",
        /* Key_SaveSend */ "Ctrl+V,S",
        /* Key_SaveRecv */ "Ctrl+V,R",
        /* Key_Receive */ "F5",
    };

    static const std::size_t KeysSize = std::extent<decltype(Keys)>::value;
    static_assert(KeysSize == Key_NumOfValues, "Invalid map");

    for (auto i = 0U; i < KeysSize; ++i) {
        if (Keys[i] == nullptr) {
            continue;
        }

        m_map[i] = QKeySequence(Keys[i]);
    }
}


} // namespace comms_champion
