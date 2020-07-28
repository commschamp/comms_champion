//
// Copyright 2013 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstdint>
#include <algorithm>
#include <limits>
#include <memory>
#include <iterator>
#include <type_traits>

#include "comms/comms.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

namespace
{

typedef comms::option::BigEndian BigEndianOpt;

template <typename TField>
TField readWriteField(
    const char* buf,
    std::size_t size,
    comms::ErrorStatus expectedStatus = comms::ErrorStatus::Success)
{
    typedef TField Field;
    Field field;

    auto iter = buf;
    auto status = field.read(iter, size);
    REQUIRE(status == expectedStatus);

    if (status != comms::ErrorStatus::Success) {
        return field;
    }

    auto diff = static_cast<std::size_t>(std::distance(buf, iter));
    REQUIRE(field.length() == diff);

    std::unique_ptr<char[]> outDataBuf(new char[diff]);
    auto writeIter = &outDataBuf[0];

    status = field.write(writeIter, diff);
    REQUIRE(status == comms::ErrorStatus::Success);
    bool bufAsExpected = std::equal(buf, buf + diff, static_cast<const char*>(&outDataBuf[0]));
    if (!bufAsExpected) {
        std::cout << "Expected buffer: " << std::hex;
        std::copy_n(buf, diff, std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << "\nActual buffer: ";
        std::copy_n(&outDataBuf[0], diff, std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << std::dec << std::endl;
    }
    REQUIRE(bufAsExpected);

    auto writeDiff = static_cast<std::size_t>(std::distance(&outDataBuf[0], writeIter));
    REQUIRE(field.length() == writeDiff);
    REQUIRE(diff == writeDiff);
    return field;
}

template <typename TField>
void writeField(
    const TField& field,
    const char* expectedBuf,
    std::size_t size,
    comms::ErrorStatus expectedStatus = comms::ErrorStatus::Success)
{
    std::unique_ptr<char[]> outDataBuf(new char[size]);
    auto writeIter = &outDataBuf[0];
    auto es = field.write(writeIter, size);
    REQUIRE(es, expectedStatus);
    bool bufAsExpected = std::equal(expectedBuf, expectedBuf + size, static_cast<const char*>(&outDataBuf[0]));
    if (!bufAsExpected) {
        std::cout << "Expected buffer: " << std::hex;
        std::copy_n(expectedBuf, size, std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << "\nActual buffer: ";
        std::copy_n(&outDataBuf[0], size, std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << std::dec << std::endl;
    }
    REQUIRE(bufAsExpected);
}

} // namespace

TEST_CASE("Test1", "test1") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t> Field;

    static_assert(!Field::isVersionDependent(), "Invalid version dependency assumption");
    static const char Buf[] = {
        0x01, 0x02, 0x03, 0x04
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == sizeof(std::uint32_t));
    REQUIRE(field.value() == 0x01020304);
    REQUIRE(field.valid());
    REQUIRE(!field.setVersion(5));
}

TEST_CASE("Test2", "test2") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::FixedLength<3>
    > Field;

    static_assert(!Field::isVersionDependent(), "Invalid version dependency assumption");

    static const char Buf[] = {
        0x01, 0x02, 0x03, 0x04
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 3);
    REQUIRE(field.value() == 0x010203);
    REQUIRE(field.valid());
}
