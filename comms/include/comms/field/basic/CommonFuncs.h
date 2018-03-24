//
// Copyright 2018 (C). Alex Robenko. All rights reserved.
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
#include <iterator>

#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace basic
{

struct CommonFuncs
{
    template <typename TField, typename TIter>
    static comms::ErrorStatus readSequence(TField& field, TIter& iter, std::size_t len)
    {
        field.clear();
        auto remLen = len;
        while (0 < remLen) {
            auto& elem = field.createBack();
            auto es = field.readElement(elem, iter, remLen);
            if (es != comms::ErrorStatus::Success) {
                field.value().pop_back();
                return es;
            }
        }

        return ErrorStatus::Success;
    }

    template <typename TField, typename TIter>
    static comms::ErrorStatus readSequenceN(TField& field, std::size_t count, TIter& iter, std::size_t& len)
    {
        field.clear();
        while (0 < count) {
            auto& elem = field.createBack();
            auto es = field.readElement(elem, iter, len);
            if (es != comms::ErrorStatus::Success) {
                field.value().pop_back();
                return es;
            }
            --count;
        }
        return comms::ErrorStatus::Success;
    }

    template <typename TField, typename TIter>
    static void readSequenceNoStatusN(TField& field, std::size_t count, TIter& iter)
    {
        field.clear();
        while (0 < count) {
            auto& elem = field.createBack();
            field.readElementNoStatus(elem, iter);
            --count;
        }
    }

    template <typename TField, typename TIter>
    static comms::ErrorStatus writeSequence(const TField& field, TIter& iter, std::size_t len)
    {
        auto es = ErrorStatus::Success;
        auto remainingLen = len;
        for (auto& elem : field.value()) {
            es = field.writeElement(elem, iter, remainingLen);
            if (es != comms::ErrorStatus::Success) {
                break;
            }
        }

        return es;
    }

    template <typename TField, typename TIter>
    static void writeSequenceNoStatus(TField& field, TIter& iter)
    {
        for (auto& elem : field.value()) {
            field.writeElementNoStatus(elem, iter);
        }
    }

    template <typename TField, typename TIter>
    static comms::ErrorStatus writeSequenceN(const TField& field, std::size_t count, TIter& iter, std::size_t& len)
    {
        auto es = ErrorStatus::Success;
        for (auto& elem : field.value()) {
            if (count == 0) {
                break;
            }

            es = field.writeElement(elem, iter, len);
            if (es != ErrorStatus::Success) {
                break;
            }

            --count;
        }

        return es;
    }

    template <typename TField, typename TIter>
    static void writeSequenceNoStatusN(const TField& field, std::size_t count, TIter& iter)
    {
        for (auto& elem : field.value()) {
            if (count == 0) {
                break;
            }

            field.writeElementNoStatus(elem, iter);
            --count;
        }
    }

    template <typename TIter>
    static void advanceWriteIterator(TIter& iter, std::size_t len)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using ByteType = typename std::iterator_traits<IterType>::value_type;
        while (len > 0U) {
            *iter = ByteType();
            ++iter;
            --len;
        }
    }

    static constexpr std::size_t maxSupportedLength()
    {
        return 0xffff;
    }

};

} // namespace basic

} // namespace field

} // namespace comms
