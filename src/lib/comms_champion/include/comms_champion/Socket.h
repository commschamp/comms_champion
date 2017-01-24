//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <functional>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

#include "Api.h"
#include "DataInfo.h"

namespace comms_champion
{

class CC_API Socket
{

public:
    enum ConnectionProperty
    {
        ConnectionProperty_Autoconnect = 0x1,
        ConnectionProperty_NonDisconnectable = 0x2
    };
    Socket();
    virtual ~Socket();

    bool start();

    void stop();

    bool socketConnect();

    void socketDisconnect();

    bool isSocketConnected() const;

    void sendData(DataInfoPtr dataPtr);

    typedef std::function<void (DataInfoPtr)> DataReceivedCallback;
    template <typename TFunc>
    void setDataReceivedCallback(TFunc&& func)
    {
        m_dataReceivedCallback = std::forward<TFunc>(func);
    }

    typedef std::function<void (const QString& msg)> ErrorReportCallback;
    template <typename TFunc>
    void setErrorReportCallback(TFunc&& func)
    {
        m_errorReportCallback = std::forward<TFunc>(func);
    }

    typedef std::function <void ()> DisconnectedReportCallback;
    template <typename TFunc>
    void setDisconnectedReportCallback(TFunc&& func)
    {
        m_disconnectedReportCallback = std::forward<TFunc>(func);
    }

    unsigned connectionProperties() const;
protected:

    virtual bool startImpl();
    virtual void stopImpl();
    virtual bool socketConnectImpl();
    virtual void socketDisconnectImpl();
    virtual void sendDataImpl(DataInfoPtr dataPtr) = 0;
    virtual unsigned connectionPropertiesImpl() const;

    void reportDataReceived(DataInfoPtr dataPtr);
    void reportError(const QString& msg);
    void reportDisconnected();

private:
    DataReceivedCallback m_dataReceivedCallback;
    ErrorReportCallback m_errorReportCallback;
    DisconnectedReportCallback m_disconnectedReportCallback;

    bool m_connected = false;
};

typedef std::shared_ptr<Socket> SocketPtr;

}  // namespace comms_champion

