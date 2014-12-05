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

#include <QtWidgets/QWidget>

class QLineEdit;
class QLabel;

namespace comms_champion
{

class FieldWidget : public QWidget
{
    Q_OBJECT
    typedef QWidget Base;
public:
    FieldWidget(QWidget* parent = nullptr);
    ~FieldWidget() = default;

public slots:
    void refresh();
    void setEditEnabled(bool enabled);
    void propertiesUpdated();

signals:
    void sigFieldUpdated();

protected:
    void emitFieldUpdated();
    bool isEditEnabled() const;
    void updateNameLabel(QLabel& label);

    static void setValidityStyleSheet(QLabel& widget, bool valid);
    static void setValidityStyleSheet(QLineEdit& widget, bool valid);
    static void setSerialisedInputMask(QLineEdit& line, int width);

    template <typename TSerValue>
    static void updateNumericSerialisedValue(
        QLineEdit& line,
        TSerValue value,
        int width)
    {
        updateNumericSerialisedValueInternal(
            line,
            static_cast<unsigned long long>(value),
            width);
    }

    virtual void refreshImpl() = 0;
    virtual void setEditEnabledImpl(bool enabled);
    virtual void propertiesUpdatedImpl();

private:
    static void updateNumericSerialisedValueInternal(
        QLineEdit& line,
        unsigned long long value,
        int width);

    bool m_editEnabled = true;
};

}  // namespace comms_champion




