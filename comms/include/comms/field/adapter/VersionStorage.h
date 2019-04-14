//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TBase>
class VersionStorage : public TBase
{
    using BaseImpl = TBase;
public:

    using ValueType = typename BaseImpl::ValueType;
    using VersionType = typename BaseImpl::VersionType;

    VersionStorage() = default;

    explicit VersionStorage(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit VersionStorage(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    VersionStorage(const VersionStorage&) = default;
    VersionStorage(VersionStorage&&) = default;
    VersionStorage& operator=(const VersionStorage&) = default;
    VersionStorage& operator=(VersionStorage&&) = default;

    static constexpr bool isVersionDependent()
    {
        return true;
    }

    VersionType getVersion() const
    {
        return m_version;
    }

    bool setVersion(VersionType val)
    {
        bool updated = BaseImpl::setVersion(val);
        if (m_version != val) {
            m_version = val;
            return true;
        }
        return updated;
    }

private:
    VersionType m_version = static_cast<VersionType>(0);
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

