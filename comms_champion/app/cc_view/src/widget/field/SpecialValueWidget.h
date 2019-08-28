//
// Copyright 2019 (C). Alex Robenko. All rights reserved.
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

    /// @brief The special value is described as string containing name and
    ///     the actual numeric value.
    using IntValueInfo = QPair<QString, long long>;

    /// @brief List of special values
    using IntValueInfosList = QList<IntValueInfo>;


    explicit SpecialValueWidget(
        const IntValueInfosList& infos,
        QWidget* parentObj = nullptr);

    ~SpecialValueWidget() noexcept;

    void setIntValue(long long value);

signals:
    void sigIntValueChanged(long long value);
    void sigRefreshReq();

private slots:
    void itemSelected(int idx);

private:
    Ui::SpecialValueWidget m_ui;
};


}  // namespace comms_champion


