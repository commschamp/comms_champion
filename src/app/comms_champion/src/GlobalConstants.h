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

#include <QtCore/QString>
#include <QtCore/QObject>

namespace comms_champion
{

class GlobalConstants : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString background READ background CONSTANT)
    Q_PROPERTY(QString waitingState READ waitingState CONSTANT)
    Q_PROPERTY(QString runningState READ runningState CONSTANT)
    Q_PROPERTY(QString sendingState READ sendingState CONSTANT)
    Q_PROPERTY(QString sendingAllState READ sendingAllState CONSTANT)
    Q_PROPERTY(QString startIconPathFromQml READ startIconPathFromQml CONSTANT)
    Q_PROPERTY(QString startAllIconPathFromQml READ startAllIconPathFromQml CONSTANT)
    Q_PROPERTY(QString stopIconPathFromQml READ stopIconPathFromQml CONSTANT)
    Q_PROPERTY(QString saveIconPathFromQml READ saveIconPathFromQml CONSTANT)
    Q_PROPERTY(QString configIconPathFromQml READ configIconPathFromQml CONSTANT)
    Q_PROPERTY(const char* indexPropertyName READ indexPropertyName CONSTANT)
    Q_PROPERTY(const char* namePropertyName READ namePropertyName CONSTANT)

    typedef QObject Base;
public:

    enum class State
    {
        Waiting,
        Running,
        Sending,
        SendingAll,
        NumOfStates // Must be last
    };

    static GlobalConstants* instance();
    static void qmlRegister();

    static const QString& background();

    static const QString& mapState(State value);
    static const QString& waitingState();
    static const QString& runningState();
    static const QString& sendingState();
    static const QString& sendingAllState();

    static const QString& startIconPathFromQml();
    static const QString& startAllIconPathFromQml();
    static const QString& stopIconPathFromQml();
    static const QString& saveIconPathFromQml();
    static const QString& configIconPathFromQml();

    static const char* indexPropertyName();
    static const char* namePropertyName();
    static const char* msgNumberPropertyName();
    static const char* msgObjPropertyName();

private:
    GlobalConstants(QObject* parent = nullptr);
};

} // namespace comms_champion


