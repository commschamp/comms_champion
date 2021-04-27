//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
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


#include "IntValueFieldWidget.h"

#include <cassert>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QVBoxLayout>
CC_ENABLE_WARNINGS()

#include "comms_champion/property/field.h"

#include "ShortIntValueFieldWidget.h"
#include "LongIntValueFieldWidget.h"
#include "LongLongIntValueFieldWidget.h"
#include "ScaledIntValueFieldWidget.h"

namespace comms_champion
{

IntValueFieldWidget::IntValueFieldWidget(WrapperPtr wrapper, QWidget* parentObj)
  : Base(parentObj),
    m_wrapper(std::move(wrapper))
{
}

IntValueFieldWidget::~IntValueFieldWidget() noexcept
{
    m_childWidget.release();
}

void IntValueFieldWidget::refreshImpl()
{
    assert((!m_wrapper) || (m_wrapper->canWrite()));
    if (m_childWidget) {
        m_childWidget->refresh();
    }
}

void IntValueFieldWidget::editEnabledUpdatedImpl()
{
    if (m_childWidget) {
        m_childWidget->setEditEnabled(isEditEnabled());
    }
}

void IntValueFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    assert(m_wrapper);
    assert(!m_childWidget);
    do {
        if (property::field::IntValue(props).hasScaledDecimals()) {
            m_childWidget.reset(new ScaledIntValueFieldWidget(std::move(m_wrapper)));
            break;
        }

        std::size_t valTypeSize = m_wrapper->valueTypeSize();
        bool isSigned = m_wrapper->isSigned();
        if ((valTypeSize < sizeof(int)) ||
             ((valTypeSize == sizeof(int) && isSigned))) {
            m_childWidget.reset(new ShortIntValueFieldWidget(std::move(m_wrapper)));
            break;
        }

        if (valTypeSize <= sizeof(unsigned)) {
            m_childWidget.reset(new LongIntValueFieldWidget(std::move(m_wrapper)));
            break;
        }

        if ((valTypeSize < sizeof(long long int)) ||
             ((valTypeSize == sizeof(long long int) && isSigned))) {
            m_childWidget.reset(new LongLongIntValueFieldWidget(std::move(m_wrapper)));
            break;
        }

        static constexpr bool The_handling_of_long_long_types_is_not_implemented_yet = false;
        static_cast<void>(The_handling_of_long_long_types_is_not_implemented_yet);
        assert(The_handling_of_long_long_types_is_not_implemented_yet);  
        return;
    } while (false);

    m_childWidget->setNameSuffix(getNameSuffix());
    assert(m_childWidget);
    auto* childLayout = new QVBoxLayout();
    childLayout->addWidget(m_childWidget.get());
    childLayout->setContentsMargins(0, 0, 0, 0);
    childLayout->setSpacing(0);
    setLayout(childLayout);
    m_childWidget->updateProperties(props);
    m_childWidget->setEditEnabled(isEditEnabled());

    connect(
        m_childWidget.get(), SIGNAL(sigFieldUpdated()),
        this, SIGNAL(sigFieldUpdated()));
}

}  // namespace comms_champion

