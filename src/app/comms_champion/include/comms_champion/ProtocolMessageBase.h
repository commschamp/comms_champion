//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

#include <type_traits>
#include <string>
#include <cassert>

namespace comms_champion
{

template <typename TMsgBase, typename TActualMsg>
class ProtocolMessageBase : public TMsgBase
{
    typedef TMsgBase Base;
    typedef TActualMsg ActualMsg;

public:
    ProtocolMessageBase() = default;
    ProtocolMessageBase(const ProtocolMessageBase&) = default;
    ProtocolMessageBase(ProtocolMessageBase&&) = default;
    virtual ~ProtocolMessageBase() = default;

    ProtocolMessageBase& operator=(const ProtocolMessageBase&) = default;
    ProtocolMessageBase& operator=(ProtocolMessageBase&&) = default;

protected:
    virtual QString idAsStringImpl() const override
    {
        auto id = Base::getId();

        typedef typename std::decay<decltype(id)>::type IdType;
        typedef typename std::conditional<
            std::is_enum<IdType>::value,
            EnumTag,
            typename std::conditional<
                std::is_integral<IdType>::value,
                IntTag,
                typename std::conditional<
                    std::is_same<std::string, IdType>::value,
                    StdStringTag,
                    typename std::conditional<
                        std::is_same<const char*, IdType>::value,
                        CStringTag,
                        OtherTag
                    >::type
                >::type
            >::type
        >::type Tag;

        return idAsStringInternal(id, Tag());
    }

    virtual void resetImpl() override
    {
        auto& actObj = static_cast<ActualMsg&>(*this);
        actObj = ActualMsg();
    }

    virtual void assignImpl(const comms_champion::Message& other) override
    {
        auto* castedOther = dynamic_cast<const ActualMsg*>(&other);
        if (castedOther == nullptr) {
            assert(!"Wrong assignment");
            return;
        }
        assert(other.idAsString() == Base::idAsString());
        auto& actObj = static_cast<ActualMsg&>(*this);
        actObj = *castedOther;
    }

private:
    struct EnumTag {};
    struct IntTag {};
    struct StdStringTag {};
    struct CStringTag {};
    struct OtherTag {};

    template <typename TId>
    static QString idAsStringInternal(TId&& id, EnumTag)
    {
        typedef typename std::decay<decltype(id)>::type IdType;
        typedef typename std::underlying_type<IdType>::type UnderlyingType;
        return QString("%1").arg((UnderlyingType)id, 1, 10, QChar('0'));
    }

    template <typename TId>
    static QString idAsStringInternal(TId&& id, IntTag)
    {
        return QString("%1").arg(id, 1, 10, QChar('0'));
    }

    template <typename TId>
    static QString idAsStringInternal(TId&& id, StdStringTag)
    {
        return QString::fromStdString(id);
    }

    template <typename TId>
    QString idAsStringInternal(TId&& id, CStringTag)
    {
        return QString(id);
    }

    template <typename TId>
    QString idAsStringInternal(TId&& id, OtherTag)
    {
        return QString();
    }
};

}  // namespace comms_champion


