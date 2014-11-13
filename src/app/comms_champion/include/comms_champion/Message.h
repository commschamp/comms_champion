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

#include <QtCore/QObject>

namespace comms_champion
{

class MessageDisplayHandler;
class Message : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)

    typedef QObject Base;
public:
    Message() = default;
    Message(const Message&) = default;
    virtual ~Message();

    Message& operator=(const Message&) = default;

    const char* name() const;
    const char* fieldName(uint idx) const;
    void display(MessageDisplayHandler& handler);

    static void qmlRegister();

protected:

    virtual const char* nameImpl() const = 0;
    virtual const char* fieldNameImpl(uint idx) const = 0;
    virtual void displayImpl(MessageDisplayHandler& handler) = 0;
};

}  // namespace comms_champion
