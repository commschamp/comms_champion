//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include <vector>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QVBoxLayout>
CC_ENABLE_WARNINGS()

#include "FieldWidget.h"

namespace comms_champion
{

class BundleFieldWidget : public FieldWidget
{
    Q_OBJECT
    typedef FieldWidget Base;
public:

    explicit BundleFieldWidget(
        QWidget* parentObj = nullptr);

    ~BundleFieldWidget();

    void addMemberField(FieldWidget* memberFieldWidget);

protected:
    virtual void refreshImpl() override;
    virtual void editEnabledUpdatedImpl() override;
    virtual void updatePropertiesImpl(const QVariantMap& props) override;

private slots:
    void memberFieldUpdated();

private:

    QVBoxLayout* m_membersLayout;
    std::vector<FieldWidget*> m_members;
};


}  // namespace comms_champion


