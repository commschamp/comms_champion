//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/Socket.h"

namespace comms_champion
{

class DummySocket : public Socket
{
    Q_OBJECT
public:
    DummySocket();

protected:
    virtual bool startImpl() override;
    virtual void stopImpl() override;
    virtual void sendDataImpl(DataToSendPtr dataPtr) override;

private slots:
    void timeout();

private:
    bool m_running = false;
};

inline
SocketPtr makeDummySocket()
{
    return SocketPtr(new DummySocket());
}

}  // namespace comms_champion


