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

#include <QWidget>
#include <QtCore/QList>
#include <QtCore/QPair>

#include "ui_SpecialValueWidget.h"

namespace comms_champion
{

class SpecialValueWidget : public QWidget
{
    Q_OBJECT
    typedef QWidget Base;
public:

    using IntValueInfo = QPair<QString, long long>;
    using IntValueInfosList = QList<IntValueInfo>;

    using FpValueInfo = QPair<QString, double>;
    using FpValueInfosList = QList<FpValueInfo>;

    explicit SpecialValueWidget(
        const IntValueInfosList& infos,
        QWidget* parentObj = nullptr);

    explicit SpecialValueWidget(
        const FpValueInfosList& infos,
        QWidget* parentObj = nullptr);


    ~SpecialValueWidget() noexcept;

    void setIntValue(long long value);
    void setFpValue(double value, double cmpEpsilon);

signals:
    void sigIntValueChanged(long long value);
    void sigFpValueChanged(double value);
    void sigRefreshReq();

private slots:
    void itemSelected(int idx);

private:
    template <typename T>
    void commonSetup(const T& infos);

    Ui::SpecialValueWidget m_ui;
    bool m_fpSpecials = false;
};


}  // namespace comms_champion


