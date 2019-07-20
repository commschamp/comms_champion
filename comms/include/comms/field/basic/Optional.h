//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/Field.h"
#include "comms/field/OptionalMode.h"

namespace comms
{

namespace field
{

namespace basic
{

template <typename TField>
class Optional : public
        comms::Field<
            comms::option::def::Endian<typename TField::Endian>,
            comms::option::def::VersionType<typename TField::VersionType>
        >
{
    using BaseImpl =
        comms::Field<
            comms::option::def::Endian<typename TField::Endian>,
            comms::option::def::VersionType<typename TField::VersionType>
        >;

public:

    using Field = TField;
    using ValueType = TField;
    using Mode = field::OptionalMode;
    using VersionType = typename BaseImpl::VersionType;

    Optional() = default;

    explicit Optional(const Field& fieldSrc, Mode mode = Mode::Tentative)
      : field_(fieldSrc),
        mode_(mode)
    {
    }

    explicit Optional(Field&& fieldSrc, Mode mode = Mode::Tentative)
      : field_(std::move(fieldSrc)),
        mode_(mode)
    {
    }

    Optional(const Optional&) = default;

    Optional(Optional&&) = default;

    ~Optional() noexcept = default;

    Optional& operator=(const Optional&) = default;

    Optional& operator=(Optional&&) = default;

    Field& field()
    {
        return field_;
    }

    const Field& field() const
    {
        return field_;
    }

    ValueType& value()
    {
        return field();
    }

    const ValueType& value() const
    {
        return field();
    }

    Mode getMode() const
    {
        return mode_;
    }

    void setMode(Mode val)
    {
        COMMS_ASSERT(val < Mode::NumOfModes);
        mode_ = val;
    }

    std::size_t length() const
    {
        if (mode_ != Mode::Exists) {
            return 0U;
        }

        return field_.length();
    }

    static constexpr std::size_t minLength()
    {
        return 0U;
    }

    static constexpr std::size_t maxLength()
    {
        return Field::maxLength();
    }

    bool valid() const
    {
        if (mode_ == Mode::Missing) {
            return true;
        }

        return field_.valid();
    }

    bool refresh() {
        if (mode_ != Mode::Exists) {
            return false;
        }
        return field_.refresh();
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        if (mode_ == Mode::Missing) {
            return comms::ErrorStatus::Success;
        }

        if ((mode_ == Mode::Tentative) && (0U == len)) {
            mode_ = Mode::Missing;
            return comms::ErrorStatus::Success;
        }

        auto es = field_.read(iter, len);
        if (es == comms::ErrorStatus::Success) {
            mode_ = Mode::Exists;
        }
        return es;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        if (mode_ != Mode::Exists) {
            mode_ = Mode::Missing;
            return;
        }

        mode_ = Mode::Exists;
        field_.readNoStatus(iter);
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        if (mode_ == Mode::Missing) {
            return comms::ErrorStatus::Success;
        }

        if ((mode_ == Mode::Tentative) && (0U == len)) {
            return comms::ErrorStatus::Success;
        }

        return field_.write(iter, len);
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        if (mode_ != Mode::Exists) {
            return;
        }

        field_.writeNoStatus(iter);
    }

    static constexpr bool isVersionDependent()
    {
        return BaseImpl::isVersionDependent() || Field::isVersionDependent();
    }

    static constexpr bool hasNonDefaultRefresh()
    {
        return BaseImpl::hasNonDefaultRefresh() || Field::hasNonDefaultRefresh();
    }

    bool setVersion(VersionType version)
    {
        return field_.setVersion(static_cast<typename Field::VersionType>(version));
    }

private:
    Field field_;
    Mode mode_ = Mode::Tentative;
};

}  // namespace basic

}  // namespace field

}  // namespace comms


