//
// Copyright 2019 (C). Alex Robenko. All rights reserved.
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

#include <cstddef>
#include <limits>
#include <type_traits>
#include <iterator>

#include "comms/ErrorStatus.h"
#include "comms/field/tag.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <std::size_t TLenFieldIdx, typename TBase>
class RemLengthMemberField : public TBase
{
    using BaseImpl = TBase;
public:
    using ValueType = typename BaseImpl::ValueType;

    static_assert(TLenFieldIdx < std::tuple_size<ValueType>::value, "Bad index");
    using LengthFieldType = typename std::tuple_element<TLenFieldIdx, ValueType>::type;
    using VersionType = typename BaseImpl::VersionType;

    RemLengthMemberField()
    {
        refreshLengthInternal();
    }

    static constexpr std::size_t maxLength()
    {
        return MaxPossibleLen;
    }

    template <std::size_t TFromIdx>
    static constexpr std::size_t maxLengthFrom()
    {
        using Tag = 
            typename std::conditional<
                TLenFieldIdx < TFromIdx,
                BaseRedirectTag,
                LocalTag
            >::type;
        return maxLengthFromInternal<TFromIdx>(Tag());
    }

    template <std::size_t TUntilIdx>
    static constexpr std::size_t maxLengthUntil()
    {
        using Tag = 
            typename std::conditional<
                TUntilIdx <= TLenFieldIdx,
                BaseRedirectTag,
                LocalTag
            >::type;

        return maxLengthUntilInternal<TUntilIdx>(Tag());
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    static constexpr std::size_t maxLengthFromUntil()
    {
        using Tag = 
            typename std::conditional<
                (TUntilIdx <= TLenFieldIdx) || (TLenFieldIdx < TFromIdx),
                BaseRedirectTag,
                LocalTag
            >::type;

        return maxLengthFromUntilInternal<TFromIdx, TUntilIdx>(Tag());
    }

    bool refresh()
    {
        bool updated = BaseImpl::refresh();
        return refreshLengthInternal() || updated;
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        return readFromUntilInternal<0, std::tuple_size<ValueType>::value>(iter, len, LocalTag());
    }

    template <std::size_t TFromIdx, typename TIter>
    ErrorStatus readFrom(TIter& iter, std::size_t len)
    {
        return readFromAndUpdateLen<TFromIdx>(iter, len);
    }    

    template <std::size_t TFromIdx, typename TIter>
    ErrorStatus readFromAndUpdateLen(TIter& iter, std::size_t& len)
    {
        using Tag = 
            typename std::conditional<
                TLenFieldIdx < TFromIdx,
                BaseRedirectTag,
                LocalTag
            >::type;
        return readFromUntilInternal<TFromIdx, std::tuple_size<ValueType>::value>(iter, len, Tag());
    }       

    template <std::size_t TUntilIdx, typename TIter>
    ErrorStatus readUntil(TIter& iter, std::size_t len)
    {
        return readUntilAndUpdateLen<TUntilIdx>(iter, len);
    }

    template <std::size_t TUntilIdx, typename TIter>
    ErrorStatus readUntilAndUpdateLen(TIter& iter, std::size_t& len)
    {
        using Tag = 
            typename std::conditional<
                TUntilIdx <= TLenFieldIdx,
                BaseRedirectTag,
                LocalTag
            >::type;      
        return readFromUntilInternal<0, TUntilIdx>(iter, len, Tag());
    }   

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    ErrorStatus readFromUntil(TIter& iter, std::size_t len)
    {
        return readFromUntilAndUpdateLen<TFromIdx, TUntilIdx>(iter, len);
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    ErrorStatus readFromUntilAndUpdateLen(TIter& iter, std::size_t& len)
    {
        using Tag = 
            typename std::conditional<
                (TLenFieldIdx < TFromIdx) || (TUntilIdx <= TLenFieldIdx),
                BaseRedirectTag,
                LocalTag
            >::type;      
        return readFromUntilInternal<TFromIdx, TUntilIdx>(iter, len, Tag());
    }      

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    template <std::size_t TFromIdx, typename TIter>
    void readFromNoStatus(TIter& iter) = delete;

    template <std::size_t TUntilIdx, typename TIter>
    void readUntilNoStatus(TIter& iter) = delete;

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    void readFromUntilNoStatus(TIter& iter) = delete;

    static constexpr bool hasNonDefaultRefresh()
    {
        return true;
    }

    bool setVersion(VersionType version)
    {
        bool updated = BaseImpl::setVersion(version);
        return refreshLengthInternal() || updated;
    }

private:
    struct BaseRedirectTag {};
    struct LocalTag {};

    template <std::size_t TFromIdx>
    static constexpr std::size_t maxLengthFromInternal(BaseRedirectTag)
    {
        return BaseImpl::template maxLengthFrom<TFromIdx>();
    }

    template <std::size_t TFromIdx>
    static constexpr std::size_t maxLengthFromInternal(LocalTag)
    {
        return MaxPossibleLen;
    }

    template <std::size_t TUntilIdx>
    static constexpr std::size_t maxLengthUntilInternal(BaseRedirectTag)
    {
        return BaseImpl::template maxLengthUntil<TUntilIdx>();
    }    

    template <std::size_t TUntilIdx>
    static constexpr std::size_t maxLengthUntilInternal(LocalTag)
    {
        return MaxPossibleLen;
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    static constexpr std::size_t maxLengthFromUntilInternal(BaseRedirectTag)
    {
        return BaseImpl::template maxLengthFromUntil<TFromIdx, TUntilIdx>();
    }    

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    static constexpr std::size_t maxLengthFromUntilInternal(LocalTag)
    {
        return MaxPossibleLen;
    }       

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    ErrorStatus readFromUntilInternal(TIter& iter, std::size_t& len, BaseRedirectTag)
    {
        return BaseImpl::template readFromUntilAndUpdateLen<TFromIdx>(iter, len);
    }       

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    ErrorStatus readFromUntilInternal(TIter& iter, std::size_t& len, LocalTag)
    {
        static_assert(TFromIdx <= TLenFieldIdx, "Invalid function invocation");
        static_assert(TLenFieldIdx < TUntilIdx, "Invalid function invocation");

        auto es = BaseImpl::template readFromUntilAndUpdateLen<TFromIdx, TLenFieldIdx>(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto beforeLenReadIter = iter;
        auto& mems = BaseImpl::value();
        auto& lenField = std::get<TLenFieldIdx>(mems);
        es = lenField.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto afterLenReadIter = iter;
        auto lenFieldLen = static_cast<std::size_t>(std::distance(beforeLenReadIter, iter));
        COMMS_ASSERT(lenFieldLen <= len);
        len -= lenFieldLen;

        auto reqLen = static_cast<std::size_t>(lenField.value());
        if (len < reqLen) {
            return comms::ErrorStatus::NotEnoughData;
        }

        es = BaseImpl::template readFromUntil<TLenFieldIdx + 1, TUntilIdx>(iter, reqLen);
        if (es == comms::ErrorStatus::Success) {
            iter = afterLenReadIter;
            std::advance(iter, reqLen);
            len -= reqLen;
        }
        return es;
    }        

    bool refreshLengthInternal()
    {
        auto& mems = BaseImpl::value();
        auto& lenField = std::get<TLenFieldIdx>(mems);
        std::size_t expLen = BaseImpl::template lengthFrom<TLenFieldIdx + 1>();
        std::size_t actLen = static_cast<std::size_t>(lenField.value());
        if (expLen == actLen) {
            return false;
        }

        using LenFieldType = typename std::decay<decltype(lenField)>::type;
        using LenFieldValueType = typename LenFieldType::ValueType;
        lenField.value() = static_cast<LenFieldValueType>(expLen);
        return true;
    }


    static const std::size_t MaxPossibleLen = 0xffff;

    static_assert(std::is_same<typename LengthFieldType::Tag, comms::field::tag::Int>::value,
        "Only IntValue fields are supported as remaining length info inside bundle");

};

}  // namespace adapter

}  // namespace field

}  // namespace comms



