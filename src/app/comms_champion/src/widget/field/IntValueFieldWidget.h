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

#include <memory>

#include "comms_champion/FieldWidget.h"
#include "comms_champion/field_wrapper/IntValueWrapper.h"

namespace comms_champion
{

class IntValueFieldWidget : public FieldWidget
{
    typedef FieldWidget Base;
public:
    using WrapperPtr = field_wrapper::IntValueWrapperPtr;

    explicit IntValueFieldWidget(
        WrapperPtr wrapper,
        QWidget* parentObj = nullptr);

    ~IntValueFieldWidget();

protected:
    virtual void refreshImpl() override;
    virtual void editEnabledUpdatedImpl() override;
    virtual void updatePropertiesImpl(const QVariantMap& props) override;

private:
    using WrapperType = typename WrapperPtr::element_type;
    using UnderlyingType = typename WrapperType::UnderlyingType;

    WrapperPtr m_wrapper;
    std::unique_ptr<FieldWidget> m_childWidget;
};


}  // namespace comms_champion


