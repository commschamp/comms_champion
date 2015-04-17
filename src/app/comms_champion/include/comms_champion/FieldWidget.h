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

#include <cassert>

#include <QtWidgets/QWidget>

class QLineEdit;
class QLabel;
class QPlainTextEdit;
class QSpinBox;

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
    static void setValidityStyleSheet(QPlainTextEdit& widget, bool valid);
    static void setSerialisedInputMask(QLineEdit& line, int minWidth, int maxWidth);
    static void setSerialisedInputMask(QLineEdit& line, int width);
    static void updateValue(QLineEdit& line, const QString& value);

    template <typename TWrapper>
    void handleNumericSerialisedValueUpdate(
        const QString& value,
        TWrapper& wrapper)
    {
        assert(isEditEnabled());
        do {
            if ((value.size() & 0x1U) == 0) {
               wrapper.setSerialisedString(value);
               break;
            }

            QString valueCpy(value);
            valueCpy.append(QChar('0'));
            wrapper.setSerialisedString(valueCpy);
        } while (false);
        refresh();
        emitFieldUpdated();
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




