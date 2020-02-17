//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
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

#include <list>

#include "comms/CompileControl.h"
CC_DISABLE_WARNINGS()
#include <QtCore/QTimer>
CC_ENABLE_WARNINGS()


#include "comms_champion/Socket.h"

namespace comms_champion
{

class EchoSocket : public QObject, public Socket
{
    Q_OBJECT

public:
    EchoSocket();
    ~EchoSocket() noexcept;

protected:
    virtual void sendDataImpl(DataInfoPtr dataPtr) override;
    virtual unsigned connectionPropertiesImpl() const override;

private slots:
    void sendDataPostponed();

private:
    QTimer m_timer;
    std::list<DataInfoPtr> m_pendingData;
    bool m_timerActive = false;
};

inline
SocketPtr makeEchoSocket()
{
    return SocketPtr(new EchoSocket());
}

}  // namespace comms_champion


