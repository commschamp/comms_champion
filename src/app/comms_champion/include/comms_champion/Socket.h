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

    typedef DataInfo::DataSeq DataSeq;
    typedef DataInfo::PropertiesMap PropertiesMap;

    struct DataToSend
    {
        DataSeq m_data;
        PropertiesMap m_extraProperties;
    };

    typedef std::shared_ptr<DataToSend> DataToSendPtr;

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

    void sendData(DataToSendPtr dataPtr)
    {
        sendDataImpl(std::move(dataPtr));
    }

signals:
    void sigDataReceived(DataInfoPtr dataPtr);

protected:

    virtual bool startImpl() = 0;
    virtual void stopImpl() = 0;
    virtual void sendDataImpl(DataToSendPtr dataPtr) = 0;

    void reportDataReceived(DataInfoPtr dataPtr)
    {
        emit sigDataReceived(std::move(dataPtr));
    }
};

typedef std::unique_ptr<Socket> SocketPtr;

}  // namespace comms_champion

