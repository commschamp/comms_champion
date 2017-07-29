//
// Copyright 2014 - 2017 (C). Alex Robenko. All rights reserved.
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

#include "DefaultMessageDisplayHandler.h"

#include <cassert>

#include <QtWidgets/QApplication>

#include "comms_champion/field_wrapper/FieldWrapperHandler.h"
#include "widget/field/IntValueFieldWidget.h"
#include "widget/field/UnsignedLongLongIntValueFieldWidget.h"
#include "widget/field/BitmaskValueFieldWidget.h"
#include "widget/field/EnumValueFieldWidget.h"
#include "widget/field/StringFieldWidget.h"
#include "widget/field/BitfieldFieldWidget.h"
#include "widget/field/OptionalFieldWidget.h"
#include "widget/field/BundleFieldWidget.h"
#include "widget/field/ArrayListRawDataFieldWidget.h"
#include "widget/field/ArrayListFieldWidget.h"
#include "widget/field/FloatValueFieldWidget.h"
#include "widget/field/VariantFieldWidget.h"
#include "widget/field/UnknownValueFieldWidget.h"

namespace comms_champion
{

namespace
{

class WidgetCreator : public field_wrapper::FieldWrapperHandler
{
public:
    typedef field_wrapper::FieldWrapperPtr FieldWrapperPtr;

    WidgetCreator()
    {
        auto widgets = qApp->topLevelWidgets();
        if (!widgets.isEmpty()) {
            m_parent = widgets[0];
        }
    }

    virtual void handle(field_wrapper::IntValueWrapper& wrapper) override
    {
        m_widget.reset(new IntValueFieldWidget(wrapper.clone(), m_parent));
    }

    virtual void handle(field_wrapper::UnsignedLongValueWrapper& wrapper) override
    {
        m_widget.reset(new UnsignedLongLongIntValueFieldWidget(wrapper.clone(), m_parent));
    }

    virtual void handle(field_wrapper::BitmaskValueWrapper& wrapper) override
    {
        m_widget.reset(new BitmaskValueFieldWidget(wrapper.clone(), m_parent));
    }

    virtual void handle(field_wrapper::EnumValueWrapper& wrapper) override
    {
        m_widget.reset(new EnumValueFieldWidget(wrapper.clone(), m_parent));
    }

    virtual void handle(field_wrapper::StringWrapper& wrapper) override
    {
        m_widget.reset(new StringFieldWidget(wrapper.clone(), m_parent));
    }

    virtual void handle(field_wrapper::BitfieldWrapper& wrapper) override
    {
        auto& membersWrappers = wrapper.getMembers();
        std::vector<FieldWidgetPtr> membersWidgets;
        membersWidgets.reserve(membersWrappers.size());
        for (auto& mem : membersWrappers) {
            mem->dispatch(*this);
            membersWidgets.push_back(getWidget());
        }

        std::unique_ptr<BitfieldFieldWidget> widget(new BitfieldFieldWidget(wrapper.clone(), m_parent));
        for (auto& memWidget : membersWidgets) {
            widget->addMemberField(memWidget.release());
        }

        m_widget = std::move(widget);
    }

    virtual void handle(field_wrapper::OptionalWrapper& wrapper) override
    {
        wrapper.getFieldWrapper().dispatch(*this);
        auto wrappedWidget = getWidget();

        std::unique_ptr<OptionalFieldWidget> widget(new OptionalFieldWidget(wrapper.clone(), m_parent));
        widget->setField(wrappedWidget.release());
        m_widget = std::move(widget);
    }

    virtual void handle(field_wrapper::BundleWrapper& wrapper) override
    {
        auto& membersWrappers = wrapper.getMembers();
        std::vector<FieldWidgetPtr> membersWidgets;
        membersWidgets.reserve(membersWrappers.size());
        for (auto& mem : membersWrappers) {
            mem->dispatch(*this);
            membersWidgets.push_back(getWidget());
        }

        std::unique_ptr<BundleFieldWidget> widget(new BundleFieldWidget(m_parent));
        for (auto& memWidget : membersWidgets) {
            widget->addMemberField(memWidget.release());
        }

        m_widget = std::move(widget);
    }

    virtual void handle(field_wrapper::ArrayListRawDataWrapper& wrapper) override
    {
        m_widget.reset(new ArrayListRawDataFieldWidget(wrapper.clone(), m_parent));
    }

    virtual void handle(field_wrapper::ArrayListWrapper& wrapper) override
    {
        auto createMembersWidgetsFunc =
            [](field_wrapper::ArrayListWrapper& wrap) -> std::vector<FieldWidgetPtr>
            {
                std::vector<FieldWidgetPtr> allFieldsWidgets;
                WidgetCreator otherCreator;
                auto& memWrappers = wrap.getMembers();
                allFieldsWidgets.reserve(memWrappers.size());
                assert(memWrappers.size() == wrap.size());

                for (auto& memWrap : memWrappers) {
                    memWrap->dispatch(otherCreator);
                    allFieldsWidgets.push_back(otherCreator.getWidget());
                }

                assert(allFieldsWidgets.size() == wrap.size());
                return allFieldsWidgets;
            };

        assert(wrapper.size() == wrapper.getMembers().size());
        m_widget.reset(new ArrayListFieldWidget(wrapper.clone(), std::move(createMembersWidgetsFunc), m_parent));
    }

    virtual void handle(field_wrapper::FloatValueWrapper& wrapper) override
    {
        m_widget.reset(new FloatValueFieldWidget(wrapper.clone(), m_parent));
    }

    virtual void handle(field_wrapper::VariantWrapper& wrapper) override
    {
        auto createMemberWidgetsFunc =
            [](field_wrapper::FieldWrapper& wrap) -> FieldWidgetPtr
            {
                WidgetCreator otherCreator;
                wrap.dispatch(otherCreator);
                return otherCreator.getWidget();
            };

        FieldWidgetPtr memberWidget;
        auto& memberWrapper = wrapper.getCurrent();
        if (memberWrapper) {
            memberWrapper->dispatch(*this);
            memberWidget = getWidget();
        }

        std::unique_ptr<VariantFieldWidget> widget(
                    new VariantFieldWidget(
                        wrapper.clone(),
                        createMemberWidgetsFunc,
                        m_parent));
        if (memberWidget) {
            widget->setMemberField(memberWidget.release());
        }

        m_widget = std::move(widget);
    }

    virtual void handle(field_wrapper::UnknownValueWrapper& wrapper) override
    {
        m_widget.reset(new UnknownValueFieldWidget(wrapper.clone(), m_parent));
    }

    virtual void handle(field_wrapper::FieldWrapper& wrapper) override
    {
        static_cast<void>(wrapper);
        assert(!"Unexpected wrapper");
    }

    FieldWidgetPtr getWidget()
    {
        return std::move(m_widget);
    }


private:
    FieldWidgetPtr m_widget;
    QWidget* m_parent = nullptr;
};



}  // namespace

DefaultMessageDisplayHandler::~DefaultMessageDisplayHandler() noexcept = default;

DefaultMessageDisplayHandler::MsgWidgetPtr DefaultMessageDisplayHandler::getMsgWidget()
{
    return std::move(m_widget);
}

void DefaultMessageDisplayHandler::beginMsgHandlingImpl(
    Message& msg)
{
    m_widget.reset(new DefaultMessageWidget(msg));
}

void DefaultMessageDisplayHandler::addFieldImpl(FieldWrapperPtr wrapper)
{
    assert(m_widget);
    WidgetCreator creator;
    wrapper->dispatch(creator);
    auto fieldWidget = creator.getWidget();
    fieldWidget->hide();
    m_widget->addFieldWidget(fieldWidget.release());
}

}  // namespace comms_champion


