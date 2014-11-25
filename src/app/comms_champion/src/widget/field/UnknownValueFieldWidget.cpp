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

#include "UnknownValueFieldWidget.h"

#include <algorithm>
#include <cassert>

#include "GlobalConstants.h"

namespace comms_champion
{

UnknownValueFieldWidget::UnknownValueFieldWidget(
    field_wrapper::UnknownValueWrapperPtr&& wrapper,
    QWidget* parent)
  : Base(parent),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);

    connect(m_ui.m_serValueLineEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));

    refresh();
    readPropertiesAndUpdateUi();
}

UnknownValueFieldWidget::~UnknownValueFieldWidget() = default;

void UnknownValueFieldWidget::refreshImpl()
{
    QString serValueStr;
    auto serValue = m_wrapper->serialisedValue();
    for (auto byte : serValue) {
        serValueStr.append(QString("%1").arg(byte, 2, 16, QChar('0')));
    }

    QString altSerValueStr(serValueStr);
    if ((!serValueStr.isEmpty()) &&
        (serValueStr[0] == '0')) {
        altSerValueStr = QString(serValueStr.data() + 1);
    }

    auto curText = m_ui.m_serValueLineEdit->text();
    if ((curText != serValueStr) &&
        (curText != altSerValueStr)) {

        auto maskWidth = static_cast<std::size_t>(m_wrapper->width());
        std::string mask;
        mask.reserve(maskWidth);
        std::fill_n(std::back_inserter(mask), maskWidth + 1, 'H');
        m_ui.m_serValueLineEdit->setInputMask(mask.c_str());
        m_ui.m_serValueLineEdit->setText(serValueStr);
    }

    setFieldValid(m_wrapper->valid());
}

void UnknownValueFieldWidget::setEditEnabledImpl(bool enabled)
{
    bool readonly = !enabled;
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void UnknownValueFieldWidget::propertiesUpdatedImpl()
{
    readPropertiesAndUpdateUi();
}

void UnknownValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    assert(isEditEnabled());

    QString valueCopy(value);
    if ((valueCopy.size() & 0x1) != 0) {
        valueCopy.resize(valueCopy.size() - 1);
    }

    assert((valueCopy.size() & 0x1) == 0);
    auto numOfDigits = valueCopy.size() / 2;

    using SerializedType =
        field_wrapper::UnknownValueWrapperPtr::element_type::SerializedType;
    SerializedType serValue;
    serValue.reserve(numOfDigits);

    static const int Step = 2;
    for (auto i = 0U; i < valueCopy.size(); i += Step) {
        QString byteValueStr(valueCopy.data() + i, Step);
        bool ok = false;
        auto byteValue = byteValueStr.toUInt(&ok, 16);
        assert(ok);
        static_cast<void>(ok);
        serValue.push_back(static_cast<SerializedType::value_type>(byteValue));
    }

    if (m_wrapper->setSerialisedValue(serValue)) {
        refresh();
        emitFieldUpdated();
    }
    else {
        setFieldValid(false);
    }
}

void UnknownValueFieldWidget::readPropertiesAndUpdateUi()
{
    auto nameProperty = property(GlobalConstants::namePropertyName());
    if (nameProperty.isValid()) {
        m_ui.m_nameLabel->setText(nameProperty.toString() + ':');
    }
}

void UnknownValueFieldWidget::setFieldValid(bool valid)
{
    setValidityStyleSheet(*m_ui.m_nameLabel, valid);
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

}  // namespace comms_champion


