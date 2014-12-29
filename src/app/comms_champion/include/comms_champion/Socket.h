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

#include <cstdint>
#include <cstddef>
#include <vector>

#include <QtCore/QObject>

#include "DataInfo.h"

namespace comms_champion
{

class Socket : public QObject
{
    Q_OBJECT

public:
    Socket() = default;
    virtual ~Socket() {}

    bool start()
    {
        return startImpl();
    }

    void stop()
    {
        stopImpl();
    }

public slots:

    void sendData(DataInfoPtr dataPtr)
    {
        sendDataImpl(std::move(dataPtr));
    }


    void feedInData(DataInfoPtr dataPtr)
    {
        feedInDataImpl(std::move(dataPtr));
    }

signals:
    void sigDataReceived(DataInfoPtr dataPtr);
    void sigDataToSend(DataInfoPtr dataPtr);

protected:

    virtual bool startImpl() = 0;
    virtual void stopImpl() = 0;
    virtual void sendDataImpl(DataInfoPtr dataPtr) = 0;
    virtual void feedInDataImpl(DataInfoPtr dataPtr) = 0;

    void reportDataReceived(DataInfoPtr dataPtr)
    {
        emit sigDataReceived(std::move(dataPtr));
    }

    void reportDataToSend(DataInfoPtr dataPtr)
    {
        emit sigDataToSend(std::move(dataPtr));
    }
};

typedef std::unique_ptr<Socket> SocketPtr;

}  // namespace comms_champion

