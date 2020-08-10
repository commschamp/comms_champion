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
typedef comms::option::LittleEndian LittleEndianOpt;

enum Enum1 {
    Enum1_Value1,
    Enum1_Value2,
    Enum1_Value3,
    Enum1_NumOfValues
};

enum class Enum2 : unsigned {
    Value1,
    Value2,
    Value3,
    Value4,
    NumOfValues
};

struct HelloInitialiser
{
    template <typename TField>
    void operator()(TField&& field)
    {
        field.value() = "hello";
    }
};

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
    REQUIRE(es == expectedStatus);
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

template <typename TField>
void writeReadField(
    const TField& field,
    const char* expectedBuf,
    std::size_t size,
    comms::ErrorStatus expectedStatus = comms::ErrorStatus::Success)
{
    std::unique_ptr<char[]> outDataBuf(new char[size]);
    auto writeIter = &outDataBuf[0];
    auto es = field.write(writeIter, size);
    REQUIRE(es == expectedStatus);
    bool bufAsExpected = std::equal(expectedBuf, expectedBuf + size, static_cast<const char*>(&outDataBuf[0]));
    if (!bufAsExpected) {
        std::cout << "Expected buffer: " << std::hex;
        std::copy_n(expectedBuf, size, std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << "\nActual buffer: ";
        std::copy_n(&outDataBuf[0], size, std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << std::dec << std::endl;
    }
    REQUIRE(bufAsExpected);

    if (es == comms::ErrorStatus::Success) {
        auto readIter = &outDataBuf[0];
        typename std::decay<decltype(field)>::type newField;
        auto readEs = newField.read(readIter, size);
        REQUIRE(readEs == comms::ErrorStatus::Success);
        REQUIRE(field == newField);
        REQUIRE(field.value() == newField.value());
    }
}

template <typename TFP>
bool fpEquals(TFP value1, TFP value2)
{
    return (std::abs(value1 - value2) <= std::numeric_limits<TFP>::epsilon());
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

TEST_CASE("Test3", "test3") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::int16_t
    > Field;

    static_assert(!Field::isVersionDependent(), "Invalid version dependency assumption");

    static const char Buf[] = {
        0x01, 0x02
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == sizeof(std::int16_t));
    REQUIRE(field.value() == static_cast<std::int16_t>(0x0102));
    REQUIRE(field.valid());
}

TEST_CASE("Test4", "test4") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::int16_t
    > Field;

    static const char Buf[] = {
        (char)0xff, (char)0xff
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == sizeof(std::int16_t));
    REQUIRE(field.value() == -1);
    REQUIRE(field.valid());
}

TEST_CASE("Test5", "test5") 
{
    typedef comms::field::IntValue<
        comms::Field<LittleEndianOpt>,
        std::int16_t
    > Field;

    static const char Buf[] = {
        0x0, (char)0x80
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == sizeof(std::int16_t));
    REQUIRE(field.value() == std::numeric_limits<std::int16_t>::min());
    REQUIRE(field.valid());
}

TEST_CASE("Test6", "test6") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::int16_t,
        comms::option::FixedLength<1>
    > Field;

    static const char Buf[] = {
        (char)0xff, 0x00
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 1);
    REQUIRE(field.value() == -1);
    REQUIRE(field.valid());
}

TEST_CASE("Test7", "test7") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::int16_t,
        comms::option::FixedLength<1>,
        comms::option::NumValueSerOffset<-2000>
    > Field;

    static_assert(!Field::isVersionDependent(), "Invalid version dependency assumption");

    static const char Buf[] = {
        13
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto field = readWriteField<Field>(Buf, BufSize);

    REQUIRE(field.length() == 1);
    REQUIRE(field.value() == 2013);
    REQUIRE(field.valid());

    field.value() = 2000;
    static const char ExpectedBuf[] = {
        0
    };
    const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    field.value() = 2000 + 0x7f;
    static const char ExpectedBuf2[] = {
        (char)0x7f
    };
    const std::size_t ExpectedBufSize2 = std::extent<decltype(ExpectedBuf2)>::value;
    writeReadField(field, ExpectedBuf2, ExpectedBufSize2);
}

TEST_CASE("Test8", "test8") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::FixedLength<3>,
        comms::option::ValidNumValueRange<0, 0x010200>
    > Field;

    static_assert(!Field::isVersionDependent(), "Invalid version dependency assumption");
    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value() == 0U);
    field.value() = 0x010200;
    REQUIRE(field.value() == 0x010200);
    REQUIRE(field.valid());

    static const char Buf[] = {
        0x01, 0x02, 0x03, 0x04
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 3);
    REQUIRE(field.value() == 0x010203);
    REQUIRE(!field.valid());
}

TEST_CASE("Test9", "test9") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ValidNumValueRange<0, 10>,
#ifndef CC_COMPILER_GCC47
        comms::option::ValidNumValueRange<20, 30>,
#endif
        comms::option::DefaultNumValue<100>
    > Field;

    static_assert(!Field::isVersionDependent(), "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.value() == 100);
    REQUIRE(!field.valid());
    field.value() = 5U;
    REQUIRE(field.valid());
    field.value() = 15U;
    REQUIRE(!field.valid());
#ifndef CC_COMPILER_GCC47
    field.value() = 25U;
    REQUIRE(field.valid());
#endif

    static const char Buf[] = {
        0x05, 0x02
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 1);
    REQUIRE(field.value() == 0x05);
    REQUIRE(field.valid());
}

TEST_CASE("Test10", "test10") 
{
    typedef comms::field::BitmaskValue<
        comms::Field<BigEndianOpt>,
        comms::option::FixedLength<2>
    > Field;

    static_assert(!Field::isVersionDependent(), "Invalid version dependency assumption");
    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value() == 0U);

    static const char Buf[] = {
        (char)0xde, (char)0xad,
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 2);
    REQUIRE(field.value() == 0xdead);
    REQUIRE(field.getBitValue(0U) == true);
    REQUIRE(field.getBitValue(1U) == false);
    REQUIRE(field.getBitValue(2U) == true);
    REQUIRE(field.getBitValue(3U) == true);
    REQUIRE(field.getBitValue(4U) == false);
    REQUIRE(field.getBitValue(5U) == true);
    REQUIRE(field.getBitValue(6U) == false);
    REQUIRE(field.getBitValue(7U) == true);
    REQUIRE(field.getBitValue(8U) == false);
    REQUIRE(field.getBitValue(9U) == true);
    REQUIRE(field.getBitValue(10U) == true);
    REQUIRE(field.getBitValue(11U) == true);
    REQUIRE(field.getBitValue(12U) == true);
    REQUIRE(field.getBitValue(13U) == false);
    REQUIRE(field.getBitValue(14U) == true);
    REQUIRE(field.getBitValue(15U) == true);

    field.setBitValue(1U, true);
    REQUIRE(field.value() == 0xdeaf);

    field.setBits(0x2);
    REQUIRE(field.value() == 0xdeaf);
    REQUIRE(field.valid());

    static const char ExpectedBuf[] = {
        (char)0xde, (char)0xaf
    };
    const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test11", "test11") 
{
    typedef comms::field::BitmaskValue<
        comms::Field<LittleEndianOpt>,
        comms::option::FixedLength<3>,
        comms::option::DefaultNumValue<0xffffff>,
        comms::option::BitmaskReservedBits<0xff0000, 0>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");
    Field field;
    REQUIRE(!field.valid());
    REQUIRE(field.value() == 0xffffff);

    static const char Buf[] = {
        (char)0xde, (char)0xad, (char)0x00, (char)0xff
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 3);
    REQUIRE(field.value() == 0xadde);
    REQUIRE(field.valid());
    REQUIRE(field.getBitValue(0U) == false);
    REQUIRE(field.getBitValue(1U) == true);
    REQUIRE(field.getBitValue(2U) == true);
    REQUIRE(field.getBitValue(3U) == true);
    REQUIRE(field.getBitValue(4U) == true);
    REQUIRE(field.getBitValue(5U) == false);
    REQUIRE(field.getBitValue(6U) == true);
    REQUIRE(field.getBitValue(7U) == true);
    REQUIRE(field.getBitValue(8U) == true);
    REQUIRE(field.getBitValue(9U) == false);
    REQUIRE(field.getBitValue(10U) == true);
    REQUIRE(field.getBitValue(11U) == true);
    REQUIRE(field.getBitValue(12U) == false);
    REQUIRE(field.getBitValue(13U) == true);
    REQUIRE(field.getBitValue(14U) == false);
    REQUIRE(field.getBitValue(15U) == true);
    REQUIRE(field.getBitValue(16U) == false);
    REQUIRE(field.getBitValue(17U) == false);
    REQUIRE(field.getBitValue(18U) == false);
    REQUIRE(field.getBitValue(19U) == false);
    REQUIRE(field.getBitValue(20U) == false);
    REQUIRE(field.getBitValue(21U) == false);
    REQUIRE(field.getBitValue(22U) == false);
    REQUIRE(field.getBitValue(23U) == false);

    field.setBits(0x10000);
    REQUIRE(field.value() == 0x1adde);
    REQUIRE(!field.valid());

    field.setBitValue(0U, true);
    REQUIRE(field.value() == 0x1addf);
    field.setBitValue(16U, false);
    REQUIRE(field.value() == 0xaddf);
    REQUIRE(field.valid());
}

TEST_CASE("Test12", "test12") 
{
    typedef comms::field::EnumValue<
        comms::Field<BigEndianOpt>,
        Enum1,
        comms::option::FixedLength<1>,
        comms::option::ValidNumValueRange<0, Enum1_NumOfValues - 1>,
        comms::option::DefaultNumValue<Enum1_NumOfValues>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;

    REQUIRE(!field.valid());
    REQUIRE(field.value() == Enum1_NumOfValues);

    static const char Buf[] = {
        (char)Enum1_Value1, (char)0x3f
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 1);
    REQUIRE(field.value() == Enum1_Value1);
    REQUIRE(field.valid());

    field.value() = Enum1_NumOfValues;
    REQUIRE(!field.valid());
    field.value() = Enum1_Value2;

    static const char ExpectedBuf[] = {
        (char)Enum1_Value2
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test13", "test13") 
{
    typedef comms::field::EnumValue<
        comms::Field<BigEndianOpt>,
        Enum2,
        comms::option::FixedLength<2>,
        comms::option::ValidNumValueRange<0, (int)(Enum2::NumOfValues) - 1>,
        comms::option::DefaultNumValue<(int)Enum2::NumOfValues>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(!field.valid());
    REQUIRE(field.value() == Enum2::NumOfValues);

    static const char Buf[] = {
        0x0, (char)Enum2::Value4, (char)0x3f
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 2);

    REQUIRE(field.value() == Enum2::Value4);
    REQUIRE(field.valid());

    field.value() = Enum2::NumOfValues;
    REQUIRE(!field.valid());
    field.value() = Enum2::Value3;

    static const char ExpectedBuf[] = {
        0x0, (char)Enum2::Value3
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test14", "test14") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t
        >
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");

    Field field;
    REQUIRE(field.valid());

    static const char Buf[] = {
        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == BufSize);
    REQUIRE(field.valid());
    REQUIRE(!field.refresh());
}

TEST_CASE("Test15", "test15") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t
        >,
        comms::option::FixedSizeStorage<32>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");

    Field field;
    REQUIRE(field.valid());

    static const char Buf[] = {
        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == BufSize);
    REQUIRE(field.valid());

    static const char Buf2[] = {
        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc
    };
    static const std::size_t BufSize2 = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf2, BufSize2);
    REQUIRE(field.length() == BufSize2);
    REQUIRE(field.valid());
}

TEST_CASE("Test16", "test16") 
{
    struct SizeField : public 
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t
        >
    {
    };        

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceSizeFieldPrefix<SizeField>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceSizeFieldPrefix<SizeField>,
        comms::option::FixedSizeStorage<256>
    > StaticStorageField;

    static_assert(!StaticStorageField::isVersionDependent(),
        "Invalid version dependency assumption");


    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().empty());

    StaticStorageField staticStorageField;
    REQUIRE(staticStorageField.valid());
    REQUIRE(staticStorageField.value().empty());

    static const char ExpectedBuf[] = {
        0x0
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
    writeReadField(staticStorageField, ExpectedBuf, ExpectedBufSize);

    static const char Buf[] = {
        0x5, 'h', 'e', 'l', 'l', 'o', 'g', 'a', 'r'
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.value().size() == static_cast<std::size_t>(Buf[0]));
    REQUIRE(field.length() == field.value().size() + 1U);
    REQUIRE(field.valid());

    staticStorageField = readWriteField<StaticStorageField>(Buf, BufSize);
    REQUIRE(staticStorageField.value().size() == static_cast<std::size_t>(Buf[0]));
    REQUIRE(staticStorageField.length() == staticStorageField.value().size() + 1U);
    REQUIRE(staticStorageField.valid());
}

TEST_CASE("Test17", "test17") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ValidNumValueRange<0, 4>
    > SizeField;

    static_assert(!SizeField::isVersionDependent(),
        "Invalid version dependency assumption");


    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceSizeFieldPrefix<SizeField>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");


    REQUIRE(Field::minLength() == SizeField::maxLength());
    REQUIRE(Field::maxLength() == SizeField::maxLength() + std::numeric_limits<std::uint16_t>::max());

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceSizeFieldPrefix<SizeField>,
        comms::option::FixedSizeStorage<256>
    > StaticStorageField;

    static_assert(!StaticStorageField::isVersionDependent(),
        "Invalid version dependency assumption");


    REQUIRE(StaticStorageField::minLength() == SizeField::maxLength());
    REQUIRE(StaticStorageField::maxLength() == SizeField::maxLength() + 255);

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().empty());

    StaticStorageField staticStorageField;
    REQUIRE(staticStorageField.valid());
    REQUIRE(staticStorageField.value().empty());

    static const char Buf[] = {
        0x5, 'h', 'e', 'l', 'l', 'o', 'g', 'a', 'r'
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.value().size() == static_cast<std::size_t>(Buf[0]));
    REQUIRE(field.length() == (field.value().size() + 1U));
    REQUIRE(!field.valid());
    REQUIRE(field.value() == "hello");

    staticStorageField = readWriteField<StaticStorageField>(Buf, BufSize);
    REQUIRE(staticStorageField.value().size() == static_cast<std::size_t>(Buf[0]));
    REQUIRE(staticStorageField.length() == field.value().size() + 1U);
    REQUIRE(!staticStorageField.valid());
    REQUIRE(staticStorageField.value() == "hello");
}

TEST_CASE("Test18", "test18") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint16_t
    > SizeField;

    static_assert(!SizeField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceSizeFieldPrefix<SizeField>,
        comms::option::DefaultValueInitialiser<HelloInitialiser>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceSizeFieldPrefix<SizeField>,
        comms::option::DefaultValueInitialiser<HelloInitialiser>,
        comms::option::FixedSizeStorage<64>
    > StaticStorageField;

    static_assert(!StaticStorageField::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(!field.value().empty());
    REQUIRE(field.value() == "hello");
    field.value().clear();
    REQUIRE(field.value().empty());
    field.value() = "bla";
    REQUIRE(field.value() == "bla");
    REQUIRE(field.value().size() == 3);
    REQUIRE(field.length() == 5);

    StaticStorageField staticStorageField;
    REQUIRE(staticStorageField.valid());
    REQUIRE(!staticStorageField.value().empty());
    REQUIRE(staticStorageField.value() == "hello");
    staticStorageField.value().clear();
    REQUIRE(staticStorageField.value().empty());
    staticStorageField.value() = "bla";
    REQUIRE(staticStorageField.value() == "bla");
    REQUIRE(staticStorageField.value().size() == 3);
    REQUIRE(staticStorageField.length() == 5);

    static const char ExpectedBuf[] = {
        0x0, 0x3, 'b', 'l', 'a'
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
    writeReadField(staticStorageField, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test19", "test19") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t
    > SizeField;

    static_assert(!SizeField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceSizeFieldPrefix<SizeField>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceSizeFieldPrefix<SizeField>,
        comms::option::FixedSizeStorage<64>
    > StaticStorageField;

    static_assert(!StaticStorageField::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    auto& fieldStr = field.value();
    REQUIRE(field.valid());
    REQUIRE(fieldStr.empty());

    StaticStorageField staticStorageField;
    auto& staticStorageFieldStr = staticStorageField.value();
    REQUIRE(staticStorageField.valid());
    REQUIRE(staticStorageFieldStr.empty());

    static const std::string Str("hello");
    std::copy(Str.begin(), Str.end(), std::back_inserter(fieldStr));
    REQUIRE(!fieldStr.empty());
    REQUIRE(fieldStr.size() == Str.size());
    REQUIRE(fieldStr == Str);

    std::copy(Str.begin(), Str.end(), std::back_inserter(staticStorageFieldStr));
    REQUIRE(!staticStorageFieldStr.empty());
    REQUIRE(staticStorageFieldStr.size() == Str.size());
    REQUIRE(std::string(staticStorageFieldStr.c_str()) == Str);

    static const char ExpectedBuf[] = {
        0x5, 'h', 'e', 'l', 'l', 'o'
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
    writeReadField(staticStorageField, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test20", "test20") 
{
    typedef comms::field::IntValue<
        comms::Field<LittleEndianOpt>,
        std::uint16_t,
        comms::option::VarLength<1, 2>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static const char Buf[] = {
        (char)0x81, 0x01
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 2U);
    REQUIRE(field.value() == static_cast<std::uint16_t>(0x81));
    REQUIRE(field.valid());

    do {
        field.value() = 0x7ff;
        REQUIRE(field.length() == 2U);
        static const char ExpectedBuf[] = {
            (char)0xff, 0x0f
        };

        static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
        writeReadField(field, ExpectedBuf, ExpectedBufSize);
    } while (false);

    do {
        field.value() = 0x0;
        REQUIRE(field.length() == 1U);
        static const char ExpectedBuf[] = {
            (char)0x0
        };

        static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
        writeReadField(field, ExpectedBuf, ExpectedBufSize);
    } while (false);
}

TEST_CASE("Test21", "test21") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::VarLength<1, 3>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static const char Buf[] = {
        (char)0x83, 0x0f
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 2U);
    REQUIRE(field.value() == static_cast<std::uint32_t>(0x18f));
    REQUIRE(field.valid());

    field.value() = 0x7ff;
    REQUIRE(field.length() == 2U);
    static const char ExpectedBuf[] = {
        (char)0x8f, (char)0x7f
    };

    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    field.value() = 0x7f;
    REQUIRE(field.length() == 1U);
    REQUIRE(field.value() == 0x7f);
    static const char ExpectedBuf2[] = {
        (char)0x7f
    };

    static const std::size_t ExpectedBufSize2 = std::extent<decltype(ExpectedBuf2)>::value;
    writeReadField(field, ExpectedBuf2, ExpectedBufSize2);

    static const char Buf2[] = {
        (char)0x91, (char)0xc2, (char)0x3f, (char)0xff
    };
    static const std::size_t BufSize2 = std::extent<decltype(Buf2)>::value;
    field = readWriteField<Field>(Buf2, BufSize2);
    REQUIRE(field.length() == 3U);
    REQUIRE(field.value() == static_cast<std::uint32_t>(0x4613f));
    REQUIRE(field.valid());
}

TEST_CASE("Test22", "test22") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::VarLength<1, 3>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static const char Buf[] = {
        (char)0x83, (char)0x8f, (char)0x8c, (char)0x3f, (char)0xff
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto field = readWriteField<Field>(Buf, BufSize, comms::ErrorStatus::ProtocolError);
    static_cast<void>(field);
}

TEST_CASE("Test23", "test23") 
{
    typedef comms::field::IntValue<
        comms::Field<LittleEndianOpt>,
        std::int16_t,
        comms::option::VarLength<1, 3>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");


    Field field;

    do {
        field.value() = static_cast<int16_t>(0xc000);
        REQUIRE(field.length() == 3U);

        static const char ExpectedMinValueBuf[] = {
            (char)0x80, (char)0x80, (char)0x7f
        };

        static const std::size_t ExpectedMinValueBufSize = std::extent<decltype(ExpectedMinValueBuf)>::value;
        writeReadField(field, ExpectedMinValueBuf, ExpectedMinValueBufSize);
    } while (false);

    do {
        field.value() = static_cast<int16_t>(0xe000);
        REQUIRE(field.length() == 2U);

        static const char ExpectedMinValueBuf[] = {
            (char)0x80, (char)0x40
        };

        static const std::size_t ExpectedMinValueBufSize = std::extent<decltype(ExpectedMinValueBuf)>::value;
        writeReadField(field, ExpectedMinValueBuf, ExpectedMinValueBufSize);
    } while (false);

    do {
        field.value() = 0x1fff;
        REQUIRE(field.length() == 2U);

        static const char ExpectedMaxValueBuf[] = {
            (char)0xff, (char)0x3f
        };

        static const std::size_t ExpectedMaxValueBufSize = std::extent<decltype(ExpectedMaxValueBuf)>::value;
        writeReadField(field, ExpectedMaxValueBuf, ExpectedMaxValueBufSize);
    } while (false);

    do {
        field.value() = 0x2000;
        REQUIRE(field.length() == 3U);

        static const char ExpectedMaxValueBuf[] = {
            (char)0x80, (char)0xc0, (char)0x0
        };

        static const std::size_t ExpectedMaxValueBufSize = std::extent<decltype(ExpectedMaxValueBuf)>::value;
        writeReadField(field, ExpectedMaxValueBuf, ExpectedMaxValueBufSize);
    } while (false);
}

TEST_CASE("Test24", "test24") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        unsigned,
        comms::option::FixedLength<2>,
        comms::option::NumValueSerOffset<2>,
        comms::option::ValidNumValueRange<0, 2> > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static const char Buf[] = {
        0x00, 0x02
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    Field field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 2);
    REQUIRE(field.value() == 0x0);
    REQUIRE(field.valid());
    field.value() = 3;
    REQUIRE(!field.valid());

    static const char ExpectedBuf[] = {
        0x00, 0x05
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test25", "test25") 
{
    typedef std::tuple<
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t,
            comms::option::FixedBitLength<2>
        >,
        comms::field::BitmaskValue<
            comms::Field<BigEndianOpt>,
            comms::option::FixedLength<1>,
            comms::option::FixedBitLength<6>
        >
    > BitfileMembers;

    typedef comms::field::Bitfield<
        comms::Field<BigEndianOpt>,
        BitfileMembers
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    static_cast<void>(field);
    REQUIRE(field.length() == 1U);
    REQUIRE(field.memberBitLength<0>() == 2U);
    REQUIRE(field.memberBitLength<1>() == 6U);

    static const char Buf[] = {
        (char)0x41, (char)0xff
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize);
    auto& members = field.value();
    auto& mem1 = std::get<0>(members);
    REQUIRE(mem1.value() == 0x1);

    auto& mem2 = std::get<1>(members);
    REQUIRE(mem2.value() == 0x10);
}

TEST_CASE("Test26", "test26") 
{
    typedef std::tuple<
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t,
            comms::option::FixedBitLength<3>
        >,
        comms::field::BitmaskValue<
            comms::Field<BigEndianOpt>,
            comms::option::FixedLength<1>,
            comms::option::FixedBitLength<5>
        >
    > BitfileMembers;

    typedef comms::field::Bitfield<
        comms::Field<BigEndianOpt>,
        BitfileMembers
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    static_cast<void>(field);
    REQUIRE(field.length() == 1U);
    REQUIRE(field.memberBitLength<0>() == 3U);
    REQUIRE(field.memberBitLength<1>() == 5U);

    static const char Buf[] = {
        (char)0x09, (char)0xff
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize);
    auto& members = field.value();
    auto& mem1 = std::get<0>(members);
    REQUIRE(mem1.value() == 0x1);

    auto& mem2 = std::get<1>(members);
    REQUIRE(mem2.value() == 0x1);
}

using Test27_FieldBase = comms::Field<comms::option::BigEndian>;

typedef std::tuple<
    comms::field::IntValue<
        Test27_FieldBase,
        std::uint8_t,
        comms::option::FixedBitLength<4>
    >,
    comms::field::BitmaskValue<
        Test27_FieldBase,
        comms::option::FixedLength<1>,
        comms::option::FixedBitLength<8>
    >,
    comms::field::EnumValue<
        Test27_FieldBase,
        Enum1,
        comms::option::FixedBitLength<4>
    >
> Test27_BitfildMembers;

template <typename... TExtraOpts>
class Test27_Field : public
    comms::field::Bitfield<
        Test27_FieldBase,
        Test27_BitfildMembers,
        TExtraOpts...
    >
{
    using Base =
        comms::field::Bitfield<
            Test27_FieldBase,
            Test27_BitfildMembers,
            TExtraOpts...>;
public:
    COMMS_FIELD_MEMBERS_NAMES(mem1, mem2, mem3);
};

TEST_CASE("Test27", "test27") 
{
    using Field = Test27_Field<>;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.length() == 2U);
    REQUIRE(field.memberBitLength<Field::FieldIdx_mem1>() == 4U);
    REQUIRE(field.memberBitLength<Field::FieldIdx_mem2>() == 8U);
    REQUIRE(field.memberBitLength<Field::FieldIdx_mem3>() == 4U);

    static const char Buf[] = {
        (char)0x4f, (char)0xa1, (char)0xaa
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize);
    auto& mem1 = field.field_mem1();
    REQUIRE(mem1.value() == 0x1);

    auto& mem2 = field.field_mem2();
    REQUIRE(mem2.value() == 0xfa);

    auto& mem3 = field.field_mem3();
    REQUIRE((unsigned)mem3.value() == 0x4);
}

TEST_CASE("Test28", "test28") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t,
            comms::option::ValidNumValueRange<0, 5>
        >,
        comms::option::SequenceSizeFieldPrefix<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint16_t
            >
        >
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    REQUIRE(Field::minLength() == sizeof(std::uint16_t));

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == 0U);

    static const char Buf[] = {
        0x0, 0xa, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == BufSize);
    REQUIRE(!field.valid());
    REQUIRE(field.value().size() == 10U);

    field.value().resize(5);
    static const char ExpectedBuf[] = {
        0x0, 0x5, 0x0, 0x1, 0x2, 0x3, 0x4
    };
    REQUIRE(field.valid());
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test29", "test29") 
{
    typedef comms::field::EnumValue<
        comms::Field<BigEndianOpt>,
        Enum1,
        comms::option::FixedLength<2>,
        comms::option::ValidNumValueRange<0, Enum1_NumOfValues - 1>,
        comms::option::DefaultNumValue<Enum1_Value2>,
        comms::option::FailOnInvalid<comms::ErrorStatus::ProtocolError>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value() == Enum1_Value2);

    static const char Buf[] = {
        0x0, (char)Enum1_Value1, (char)0x3f
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 2);
    REQUIRE(field.value() == Enum1_Value1);
    REQUIRE(field.valid());

    static const char Buf2[] = {
        0x0, (char)Enum1_NumOfValues, (char)0x3f
    };
    static const std::size_t BufSize2 = std::extent<decltype(Buf2)>::value;
    readWriteField<Field>(Buf2, BufSize2, comms::ErrorStatus::ProtocolError);

    field.value() = Enum1_Value3;
    REQUIRE(field.valid());

    static const char ExpectedBuf[] = {
        0x0, (char)Enum1_Value3
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test30", "test30") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::DefaultNumValue<0x2>,
        comms::option::ValidNumValueRange<0x2, 0x2>,
        comms::option::IgnoreInvalid
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value() == 0x2);

    static const char Buf[] = {
        0x0f
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    const auto* readIter = &Buf[0];
    auto es = field.read(readIter, BufSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(field.value() == 0x2);
    REQUIRE(field.valid());

    static const char Buf2[] = {
        0x00, 0x02, (char)0xff
    };

    static const std::size_t BufSize2 = std::extent<decltype(Buf2)>::value;
    readIter = &Buf2[0];
    es = field.read(readIter, BufSize2);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(field.value() == 0x2);
    REQUIRE(field.valid());
}

TEST_CASE("Test31", "test31") 
{

    typedef comms::field::Optional<
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint16_t,
            comms::option::ValidNumValueRange<0, 10>
        >
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef Field::Mode Mode;

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.field().value() == 0U);
    REQUIRE(field.getMode() == Mode::Tentative);

    static const char Buf[] = {
        0x0f, (char)0xf0
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    const auto* readIter = &Buf[0];
    auto es = field.read(readIter, BufSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(field.field().value() == 0xff0);
    REQUIRE(!field.valid());
    REQUIRE(field.getMode() == Mode::Exists);
    field.setMode(Mode::Missing);

    char bufTmp[16] = {0};
    static const std::size_t BufTmpSize = std::extent<decltype(bufTmp)>::value;

    auto writeIter = &bufTmp[0];
    es = field.write(writeIter, BufTmpSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(writeIter == &bufTmp[0]);
}

TEST_CASE("Test32", "test32") 
{

    typedef comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint16_t,
                comms::option::ValidNumValueRange<0, 10>,
                comms::option::DefaultNumValue<5>
            >,
            comms::field::EnumValue<
                comms::Field<BigEndianOpt>,
                Enum1,
                comms::option::FixedLength<1>,
                comms::option::ValidNumValueRange<0, Enum1_NumOfValues - 1>,
                comms::option::DefaultNumValue<Enum1_Value2>
            >
        >
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 3U, "Invalid minLength");
    static_assert(Field::minLengthFrom<1>() == 1U, "Invalid minLength");
    static_assert(Field::minLengthUntil<1>() == 2U, "Invalid minLength");
    static_assert(Field::maxLength() == 3U, "Invalid maxLength");
    static_assert(Field::maxLengthFrom<1>() == 1U, "Invalid minLength");
    static_assert(Field::maxLengthUntil<1>() == 2U, "Invalid minLength");

    Field field;
    REQUIRE(field.length() == 3U);
    REQUIRE(field.lengthFrom<1>() == 1U);
    REQUIRE(field.lengthUntil<1>() == 2U);
    REQUIRE(field.valid());
    auto& intValField = std::get<0>(field.value());
    auto& enumValField = std::get<1>(field.value());
    REQUIRE(intValField.value() == 5U);
    REQUIRE(enumValField.value() == Enum1_Value2);

    intValField.value() = 50U;
    REQUIRE(!field.valid());
    intValField.value() = 1U;
    REQUIRE(field.valid());
    enumValField.value() = Enum1_NumOfValues;
    REQUIRE(!field.valid());

    static const char Buf[] = {
        0x00, 0x3, Enum1_Value3, (char)0xff
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 3U);
    REQUIRE(field.valid());
    REQUIRE(intValField.value() == 3U);
    REQUIRE(enumValField.value() == Enum1_Value3);

    intValField.value() = 0xabcd;
    enumValField.value() = Enum1_Value1;

    static const char ExpectedBuf[] = {
        (char)0xab, (char)0xcd, (char)Enum1_Value1
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    Field fieldTmp;
    auto readIter = &ExpectedBuf[0];
    auto es = fieldTmp.readFromUntil<0, 2>(readIter, ExpectedBufSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(fieldTmp == field);

    fieldTmp = Field();
    REQUIRE(fieldTmp != field);

    readIter = &ExpectedBuf[0];
    es = fieldTmp.readUntil<1>(readIter, 2);
    REQUIRE(es == comms::ErrorStatus::Success);
    es = fieldTmp.readFrom<1>(readIter, 1);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(fieldTmp == field);

    std::vector<std::uint8_t> outBuf;
    auto writeIter = std::back_inserter(outBuf);
    es = fieldTmp.writeFromUntil<0, 2>(writeIter, outBuf.max_size());
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(outBuf.size() == ExpectedBufSize);
    REQUIRE(std::equal(outBuf.begin(), outBuf.end(), (const std::uint8_t*)&ExpectedBuf[0]));

    outBuf.clear();
    writeIter = std::back_inserter(outBuf);
    es = fieldTmp.writeUntil<1>(writeIter, outBuf.max_size());
    REQUIRE(es == comms::ErrorStatus::Success);
    es = fieldTmp.writeFrom<1>(writeIter, outBuf.max_size());
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(outBuf.size() == ExpectedBufSize);
    REQUIRE(std::equal(outBuf.begin(), outBuf.end(), (const std::uint8_t*)&ExpectedBuf[0]));
}

TEST_CASE("Test33", "test33") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t
    > SizeField;

    static_assert(!SizeField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceSizeFieldPrefix<SizeField>
    > StringField;

    static_assert(!StringField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        StringField
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    REQUIRE(Field::minLength() == 0U);
    REQUIRE(Field::maxLength() == (0xffff * StringField::maxLength()));

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().empty());

    static const char Buf[] = {
        0x05, 'h', 'e', 'l', 'l', 'o', 0x03, 'b', 'l', 'a'
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == BufSize);
    REQUIRE(field.valid());
    REQUIRE(field.value()[0].value() == "hello");
    REQUIRE(field.value()[1].value() == "bla");
}

TEST_CASE("Test34", "test34") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t
        >,
        comms::option::SequenceSizeForcingEnabled
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().empty());
    static const std::size_t MaxCount = 5;
    field.forceReadElemCount(MaxCount);

    static const char Buf[] = {
        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    auto iter = &Buf[0];
    auto status = field.read(iter, BufSize);
    REQUIRE(status == comms::ErrorStatus::Success);

    auto diff = static_cast<std::size_t>(std::distance(&Buf[0], iter));
    REQUIRE(field.length() == MaxCount);
    REQUIRE(diff == MaxCount);
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == MaxCount);
}

TEST_CASE("Test35", "test35") 
{
    typedef comms::field::FloatValue<
        comms::Field<BigEndianOpt>,
        float
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(fpEquals(field.value(), 0.0f));
    field.value() = 1.23f;
    REQUIRE(fpEquals(field.value(), 1.23f));

    std::vector<std::uint8_t> buf;
    auto writeIter = std::back_inserter(buf);
    auto es = field.write(writeIter, buf.max_size());
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(buf.size() == sizeof(float));

    field = Field();
    REQUIRE(fpEquals(field.value(), 0.0f));

    const auto* readIter = &buf[0];
    es = field.read(readIter, buf.size());
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(fpEquals(field.value(), 1.23f));
}

TEST_CASE("Test36", "test36") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::SequenceFixedSize<5>
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 5U, "Invalid min length");
    static_assert(Field::maxLength() == 5U, "Invalid max length");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(Field::minLength() == 5U);
    REQUIRE(Field::maxLength() == 5U);

    static const char Buf[] = {
        0x0, 0x1, 0x2, 0x3, 0x4
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == BufSize);
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == BufSize);

    REQUIRE(!field.refresh());
}

TEST_CASE("Test37", "test37") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint16_t
        >,
        comms::option::SequenceFixedSize<3>
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 6U, "Invalid min length");
    static_assert(Field::maxLength() == 6U, "Invalid max length");

    Field field;
    REQUIRE(field.valid());

    static const char Buf[] = {
        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 6U);
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == 3U);
    REQUIRE((field.value())[0].value() == 0x1);
    REQUIRE((field.value())[1].value() == 0x203);
    REQUIRE((field.value())[2].value() == 0x405);
}

TEST_CASE("Test38", "test38") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ValidNumValueRange<0, 0>
    > TrailField;

    static_assert(!TrailField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceFixedSize<5>,
        comms::option::SequenceTrailingFieldSuffix<TrailField>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");
        
    static_assert(std::is_same<Field::ValueType, std::string>::value,
        "Invalid storage assumption assumption");        

    static_assert(Field::minLength() == 6U, "Invalid min length");
    static_assert(Field::maxLength() == 6U, "Invalid max length");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.length() == 6U);

    field.value() = "hello";
    REQUIRE(field.length() == 6U);

    static const char ExpectedBuf[] = {
        'h', 'e', 'l', 'l', 'o', 0x0
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    field.value() = "foo";
    REQUIRE(field.length() == 6U);

    static const char ExpectedBuf2[] = {
        'f', 'o', 'o', 0x0, 0x0, 0x0
    };
    static const std::size_t ExpectedBufSize2 = std::extent<decltype(ExpectedBuf2)>::value;
    writeReadField(field, ExpectedBuf2, ExpectedBufSize2);

    field = readWriteField<Field>(&ExpectedBuf2[0], ExpectedBufSize2);
    REQUIRE(field.value() == "foo");
}

TEST_CASE("Test39", "test39") 
{
    typedef comms::field::FloatValue<
        comms::Field<BigEndianOpt>,
        float,
        comms::option::ValidNumValueRange<5, 10>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(fpEquals(field.value(), 0.0f));
    REQUIRE(!field.valid());
    field.value() = 4.999999f;
    REQUIRE(fpEquals(field.value(), 4.999999f));
    REQUIRE(!field.valid());
    field.value() = 5.00001f;
    REQUIRE(fpEquals(field.value(), 5.00001f));
    REQUIRE(field.valid());
}

TEST_CASE("Test40", "test40") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<1, 100>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.value() == 0U);
    REQUIRE(field.scaleAs<double>() == 0.0);

    field.setScaled(0.15);
    REQUIRE(field.value() == 15U);


    static const char Buf[] = {
        115
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.value() == 115);
    REQUIRE(fpEquals(field.scaleAs<float>(), 1.15f));
}

TEST_CASE("Test41", "test41") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ValidNumValueRange<0, 0>
    > TermField;

    static_assert(!TermField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceTerminationFieldSuffix<TermField>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.length() == 1U);

    field.value() = "hello";
    REQUIRE(field.length() == 6U);

    static const char ExpectedBuf[] = {
        'h', 'e', 'l', 'l', 'o', 0x0
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    static const char InputBuf[] = {
        'f', 'o', 'o', 0x0, 'b', 'l', 'a'
    };

    static const std::size_t InputBufSize = std::extent<decltype(InputBuf)>::value;

    auto* readIter = &InputBuf[0];
    auto es = field.read(readIter, InputBufSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(field.value() == "foo");
    REQUIRE(field.value().size() == 3U);
    REQUIRE(std::distance(&InputBuf[0], readIter) == 4);
}

TEST_CASE("Test42", "test42") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::VarLength<1, 4>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.value() == 0U);
    REQUIRE(field.length() == 1U);

    field.value() = 127U;
    REQUIRE(field.length() == 1U);
    static const char ExpectedBuf1[] = {
        (char)0x7f
    };

    static const std::size_t ExpectedBuf1Size = std::extent<decltype(ExpectedBuf1)>::value;
    writeReadField(field, ExpectedBuf1, ExpectedBuf1Size);

    field.value() = 128U;
    REQUIRE(field.length() == 2U);
    static const char ExpectedBuf2[] = {
        (char)0x81, 0x00
    };

    static const std::size_t ExpectedBuf2Size = std::extent<decltype(ExpectedBuf2)>::value;
    writeReadField(field, ExpectedBuf2, ExpectedBuf2Size);

    field.value() = 0x3fff;
    REQUIRE(field.length() == 2U);
    static const char ExpectedBuf3[] = {
        (char)0xff, (char)0x7f
    };

    static const std::size_t ExpectedBuf3Size = std::extent<decltype(ExpectedBuf3)>::value;
    writeReadField(field, ExpectedBuf3, ExpectedBuf3Size);

    field.value() = 0x4000;
    REQUIRE(field.length() == 3U);
    static const char ExpectedBuf4[] = {
        (char)0x81, (char)0x80, (char)0x00
    };

    static const std::size_t ExpectedBuf4Size = std::extent<decltype(ExpectedBuf4)>::value;
    writeReadField(field, ExpectedBuf4, ExpectedBuf4Size);

    field.value() = 0x1fffff;
    REQUIRE(field.length() == 3U);
    static const char ExpectedBuf5[] = {
        (char)0xff, (char)0xff, (char)0x7f
    };

    static const std::size_t ExpectedBuf5Size = std::extent<decltype(ExpectedBuf5)>::value;
    writeReadField(field, ExpectedBuf5, ExpectedBuf5Size);

    field.value() = 0x200000;
    REQUIRE(field.length() == 4U);
    static const char ExpectedBuf6[] = {
        (char)0x81, (char)0x80, (char)0x80, (char)0x00
    };

    static const std::size_t ExpectedBuf6Size = std::extent<decltype(ExpectedBuf6)>::value;
    writeReadField(field, ExpectedBuf6, ExpectedBuf6Size);
}

TEST_CASE("Test43", "test43") 
{
    typedef comms::field::IntValue<
        comms::Field<LittleEndianOpt>,
        std::uint32_t,
        comms::option::VarLength<1, 4>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.value() == 0U);
    REQUIRE(field.length() == 1U);

    field.value() = 127U;
    REQUIRE(field.length() == 1U);
    static const char ExpectedBuf1[] = {
        (char)0x7f
    };

    static const std::size_t ExpectedBuf1Size = std::extent<decltype(ExpectedBuf1)>::value;
    writeReadField(field, ExpectedBuf1, ExpectedBuf1Size);

    field.value() = 128U;
    REQUIRE(field.length() == 2U);
    static const char ExpectedBuf2[] = {
        (char)0x80, 0x01
    };

    static const std::size_t ExpectedBuf2Size = std::extent<decltype(ExpectedBuf2)>::value;
    writeReadField(field, ExpectedBuf2, ExpectedBuf2Size);

    field.value() = 0x3fff;
    REQUIRE(field.length() == 2U);
    static const char ExpectedBuf3[] = {
        (char)0xff, (char)0x7f
    };

    static const std::size_t ExpectedBuf3Size = std::extent<decltype(ExpectedBuf3)>::value;
    writeReadField(field, ExpectedBuf3, ExpectedBuf3Size);

    field.value() = 0x4000;
    REQUIRE(field.length() == 3U);
    static const char ExpectedBuf4[] = {
        (char)0x80, (char)0x80, (char)0x01
    };

    static const std::size_t ExpectedBuf4Size = std::extent<decltype(ExpectedBuf4)>::value;
    writeReadField(field, ExpectedBuf4, ExpectedBuf4Size);

    field.value() = 0x1fffff;
    REQUIRE(field.length() == 3U);
    static const char ExpectedBuf5[] = {
        (char)0xff, (char)0xff, (char)0x7f
    };

    static const std::size_t ExpectedBuf5Size = std::extent<decltype(ExpectedBuf5)>::value;
    writeReadField(field, ExpectedBuf5, ExpectedBuf5Size);

    field.value() = 0x200000;
    REQUIRE(field.length() == 4U);
    static const char ExpectedBuf6[] = {
        (char)0x80, (char)0x80, (char)0x80, (char)0x01
    };

    static const std::size_t ExpectedBuf6Size = std::extent<decltype(ExpectedBuf6)>::value;
    writeReadField(field, ExpectedBuf6, ExpectedBuf6Size);
}

TEST_CASE("Test44", "test44") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::VarLength<2, 4>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.value() == 0U);
    REQUIRE(field.length() == 2U);

    static const char ExpectedBuf1[] = {
        (char)0x80, 0x00
    };

    static const std::size_t ExpectedBuf1Size = std::extent<decltype(ExpectedBuf1)>::value;
    writeReadField(field, ExpectedBuf1, ExpectedBuf1Size);

    field.value() = 127U;
    REQUIRE(field.length() == 2U);
    static const char ExpectedBuf2[] = {
        (char)0x80, 0x7f
    };

    static const std::size_t ExpectedBuf2Size = std::extent<decltype(ExpectedBuf2)>::value;
    writeReadField(field, ExpectedBuf2, ExpectedBuf2Size);

    field.value() = 128U;
    REQUIRE(field.length() == 2U);
    static const char ExpectedBuf3[] = {
        (char)0x81, 0x00
    };

    static const std::size_t ExpectedBuf3Size = std::extent<decltype(ExpectedBuf3)>::value;
    writeReadField(field, ExpectedBuf3, ExpectedBuf3Size);
}

TEST_CASE("Test45", "test45") 
{
    typedef comms::field::IntValue<
        comms::Field<LittleEndianOpt>,
        std::uint32_t,
        comms::option::VarLength<2, 4>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.value() == 0U);
    REQUIRE(field.length() == 2U);

    static const char ExpectedBuf1[] = {
        (char)0x80, 0x00
    };

    static const std::size_t ExpectedBuf1Size = std::extent<decltype(ExpectedBuf1)>::value;
    writeReadField(field, ExpectedBuf1, ExpectedBuf1Size);

    field.value() = 127U;
    REQUIRE(field.length() == 2U);
    static const char ExpectedBuf2[] = {
        (char)0xff, 0x00
    };

    static const std::size_t ExpectedBuf2Size = std::extent<decltype(ExpectedBuf2)>::value;
    writeReadField(field, ExpectedBuf2, ExpectedBuf2Size);

    field.value() = 128U;
    REQUIRE(field.length() == 2U);
    static const char ExpectedBuf3[] = {
        (char)0x80, 0x01
    };

    static const std::size_t ExpectedBuf3Size = std::extent<decltype(ExpectedBuf3)>::value;
    writeReadField(field, ExpectedBuf3, ExpectedBuf3Size);
}

TEST_CASE("Test46", "test46") 
{
    typedef comms::field::NoValue<
        comms::Field<BigEndianOpt>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;

    static const char ExpectedBuf[] = {0};
    writeReadField(field, ExpectedBuf, 0);
}

struct BundleInitialiserTest47
{
    template <typename TField>
    void operator()(TField& field) const
    {
        auto& members = field.value();
        auto& first = std::get<0>(members);
        auto& second = std::get<1>(members);
        first.value() = 1;
        second.value() = 2;
    }
};

TEST_CASE("Test47", "test47") 
{
    typedef comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint16_t
            >,
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t            >
        >,
        comms::option::DefaultValueInitialiser<BundleInitialiserTest47>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    REQUIRE(Field::minLength() == 3U);
    REQUIRE(Field::maxLength() == 3U);

    Field field;

    static const char ExpectedBuf[] = {
        (char)0x0, (char)0x1, (char)0x2
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test48", "test48") 
{

    typedef comms::field::Optional<
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint16_t
        >,
        comms::option::DefaultOptionalMode<comms::field::OptionalMode::Exists>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef Field::Mode Mode;

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.field().value() == 0U);
    REQUIRE(field.getMode() == Mode::Exists);

    field.field().value() = 0xff0;

    static const char Buf[] = {
        0x0f, (char)0xf0
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    writeReadField(field, Buf, BufSize);
}

struct BundleCustomReaderTest49
{
    template <typename TField, typename TIter>
    comms::ErrorStatus operator()(TField& field, TIter& iter, std::size_t len) const
    {
        auto& members = field.value();
        auto& first = std::get<0>(members);
        auto& second = std::get<1>(members);

        auto es = first.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        if (first.value() != 0) {
            second.setMode(comms::field::OptionalMode::Missing);
        }
        else {
            second.setMode(comms::field::OptionalMode::Exists);
        }

        return second.read(iter, len - first.length());
    }
};

TEST_CASE("Test49", "test49") 
{

    typedef comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t
            >,
            comms::field::Optional<
                comms::field::IntValue<
                    comms::Field<BigEndianOpt>,
                    std::uint16_t
                >
            >
        >,
        comms::option::CustomValueReader<BundleCustomReaderTest49>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 1U, "Invalid minLength");
    static_assert(Field::maxLength() == 3U, "Invalid maxLength");
    static_assert(Field::minLengthUntil<1>() == 1U, "Invalid minLength");
    static_assert(Field::maxLengthUntil<1>() == 1U, "Invalid maxLength");
    static_assert(Field::minLengthFrom<1>() == 0U, "Invalid minLength");
    static_assert(Field::maxLengthFrom<1>() == 2U, "Invalid maxLength");


    Field field;
    REQUIRE(field.valid());
    auto& mem1 = std::get<0>(field.value());
    auto& mem2 = std::get<1>(field.value());

    static const char Buf[] = {
        0x00, 0x10, 0x20, (char)0xff
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 3U);
    REQUIRE(mem1.value() == 0U);
    REQUIRE(mem2.field().value() == 0x1020);
    REQUIRE(mem2.getMode() == comms::field::OptionalMode::Exists);

    static const char Buf2[] = {
        0x01, 0x10, 0x20, (char)0xff
    };

    static const std::size_t Buf2Size = std::extent<decltype(Buf2)>::value;

    field = readWriteField<Field>(Buf2, Buf2Size);
    REQUIRE(field.length() == 1U);
    REQUIRE(mem1.value() == 1U);
    REQUIRE(mem2.getMode() == comms::field::OptionalMode::Missing);
}

struct Test50_Field : public comms::field::BitmaskValue<
    comms::Field<comms::option::BigEndian>,
    comms::option::FixedLength<1> >
{
    COMMS_BITMASK_BITS(first, second, third, fourth, sixth=5, seventh, eighth);
    COMMS_BITMASK_BITS_ACCESS_NOTEMPLATE(first, second, third, fourth, sixth, seventh, eighth);
};

template <typename... TExtraOpts>
class Test50_Field2 : public
    comms::field::BitmaskValue<
        comms::Field<comms::option::BigEndian>,
        comms::option::FixedLength<1>,
        TExtraOpts...
    >
{
    using Base =
        comms::field::BitmaskValue<
            comms::Field<comms::option::BigEndian>,
            comms::option::FixedLength<1>,
            TExtraOpts...
        >;
public:
    COMMS_BITMASK_BITS_SEQ(first, second, third, fourth, fifth, sixth, seventh, eighth);
};

TEST_CASE("Test50", "test50") 
{
    using Field = Test50_Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    field.value() = 0xaa;
    REQUIRE(field.getBitValue_first() == false);
    REQUIRE(field.getBitValue_second() == true);
    REQUIRE(field.getBitValue_third() == false);
    REQUIRE(field.getBitValue_fourth() == true);
    REQUIRE(field.getBitValue_sixth() == true);
    REQUIRE(field.getBitValue_seventh() == false);
    REQUIRE(field.getBitValue_eighth() == true);

    field.setBitValue_first(true);
    field.setBitValue_second(false);
    field.setBitValue_third(true);
    field.setBitValue_fourth(false);
    field.setBitValue_sixth(false);
    field.setBitValue_seventh(true);
    field.setBitValue_eighth(false);

    REQUIRE(field.value() == 0x45);

    using Field2 = Test50_Field2<>;
    Field2 field2;
    static_cast<void>(field2);

    static_assert(!Field2::isVersionDependent(),
        "Invalid version dependency assumption");
}

class Field_51 : public comms::field::Bitfield<
        comms::Field<BigEndianOpt>,
        std::tuple<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t,
                comms::option::FixedBitLength<2>
            >,
            comms::field::BitmaskValue<
                comms::Field<BigEndianOpt>,
                comms::option::FixedLength<1>,
                comms::option::FixedBitLength<6>
            >
        >
    >
{
public:
    COMMS_FIELD_MEMBERS_ACCESS_NOTEMPLATE(name1, name2)
};

TEST_CASE("Test51", "test51") 
{
    typedef Field_51 Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.length() == 1U);
    REQUIRE(field.memberBitLength<Field::FieldIdx_name1>() == 2U);
    REQUIRE(field.memberBitLength<Field::FieldIdx_name2>() == 6U);

    static const char Buf[] = {
        (char)0x41, (char)0xff
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize);
    auto& mem1 = field.field_name1();
    REQUIRE(mem1.value() == 0x1);

    auto& mem2 = field.field_name2();
    REQUIRE(mem2.value() == 0x10);
}

TEST_CASE("Test52", "test52") 
{
    typedef std::tuple<
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t,
            comms::option::FixedBitLength<8>
        >,
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::int8_t,
            comms::option::FixedBitLength<8>
        >
    > BitfildMembers;

    typedef comms::field::Bitfield<
        comms::Field<BigEndianOpt>,
        BitfildMembers
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    static_cast<void>(field);
    REQUIRE(field.length() == 2U);
    REQUIRE(field.memberBitLength<0>() == 8U);
    REQUIRE(field.memberBitLength<1>() == 8U);

    static const char Buf[] = {
        (char)0xff, (char)0xff
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize);
    auto& members = field.value();
    auto& mem1 = std::get<0>(members);
    REQUIRE(mem1.value() == 255);

    auto& mem2 = std::get<1>(members);
    REQUIRE(mem2.value() == -1);
}

TEST_CASE("Test53", "test53") 
{
    typedef comms::field::IntValue<
        comms::Field<LittleEndianOpt>,
        std::int32_t,
        comms::option::FixedBitLength<23>,
        comms::option::ScalingRatio<180, 0x800000>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field(std::numeric_limits<std::int32_t>::max());
    static const double ExpVal1 =
        (static_cast<double>(std::numeric_limits<std::int32_t>::max()) * 180) / 0x800000;
    REQUIRE(field.scaleAs<double>() == ExpVal1);
}

TEST_CASE("Test54", "test54") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::int8_t,
        comms::option::ScalingRatio<100, 1>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field(1);

    REQUIRE(field.value() == 1);
    REQUIRE(field.scaleAs<int>() == 100);

    field.setScaled(1000);
    REQUIRE(field.value() == 10);

    field.setScaled(260.38);
    REQUIRE(field.value() == 2);

    field.setScaled(-200.00);
    REQUIRE(field.value() == -2);
}

TEST_CASE("Test55", "test55") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::int16_t,
        comms::option::ScalingRatio<1, 100>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;

    field.setScaled(-0.1);
    REQUIRE(field.value() == -10);

    field.value() = -123;
    REQUIRE(field.scaleAs<float>() == -1.23f);
}

TEST_CASE("Test56", "test56") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ValidNumValueRange<0, 0>
    > TrailField;

    static_assert(!TrailField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceFixedSize<5>,
        comms::option::FixedSizeStorage<5>,
        comms::option::SequenceTrailingFieldSuffix<TrailField>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 6U, "Invalid min length");
    static_assert(Field::maxLength() == 6U, "Invalid max length");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.length() == 6U);

    field.value() = "hello";
    REQUIRE(field.length() == 6U);

    static const char ExpectedBuf[] = {
        'h', 'e', 'l', 'l', 'o', 0x0
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    field.value() = "foo";
    REQUIRE(field.length() == 6U);

    static const char ExpectedBuf2[] = {
        'f', 'o', 'o', 0x0, 0x0, 0x0
    };
    static const std::size_t ExpectedBufSize2 = std::extent<decltype(ExpectedBuf2)>::value;
    writeReadField(field, ExpectedBuf2, ExpectedBufSize2);

    field = readWriteField<Field>(&ExpectedBuf2[0], ExpectedBufSize2);
    REQUIRE(field.value() == "foo");
}

TEST_CASE("Test57", "test57") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::ScalingRatio<1, 10>,
        comms::option::UnitsMilliseconds
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static const std::uint32_t InitVal = 600000;
    Field field;
    field.value() = InitVal;
    REQUIRE(comms::units::getMilliseconds<unsigned>(field) == (InitVal / 10));
    REQUIRE(comms::units::getMicroseconds<unsigned long>(field) == (InitVal * 1000L) / 10);
    REQUIRE(comms::units::getNanoseconds<unsigned long long>(field) == (InitVal * 1000ULL * 1000) / 10);
    REQUIRE(comms::units::getSeconds<unsigned>(field) == InitVal / (10 * 1000));
    REQUIRE(comms::units::getMinutes<unsigned>(field) == InitVal / (10 * 60 * 1000));
    REQUIRE(comms::units::getHours<double>(field) == (double)InitVal / (10 * 60 * 60 * 1000));
    REQUIRE(comms::units::getDays<double>(field) == (double)InitVal / (10 * 24L * 60 * 60 * 1000));
    REQUIRE(comms::units::getWeeks<double>(field) == (double)InitVal / (10 * 7ULL * 24 * 60 * 60 * 1000));

    comms::units::setNanoseconds(field, 500000U);
    REQUIRE(comms::units::getNanoseconds<unsigned>(field) == 500000U);
    REQUIRE(field.value() == 5);

    comms::units::setMicroseconds(field, 300U);
    REQUIRE(comms::units::getMicroseconds<unsigned>(field) == 300U);
    REQUIRE(field.value() == 3);

    comms::units::setMilliseconds(field, 100U);
    REQUIRE(comms::units::getMilliseconds<unsigned>(field) == 100U);
    REQUIRE(fpEquals(comms::units::getSeconds<float>(field), 0.1f));
    REQUIRE(field.value() == 1000);

    comms::units::setSeconds(field, 1.2);
    REQUIRE(fpEquals(comms::units::getSeconds<float>(field), 1.2f));
    REQUIRE(comms::units::getMilliseconds<unsigned>(field) == 1200U);
    REQUIRE(field.value() == 12000);

    comms::units::setMinutes(field, (double)1/3);
    REQUIRE(fpEquals(comms::units::getMinutes<double>(field), (double)1/3));
    REQUIRE(fpEquals(comms::units::getHours<double>(field), (double)1/(3*60)));
    REQUIRE(comms::units::getSeconds<unsigned>(field) == 20U);
    REQUIRE(comms::units::getMilliseconds<unsigned>(field) == 20000U);
    REQUIRE(field.value() == 200000);

    comms::units::setHours(field, 0.5f);
    REQUIRE(fpEquals(comms::units::getHours<double>(field), 0.5));
    REQUIRE(comms::units::getMinutes<unsigned>(field) == 30U);
    REQUIRE(comms::units::getSeconds<unsigned>(field) == 30U * 60U);
    REQUIRE(comms::units::getMilliseconds<unsigned>(field) == 30U * 60U * 1000U);
    REQUIRE(field.value() == 30U * 60U * 1000U * 10U);

    comms::units::setDays(field, (float)1/3);
    REQUIRE(fpEquals(comms::units::getDays<double>(field), (double)1/3));
    REQUIRE(comms::units::getHours<unsigned>(field) == 8U);
    REQUIRE(comms::units::getMinutes<unsigned>(field) == 8U * 60);
    REQUIRE(comms::units::getSeconds<unsigned>(field) == 8U * 60U * 60U);
    REQUIRE(comms::units::getMilliseconds<unsigned long>(field) == 8UL * 60U * 60U * 1000U);
    REQUIRE(field.value() == 8UL * 60U * 60U * 1000U * 10U);

    comms::units::setWeeks(field, (double)2/7);
    REQUIRE(fpEquals(comms::units::getWeeks<double>(field), (double)2/7));
    REQUIRE(comms::units::getDays<unsigned>(field) == 2U);
    REQUIRE(comms::units::getHours<unsigned>(field) == 2U * 24U);
    REQUIRE(comms::units::getMinutes<unsigned>(field) == 2U * 24 * 60);
    REQUIRE(comms::units::getSeconds<unsigned long>(field) == 2UL * 24U * 60U * 60U);
    REQUIRE(comms::units::getMilliseconds<unsigned long>(field) == 2UL * 24U * 60U * 60U * 1000U);
    REQUIRE(field.value() == 2UL * 24U * 60U * 60U * 1000U * 10U);
}

TEST_CASE("Test58", "test58") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<100, 1>,
        comms::option::UnitsNanoseconds
    > Field1;

    static_assert(!Field1::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isNanoseconds<Field1>(), "Invalid units");

    do {
        Field1 field(1U);
        static_assert(comms::units::isNanoseconds(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(comms::units::getNanoseconds<unsigned>(field) == 100U);
        REQUIRE(fpEquals(comms::units::getMicroseconds<double>(field), 0.1));
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<100, 1>,
        comms::option::UnitsMicroseconds
    > Field2;

    static_assert(!Field2::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isMicroseconds<Field2>(), "Invalid units");

    do {
        Field2 field(5U);
        static_assert(comms::units::isMicroseconds(field), "Invalid units");
        REQUIRE(field.value() == 5U);
        REQUIRE(comms::units::getMicroseconds<unsigned>(field) == 500U);
        REQUIRE(fpEquals(comms::units::getMilliseconds<double>(field), 0.5));
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::UnitsMilliseconds
    > Field3;

    static_assert(!Field3::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isMilliseconds<Field3>(), "Invalid units");

    do {
        Field3 field(200U);
        static_assert(comms::units::isMilliseconds(field), "Invalid units");
        REQUIRE(field.value() == 200U);
        REQUIRE(comms::units::getMilliseconds<unsigned>(field) == 200U);
        REQUIRE(fpEquals(comms::units::getSeconds<double>(field), 0.2));
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<1, 10>,
        comms::option::UnitsSeconds
    > Field4;

    static_assert(!Field4::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isSeconds<Field4>(), "Invalid units");

    do {
        Field4 field(1U);
        static_assert(comms::units::isSeconds(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(fpEquals(comms::units::getSeconds<double>(field), 0.1));
        REQUIRE(comms::units::getMilliseconds<unsigned>(field) == 100U);
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<1, 10>,
        comms::option::UnitsMinutes
    > Field5;

    static_assert(!Field5::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isMinutes<Field5>(), "Invalid units");

    do {
        Field5 field(1U);
        static_assert(comms::units::isMinutes(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(fpEquals(comms::units::getMinutes<double>(field), 0.1));
        REQUIRE(comms::units::getSeconds<unsigned>(field) == 6U);
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<1, 10>,
        comms::option::UnitsHours
    > Field6;

    static_assert(!Field6::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isHours<Field6>(), "Invalid units");

    do {
        Field6 field(1U);
        static_assert(comms::units::isHours(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(fpEquals(comms::units::getHours<double>(field), 0.1));
        REQUIRE(comms::units::getSeconds<unsigned>(field) == 6U * 60U);
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<1, 12>,
        comms::option::UnitsDays
    > Field7;

    static_assert(!Field7::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isDays<Field7>(), "Invalid units");

    do {
        Field7 field(1U);
        static_assert(comms::units::isDays(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(fpEquals(comms::units::getDays<double>(field), (double)1/12));
        REQUIRE(comms::units::getHours<unsigned>(field) == 2U);
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::UnitsWeeks
    > Field8;

    static_assert(!Field8::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isWeeks<Field8>(), "Invalid units");

    do {
        Field8 field(1U);
        static_assert(comms::units::isWeeks(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(comms::units::getWeeks<unsigned>(field) == 1U);
        REQUIRE(comms::units::getHours<unsigned>(field) == 24U * 7U);
    } while (false);
}

TEST_CASE("Test59", "test59") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::UnitsMillimeters
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    field.value() = 345U;
    REQUIRE(comms::units::getNanometers<unsigned long long >(field) == 345000000UL);
    REQUIRE(comms::units::getMicrometers<unsigned>(field) == 345000U);
    REQUIRE(comms::units::getMillimeters<unsigned>(field) == 345U);
    REQUIRE(fpEquals(comms::units::getCentimeters<double>(field), 34.5));
    REQUIRE(fpEquals(comms::units::getMeters<double>(field), 0.345));
    REQUIRE(fpEquals(comms::units::getKilometers<double>(field), 0.000345));

    comms::units::setNanometers(field, 100000000UL);
    REQUIRE(field.value() == 100U);
    REQUIRE(comms::units::getMillimeters<unsigned>(field) == 100U);

    comms::units::setMicrometers(field, 222000UL);
    REQUIRE(field.value() == 222U);
    REQUIRE(comms::units::getMillimeters<unsigned>(field) == 222U);

    comms::units::setMillimeters(field, 400);
    REQUIRE(field.value() == 400U);
    REQUIRE(comms::units::getMicrometers<unsigned>(field) == 400000U);

    comms::units::setCentimeters(field, 10);
    REQUIRE(comms::units::getMillimeters<unsigned>(field) == 100U);

    comms::units::setMeters(field, 0.025);
    REQUIRE(comms::units::getMillimeters<unsigned>(field) == 25U);

    comms::units::setKilometers(field, 0.025);
    REQUIRE(comms::units::getMeters<unsigned>(field) == 25U);
}

TEST_CASE("Test60", "test60") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<100, 1>,
        comms::option::UnitsNanometers
    > Field1;

    static_assert(!Field1::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isNanometers<Field1>(), "Invalid units");

    do {
        Field1 field(1U);
        static_assert(comms::units::isNanometers(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(comms::units::getNanometers<unsigned>(field) == 100U);
        REQUIRE(fpEquals(comms::units::getMicrometers<double>(field), 0.1));
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<100, 1>,
        comms::option::UnitsMicrometers
    > Field2;

    static_assert(!Field2::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isMicrometers<Field2>(), "Invalid units");

    do {
        Field2 field(5U);
        static_assert(comms::units::isMicrometers(field), "Invalid units");
        REQUIRE(field.value() == 5U);
        REQUIRE(comms::units::getMicrometers<unsigned>(field) == 500U);
        REQUIRE(fpEquals(comms::units::getMillimeters<double>(field), 0.5));
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::UnitsMillimeters
    > Field3;

    static_assert(!Field3::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isMillimeters<Field3>(), "Invalid units");

    do {
        Field3 field(200U);
        static_assert(comms::units::isMillimeters(field), "Invalid units");
        REQUIRE(field.value() == 200U);
        REQUIRE(comms::units::getMillimeters<unsigned>(field) == 200U);
        REQUIRE(fpEquals(comms::units::getMeters<double>(field), 0.2));
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<1, 10>,
        comms::option::UnitsMeters
    > Field4;

    static_assert(!Field4::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isMeters<Field4>(), "Invalid units");

    do {
        Field4 field(1U);
        static_assert(comms::units::isMeters(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(fpEquals(comms::units::getMeters<double>(field), 0.1));
        REQUIRE(comms::units::getMillimeters<unsigned>(field) == 100U);
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<1, 10>,
        comms::option::UnitsCentimeters
    > Field5;

    static_assert(!Field5::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isCentimeters<Field5>(), "Invalid units");

    do {
        Field5 field(1U);
        static_assert(comms::units::isCentimeters(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(fpEquals(comms::units::getCentimeters<double>(field), 0.1));
        REQUIRE(comms::units::getMillimeters<unsigned>(field) == 1U);
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<1, 10>,
        comms::option::UnitsKilometers
    > Field6;

    static_assert(!Field6::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isKilometers<Field6>(), "Invalid units");

    do {
        Field6 field(1U);
        static_assert(comms::units::isKilometers(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(fpEquals(comms::units::getKilometers<double>(field), 0.1));
        REQUIRE(comms::units::getMeters<unsigned>(field) == 100U);
    } while (false);
}

TEST_CASE("Test61", "test61") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::UnitsCentimetersPerSecond
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    field.value() = 10U;
    REQUIRE(comms::units::getNanometersPerSecond<unsigned long long >(field) == 100000000UL);
    REQUIRE(comms::units::getMicrometersPerSecond<unsigned>(field) == 100000U);
    REQUIRE(comms::units::getMillimetersPerSecond<unsigned>(field) == 100U);
    REQUIRE(comms::units::getCentimetersPerSecond<unsigned>(field) == 10U);
    REQUIRE(fpEquals(comms::units::getMetersPerSecond<double>(field), 0.1));
    REQUIRE(fpEquals(comms::units::getKilometersPerSecond<double>(field), 0.0001));
    REQUIRE(fpEquals(comms::units::getKilometersPerHour<double>(field), (0.1 * 3600) / 1000));

    comms::units::setNanometersPerSecond(field, 50000000UL);
    REQUIRE(field.value() == 5U);
    REQUIRE(comms::units::getMillimetersPerSecond<unsigned>(field) == 50U);

    comms::units::setMicrometersPerSecond(field, 10000UL);
    REQUIRE(field.value() == 1U);
    REQUIRE(comms::units::getMillimetersPerSecond<unsigned>(field) == 10U);

    comms::units::setMillimetersPerSecond(field, 400);
    REQUIRE(field.value() == 40U);
    REQUIRE(comms::units::getCentimetersPerSecond<unsigned>(field) == 40U);

    comms::units::setCentimetersPerSecond(field, 10);
    REQUIRE(comms::units::getMillimetersPerSecond<unsigned>(field) == 100U);

    comms::units::setMetersPerSecond(field, 0.02);
    REQUIRE(comms::units::getMillimetersPerSecond<unsigned>(field) == 20U);

    comms::units::setKilometersPerSecond(field, 0.00002);
    REQUIRE(comms::units::getMillimetersPerSecond<unsigned>(field) == 20U);

    comms::units::setKilometersPerHour(field, 36);
    REQUIRE(comms::units::getMetersPerSecond<unsigned>(field) == 10U);
}

TEST_CASE("Test62", "test62") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<100, 1>,
        comms::option::UnitsNanometersPerSecond
    > Field1;

    static_assert(!Field1::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isNanometersPerSecond<Field1>(), "Invalid units");

    do {
        Field1 field(1U);
        static_assert(comms::units::isNanometersPerSecond(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(comms::units::getNanometersPerSecond<unsigned>(field) == 100U);
        REQUIRE(fpEquals(comms::units::getMicrometersPerSecond<double>(field), 0.1));
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<100, 1>,
        comms::option::UnitsMicrometersPerSecond
    > Field2;

    static_assert(!Field2::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isMicrometersPerSecond<Field2>(), "Invalid units");

    do {
        Field2 field(5U);
        static_assert(comms::units::isMicrometersPerSecond(field), "Invalid units");
        REQUIRE(field.value() == 5U);
        REQUIRE(comms::units::getMicrometersPerSecond<unsigned>(field) == 500U);
        REQUIRE(fpEquals(comms::units::getMillimetersPerSecond<double>(field), 0.5));
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::UnitsMillimetersPerSecond
    > Field3;

    static_assert(!Field3::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isMillimetersPerSecond<Field3>(), "Invalid units");

    do {
        Field3 field(200U);
        static_assert(comms::units::isMillimetersPerSecond(field), "Invalid units");
        REQUIRE(field.value() == 200U);
        REQUIRE(comms::units::getMillimetersPerSecond<unsigned>(field) == 200U);
        REQUIRE(fpEquals(comms::units::getMetersPerSecond<double>(field), 0.2));
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<1, 10>,
        comms::option::UnitsMetersPerSecond
    > Field4;

    static_assert(!Field4::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isMetersPerSecond<Field4>(), "Invalid units");

    do {
        Field4 field(1U);
        static_assert(comms::units::isMetersPerSecond(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(fpEquals(comms::units::getMetersPerSecond<double>(field), 0.1));
        REQUIRE(comms::units::getMillimetersPerSecond<unsigned>(field) == 100U);
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ScalingRatio<1, 10>,
        comms::option::UnitsCentimetersPerSecond
    > Field5;

    static_assert(!Field5::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isCentimetersPerSecond<Field5>(), "Invalid units");

    do {
        Field5 field(1U);
        static_assert(comms::units::isCentimetersPerSecond(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(fpEquals(comms::units::getCentimetersPerSecond<double>(field), 0.1));
        REQUIRE(comms::units::getMillimetersPerSecond<unsigned>(field) == 1U);
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::UnitsKilometersPerHour
    > Field6;

    static_assert(!Field6::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isKilometersPerHour<Field6>(), "Invalid units");

    do {
        Field6 field(36U);
        static_assert(comms::units::isKilometersPerHour(field), "Invalid units");
        REQUIRE(field.value() == 36U);
        REQUIRE(comms::units::getMetersPerSecond<unsigned>(field) == 10U);
    } while (false);

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::UnitsKilometersPerSecond
    > Field7;

    static_assert(!Field7::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isKilometersPerSecond<Field7>(), "Invalid units");

    do {
        Field7 field(1U);
        static_assert(comms::units::isKilometersPerSecond(field), "Invalid units");
        REQUIRE(field.value() == 1U);
        REQUIRE(comms::units::getMetersPerSecond<unsigned>(field) == 1000U);
    } while (false);
}

TEST_CASE("Test63", "test63") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::UnitsKilohertz
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isKilohertz<Field>(), "Invalid units");

    Field field;
    static_assert(comms::units::isKilohertz(field), "Invalid units");
    field.value() = 10U;
    REQUIRE(comms::units::getHertz<unsigned long>(field) == 10000UL);
    REQUIRE(comms::units::getKilohertz<unsigned>(field) == 10U);
    REQUIRE(fpEquals(comms::units::getMegahertz<double>(field), 0.01));
    REQUIRE(fpEquals(comms::units::getGigahertz<double>(field), 0.00001));

    comms::units::setHertz(field, 20000U);
    REQUIRE(comms::units::getKilohertz<unsigned>(field) == 20U);

    comms::units::setKilohertz(field, 1);
    REQUIRE(comms::units::getHertz<unsigned long>(field) == 1000L);

    comms::units::setMegahertz(field, 2);
    REQUIRE(comms::units::getHertz<unsigned long>(field) == 2000000UL);

    comms::units::setGigahertz(field, 3);
    REQUIRE(comms::units::getKilohertz<unsigned long>(field) == 3000000UL);
}

TEST_CASE("Test64", "test64") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::ScalingRatio<1, 10>,
        comms::option::UnitsDegrees
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isDegrees<Field>(), "Invalid units");

    Field field;
    static_assert(comms::units::isDegrees(field), "Invalid units");
    field.value() = 300U;
    REQUIRE(comms::units::getDegrees<unsigned>(field) == 30U);
    REQUIRE(std::abs(comms::units::getRadians<double>(field) - 0.523599) <= 0.000001);

    comms::units::setDegrees(field, 50U);
    REQUIRE(field.value() == 500U);
    REQUIRE(comms::units::getDegrees<unsigned>(field) == 50U);
    REQUIRE(std::abs(comms::units::getRadians<double>(field) - 0.872665) <= 0.000001);

    comms::units::setRadians(field, 1.04719);
    REQUIRE(comms::units::getDegrees<unsigned>(field) == 60U);
    REQUIRE(field.value() == 600U);
    REQUIRE(std::abs(comms::units::getRadians<double>(field) - 1.04719) <= 0.00001);
}

TEST_CASE("Test65", "test65") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::ScalingRatio<1, 100>,
        comms::option::UnitsRadians
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isRadians<Field>(), "Invalid units");

    Field field;
    static_assert(comms::units::isRadians(field), "Invalid units");
    field.value() = 100U;
    REQUIRE(comms::units::getRadians<unsigned>(field) == 1U);
    REQUIRE(std::abs(comms::units::getDegrees<double>(field) - 57.2958) <= 0.0001);

    comms::units::setRadians(field, 0.5);
    REQUIRE(field.value() == 50U);
    REQUIRE(fpEquals(comms::units::getRadians<double>(field), 0.5));
    REQUIRE(std::abs(comms::units::getDegrees<double>(field) - 28.6479) <= 0.0001);

    comms::units::setDegrees(field, 114.592);
    REQUIRE(comms::units::getRadians<unsigned>(field) == 2U);
    REQUIRE(field.value() == 200U);
    REQUIRE(std::abs(comms::units::getDegrees<double>(field) - 114.592) <= 0.001);
}

TEST_CASE("Test66", "test66") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::UnitsMilliamps
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isMilliamps<Field>(), "Invalid units");

    Field field;
    static_assert(comms::units::isMilliamps(field), "Invalid units");
    field.value() = 345U;
    REQUIRE(comms::units::getNanoamps<unsigned long long>(field) == 345000000UL);
    REQUIRE(comms::units::getMicroamps<unsigned>(field) == 345000U);
    REQUIRE(comms::units::getMilliamps<unsigned>(field) == 345U);
    REQUIRE(fpEquals(comms::units::getAmps<double>(field), 0.345));
    REQUIRE(fpEquals(comms::units::getKiloamps<double>(field), 0.000345));

    comms::units::setNanoamps(field, 100000000UL);
    REQUIRE(field.value() == 100U);
    REQUIRE(comms::units::getMilliamps<unsigned>(field) == 100U);

    comms::units::setMicroamps(field, 222000UL);
    REQUIRE(field.value() == 222U);
    REQUIRE(comms::units::getMilliamps<unsigned>(field) == 222U);

    comms::units::setMilliamps(field, 400);
    REQUIRE(field.value() == 400U);
    REQUIRE(comms::units::getMicroamps<unsigned>(field) == 400000U);

    comms::units::setAmps(field, 0.025);
    REQUIRE(comms::units::getMilliamps<unsigned>(field) == 25U);

    comms::units::setKiloamps(field, 0.025);
    REQUIRE(comms::units::getAmps<unsigned>(field) == 25U);
}

TEST_CASE("Test67", "test67") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint32_t,
        comms::option::UnitsMillivolts
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isMillivolts<Field>(), "Invalid units");

    Field field;
    static_assert(comms::units::isMillivolts(field), "Invalid units");
    field.value() = 345U;
    REQUIRE(comms::units::getNanovolts<unsigned long long >(field) == 345000000UL);
    REQUIRE(comms::units::getMicrovolts<unsigned>(field) == 345000U);
    REQUIRE(comms::units::getMillivolts<unsigned>(field) == 345U);
    REQUIRE(fpEquals(comms::units::getVolts<double>(field), 0.345));
    REQUIRE(fpEquals(comms::units::getKilovolts<double>(field), 0.000345));

    comms::units::setNanovolts(field, 100000000UL);
    REQUIRE(field.value() == 100U);
    REQUIRE(comms::units::getMillivolts<unsigned>(field) == 100U);

    comms::units::setMicrovolts(field, 222000UL);
    REQUIRE(field.value() == 222U);
    REQUIRE(comms::units::getMillivolts<unsigned>(field) == 222U);

    comms::units::setMillivolts(field, 400);
    REQUIRE(field.value() == 400U);
    REQUIRE(comms::units::getMicrovolts<unsigned>(field) == 400000U);

    comms::units::setVolts(field, 0.025);
    REQUIRE(comms::units::getMillivolts<unsigned>(field) == 25U);

    comms::units::setKilovolts(field, 0.025);
    REQUIRE(comms::units::getVolts<unsigned>(field) == 25U);
}

TEST_CASE("Test68", "test68") 
{
    typedef comms::field::FloatValue<
        comms::Field<BigEndianOpt>,
        float,
        comms::option::UnitsSeconds
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(comms::units::isSeconds<Field>(), "Invalid units");

    Field field;
    static_assert(comms::units::isSeconds(field), "Invalid units");
    field.value() = 1.345f;

    REQUIRE(fpEquals(field.value(), 1.345f));
    REQUIRE(comms::units::getMicroseconds<unsigned>(field) == 1345000U);
    REQUIRE(comms::units::getMilliseconds<unsigned>(field) == 1345U);
    REQUIRE(fpEquals(comms::units::getSeconds<float>(field), 1.345f));

    comms::units::setMilliseconds(field, 500U);
    REQUIRE(fpEquals(field.value(), 0.5f));
    REQUIRE(comms::units::getMilliseconds<unsigned>(field) == 500U);
    REQUIRE(fpEquals(comms::units::getSeconds<float>(field), 0.5f));

    comms::units::setMinutes(field, (float)1/180);
    REQUIRE(fpEquals(comms::units::getSeconds<float>(field), (float)1/3));
    REQUIRE(comms::units::getMilliseconds<unsigned>(field) == 333U);
    REQUIRE(fpEquals(comms::units::getMilliseconds<float>(field), (333 + (float)1/3)));
}

TEST_CASE("Test69", "test69") 
{
    struct LenField : public
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t
        >
    {
    };

    static_assert(!LenField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::IntValue<comms::Field<BigEndianOpt>, std::uint16_t>,
        comms::option::SequenceSerLengthFieldPrefix<LenField>
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().empty());

    static const char ExpectedBuf[] = {
        0x0
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    static const char Buf[] = {
        0x8, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.value().size() == static_cast<std::size_t>(Buf[0]) / 2U);
    REQUIRE(field.length() == (field.value().size() * 2) + 1U);
    REQUIRE(field.value()[0].value() == 0x0102);
    REQUIRE(field.value()[1].value() == 0x0304);
    REQUIRE(field.value()[2].value() == 0x0506);
    REQUIRE(field.value()[3].value() == 0x0708);

    static const char Buf2[] = {
        0x7, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8
    };
    static const std::size_t Buf2Size = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf2, Buf2Size, comms::ErrorStatus::InvalidMsgData);

    static const char Buf3[] = {
        0x4, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
    };
    static const std::size_t Buf3Size = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf3, Buf3Size);
    REQUIRE(field.value().size() == static_cast<std::size_t>(Buf3[0]) / 2U);
    REQUIRE(field.length() == (field.value().size() * 2) + 1U);
    REQUIRE(field.value()[0].value() == 0x0a0b);
    REQUIRE(field.value()[1].value() == 0x0c0d);

    static const char Buf4[] = {
        0x3, 0xa, 0xb, 0xc
    };
    static const std::size_t Buf4Size = std::extent<decltype(Buf)>::value; 
    field = readWriteField<Field>(Buf4, Buf4Size, comms::ErrorStatus::InvalidMsgData);
}

using Test70_FieldBase = comms::Field<comms::option::BigEndian>;

template <std::uint8_t TVal>
using Test70_IntKeyField =
    comms::field::IntValue<
        Test70_FieldBase,
        std::uint8_t,
        comms::option::DefaultNumValue<TVal>,
        comms::option::ValidNumValueRange<TVal, TVal>,
        comms::option::FailOnInvalid<>
>;

class Test70_Mem1 : public
    comms::field::Bundle<
        Test70_FieldBase,
        std::tuple<
            Test70_IntKeyField<1>,
            comms::field::IntValue<Test70_FieldBase, std::uint16_t>
        >
    >
{
    using Base =
        comms::field::Bundle<
            Test70_FieldBase,
            std::tuple<
                Test70_IntKeyField<1>,
                comms::field::IntValue<Test70_FieldBase, std::uint16_t>
            >
        >;
public:
    COMMS_FIELD_MEMBERS_NAMES(key, value);
};

class Test70_Mem2 : public
    comms::field::Bundle<
        Test70_FieldBase,
        std::tuple<
            Test70_IntKeyField<2>,
            comms::field::IntValue<Test70_FieldBase, std::uint32_t>
        >
    >
{
    using Base =
        comms::field::Bundle<
            Test70_FieldBase,
            std::tuple<
                Test70_IntKeyField<2>,
                comms::field::IntValue<Test70_FieldBase, std::uint32_t>
            >
        >;

public:
    COMMS_FIELD_MEMBERS_NAMES(key, value);
};

template <typename... TExtra>
class Test70_Field : public
    comms::field::Variant<
        Test70_FieldBase,
        std::tuple<
            Test70_Mem1,
            Test70_Mem2
        >,
        TExtra...
    >
{
    using Base =
        comms::field::Variant<
            Test70_FieldBase,
            std::tuple<
                Test70_Mem1,
                Test70_Mem2
            >,
            TExtra...
        >;
public:
    COMMS_VARIANT_MEMBERS_NAMES(mem1, mem2);

    static_assert(std::is_same<typename Field_mem1::Field_key::ValueType, std::uint8_t>::value, "Invalid type");
    static_assert(std::is_same<typename Field_mem1::Field_value::ValueType, std::uint16_t>::value, "Invalid type");
    static_assert(std::is_same<typename Field_mem2::Field_value::ValueType, std::uint32_t>::value, "Invalid type");
};

class Test70_LengthRetriever
{
public:
    Test70_LengthRetriever(std::size_t& val) : val_(val) {}
    template <std::size_t TIdx, typename TField>
    void operator()(const TField& field)
    {
        val_ = field.length();
    }
private:
    std::size_t& val_;
};

TEST_CASE("Test70", "test70") 
{
    using Field = Test70_Field<>;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(!field.valid());
    REQUIRE(field.length() == 0U);
    REQUIRE(field.currentField() == std::tuple_size<Field::Members>::value);

    auto& mem1 = field.initField_mem1();
    std::get<1>(field.accessField_mem1().value()).value() = 0x0a0b;
    REQUIRE(std::get<1>(mem1.value()).value() == 0x0a0b);
    REQUIRE(field.currentField() == 0U);
    REQUIRE(field.length() == 3U);
    REQUIRE(field.valid());

    Field field2(field);
    REQUIRE(field2 == field);

    Field field3(std::move(field2));
    REQUIRE(field3 == field);

    auto& mem2 = field.initField_mem2();
    std::get<1>(field.accessField_mem2().value()).value() = 0x0c0c0c0c;
    REQUIRE(std::get<1>(mem2.value()).value() == 0x0c0c0c0c);
    REQUIRE(field.currentField() == 1U);
    REQUIRE(field.length() == 5U);
    REQUIRE(field.valid());

    field.reset();
    REQUIRE(!field.currentFieldValid());
    REQUIRE(!field.valid());
    REQUIRE(field.length() == 0U);
    REQUIRE(field.currentField() == std::tuple_size<Field::Members>::value);

    static const char Buf[] = {
        0x1, 0x2, 0x3
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.valid());
    REQUIRE(field.length() == 3U);
    REQUIRE(field.currentField() == 0U);

    static const char Buf2[] = {
        0x2, 0x3, 0x4
    };
    static const std::size_t Buf2Size = std::extent<decltype(Buf2)>::value;
    field = readWriteField<Field>(Buf2, Buf2Size, comms::ErrorStatus::NotEnoughData);
    REQUIRE(!field.valid());
    REQUIRE(field.length() == 0U);
    REQUIRE(field.currentField() == std::tuple_size<Field::Members>::value);

    static const char Buf3[] = {
        0x2, 0x3, 0x4, 0x5, 0x6
    };
    static const std::size_t Buf3Size = std::extent<decltype(Buf3)>::value;
    field = readWriteField<Field>(Buf3, Buf3Size);
    REQUIRE(field.valid());
    REQUIRE(field.length() == 5U);
    REQUIRE(field.currentField() == 1U);

    std::size_t len1 = 0U;
    field.currentFieldExec(Test70_LengthRetriever(len1));
    REQUIRE(field.length() == len1);

    std::size_t len2 = 0U;
    Test70_LengthRetriever lenRetriever(len2);
    field.currentFieldExec(lenRetriever);
    REQUIRE(len2 == len1);

    std::size_t len3 = 0U;
    static_cast<const Field&>(field).currentFieldExec(Test70_LengthRetriever(len3));
    REQUIRE(len3 == len1);

    field.initField_mem1();
    std::get<1>(field.accessField_mem1().value()).value() = 0x0a0b;
    REQUIRE(field3 == field);

    using InitialisedField = Test70_Field<comms::option::DefaultVariantIndex<0> >;
    InitialisedField iniField;
    REQUIRE(iniField.valid());
    REQUIRE(iniField.length() == 3U);
    REQUIRE(iniField.currentField() == 0);

    auto& iniMem1 = iniField.initField_mem1();
    REQUIRE(std::get<0>(iniMem1.value()).value() == 1U);
    REQUIRE(std::get<1>(iniMem1.value()).value() == 0U);
    REQUIRE(field.currentField() == 0U);
    REQUIRE(field.length() == 3U);
    REQUIRE(field.valid());

    std::size_t len4 = 0U;
    field.currentFieldExec(Test70_LengthRetriever(len4));
    REQUIRE(field.length() == len4);
}

struct Test71_Field : public
    comms::field::Bundle<
        comms::Field<comms::option::BigEndian>,
        std::tuple<
            comms::field::IntValue<
                comms::Field<comms::option::BigEndian>,
                std::uint8_t
            >,
            comms::field::Optional<
                comms::field::IntValue<
                    comms::Field<comms::option::BigEndian>,
                    std::uint8_t
                >,
                comms::option::DefaultOptionalMode<comms::field::OptionalMode::Missing>
            >
        >,
        comms::option::HasCustomRead,
        comms::option::HasCustomRefresh
    >
{
    COMMS_FIELD_MEMBERS_ACCESS_NOTEMPLATE(mask, val);

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        auto es = field_mask().read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        if (field_mask().value() == 0) {
            field_val().setMissing();
        }
        else {
            field_val().setExists();
        }

        len -= field_mask().length();
        return field_val().read(iter, len);
    }

    bool refresh()
    {
        bool exists = (field_mask().value() != 0);
        if (exists == field_val().doesExist()) {
            return false;
        }

        if (exists) {
            field_val().setExists();
        }
        else {
            field_val().setMissing();
        }
        return true;
    }
};

TEST_CASE("Test71", "test71") 
{
    using Field = Test71_Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.length() == 1U);
    REQUIRE(field.field_val().isMissing());

    field.field_mask().value() = 1;
    bool result = field.refresh();
    REQUIRE(result);
    REQUIRE(field.length() == 2U);
    REQUIRE(!field.refresh());
    field.field_mask().value() = 0;
    REQUIRE(field.refresh());
    REQUIRE(field.length() == 1U);

    static const char Buf[] = {
        0, 0, 0
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize, comms::ErrorStatus::Success);
    REQUIRE(field.length() == 1U);
    REQUIRE(field.field_val().isMissing());

    static const char Buf2[] = {
        1, 5, 0
    };
    static const std::size_t Buf2Size = std::extent<decltype(Buf2)>::value;

    field = readWriteField<Field>(Buf2, Buf2Size, comms::ErrorStatus::Success);
    REQUIRE(field.length() == 2U);
    REQUIRE(field.field_val().doesExist());
    REQUIRE(field.field_val().field().value() == (unsigned)Buf2[1]);
}

TEST_CASE("Test72", "test72") 
{
    static_assert(!comms::field::basic::details::stringHasPushBack<comms::util::StringView>(),
            "StringView doesn't have push_back");

    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t
    > SizeField;

    static_assert(!SizeField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceSizeFieldPrefix<SizeField>,
        comms::option::OrigDataView
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().empty());

    static const char Buf[] = {
        0x5, 'h', 'e', 'l', 'l', 'o', 'g', 'a', 'r'
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.value().size() == static_cast<std::size_t>(Buf[0]));
    REQUIRE(field.length() == field.value().size() + 1U);
    REQUIRE(field.valid());
    REQUIRE(&(*field.value().begin()) == &Buf[1]);

    static const std::string Str("blabla");
    field.value() = Field::ValueType(Str.c_str(), Str.size());
    REQUIRE(&(*field.value().begin()) == &Str[0]);

    static const char ExpectedBuf[] = {
        0x6, 'b', 'l', 'a', 'b', 'l', 'a'
    };
    static const std::size_t ExpectedBufSize =
            std::extent<decltype(ExpectedBuf)>::value;

    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test73", "test73") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ValidNumValueRange<0, 0>
    > TermField;

    static_assert(!TermField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceTerminationFieldSuffix<TermField>,
        comms::option::OrigDataView
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.length() == 1U);

    static const char* HelloStr = "hello";
    field.value() = HelloStr;
    REQUIRE(&(*field.value().begin()) == HelloStr);
    REQUIRE(field.length() == 6U);

    static const char ExpectedBuf[] = {
        'h', 'e', 'l', 'l', 'o', 0x0
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    static const char InputBuf[] = {
        'f', 'o', 'o', 0x0, 'b', 'l', 'a'
    };

    static const std::size_t InputBufSize = std::extent<decltype(InputBuf)>::value;

    auto* readIter = &InputBuf[0];
    auto es = field.read(readIter, InputBufSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(&(*field.value().begin()) == InputBuf);
    REQUIRE(field.value() == "foo");
    REQUIRE(field.value().size() == 3U);
    REQUIRE(std::distance(&InputBuf[0], readIter) == 4);
}

TEST_CASE("Test74", "test74") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ValidNumValueRange<0, 0>
    > TrailField;

    static_assert(!TrailField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceFixedSize<5>,
        comms::option::SequenceTrailingFieldSuffix<TrailField>,
        comms::option::OrigDataView
    > Field;

#if COMMS_HAS_CPP17_STRING_VIEW
    static_assert(std::is_same<Field::ValueType, std::string_view>::value, 
        "std::string_view is expected to be used for this field");    
#else // #if COMMS_HAS_CPP17_STRING_VIEW
    static_assert(std::is_same<Field::ValueType, comms::util::StringView>::value, 
        "comms::util::StringView is expected to be used for this field");          
#endif // #if COMMS_HAS_CPP17_STRING_VIEW

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 6U, "Invalid min length");
    static_assert(Field::maxLength() == 6U, "Invalid max length");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.length() == 6U);

    static const char* HelloStr = "hello";
    field.value() = HelloStr;
    REQUIRE(field.value().size() == 5U);
    REQUIRE(field.length() == 6U);
    REQUIRE(&(*field.value().begin()) == HelloStr);

    static const char ExpectedBuf[] = {
        'h', 'e', 'l', 'l', 'o', 0x0
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    field.value() = "foo";
    REQUIRE(field.value().size() == 3U);
    REQUIRE(std::string(field.value().data()) == "foo");
    REQUIRE(field.value() == Field::ValueType("foo"));
    REQUIRE(field.length() == 6U);

    static const char ExpectedBuf2[] = {
        'f', 'o', 'o', 0x0, 0x0, 0x0
    };
    static const std::size_t ExpectedBufSize2 = std::extent<decltype(ExpectedBuf2)>::value;
    writeReadField(field, ExpectedBuf2, ExpectedBufSize2);

    field = readWriteField<Field>(&ExpectedBuf2[0], ExpectedBufSize2);
    REQUIRE(field.value() == "foo");
}

TEST_CASE("Test75", "test75") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::OrigDataView
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

#if COMMS_HAS_CPP20_SPAN
    static_assert(std::is_same<Field::ValueType, std::span<std::uint8_t> >::value,
        "Expected to be std::span");    
#else
    static_assert(std::is_same<Field::ValueType, comms::util::ArrayView<std::uint8_t> >::value,
        "Expected to be comms::util::ArrayView");
#endif        


    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().empty());

    static const char Buf[] = {
        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == BufSize);
    REQUIRE(field.valid());

    auto& view = field.value();
    auto* viewStart = reinterpret_cast<const char*>(&(*view.begin()));
    REQUIRE(viewStart == &Buf[0]);
    REQUIRE(!field.refresh());
}

TEST_CASE("Test76", "test76") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::SequenceSizeFieldPrefix<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint16_t
            >
        >,
        comms::option::OrigDataView
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    REQUIRE(Field::minLength() == sizeof(std::uint16_t));

    Field field;
    REQUIRE(field.value().size() == 0U);
    REQUIRE(field.value().empty());

    static const char Buf[] = {
        0x0, 0xa, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xf, 0xf
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 12);
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == 10U);
    REQUIRE(&(*field.value().begin()) == reinterpret_cast<const std::uint8_t*>(&Buf[2]));

    field.value().remove_suffix(5);
    REQUIRE(field.valid());
    static const char ExpectedBuf[] = {
        0x0, 0x5, 0x0, 0x1, 0x2, 0x3, 0x4
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test77", "test77") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::SequenceFixedSize<6>,
        comms::option::OrigDataView
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 6U, "Invalid min length");
    static_assert(Field::maxLength() == 6U, "Invalid max length");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == 0U);
    REQUIRE(field.value().empty());

    static const char Buf[] = {
        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 6U);
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == 6U);
    REQUIRE((field.value())[0] == 0x0);
    REQUIRE((field.value())[1] == 0x1);
    REQUIRE(&(*field.value().begin()) == reinterpret_cast<const std::uint8_t*>(&Buf[0]));

    field.value().remove_prefix(3);
    REQUIRE(field.value().size() == 3U);
    REQUIRE((field.value())[0] == 0x3);
    REQUIRE((field.value())[1] == 0x4);
    REQUIRE(&(*field.value().begin()) == reinterpret_cast<const std::uint8_t*>(&Buf[3]));
    REQUIRE(field.length() == 6U);

    static const char ExpectedBuf[] = {
        0x3, 0x4, 0x5, 0x0, 0x0, 0x0
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeField(field, ExpectedBuf, ExpectedBufSize);
}

class Test78_Field : public
    comms::field::Variant<
        Test70_FieldBase,
        std::tuple<
            Test70_Mem1,
            Test70_Mem2
        >
    >
{
public:
    COMMS_VARIANT_MEMBERS_ACCESS_NOTEMPLATE(mem1, mem2);
};

TEST_CASE("Test78", "test78") 
{
    Test78_Field field;

    static_assert(!Test78_Field::isVersionDependent(),
        "Invalid version dependency assumption");

    auto& mem1_1 = field.initField_mem1();
    static_cast<void>(mem1_1);
    auto& mem1_2 = field.accessField_mem1();
    static_cast<void>(mem1_2);

    auto& mem2_1 = field.initField_mem2();
    static_cast<void>(mem2_1);
    auto& mem2_2 = field.accessField_mem2();
    static_cast<void>(mem2_2);
}

TEST_CASE("Test79", "test79") 
{
    class Field : public
        comms::field::ArrayList<
            comms::Field<BigEndianOpt>,
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t
            >,
            comms::option::SequenceElemLengthForcingEnabled,
            comms::option::SequenceFixedSize<3>
        >
    {
    public:
        Field()
        {
            forceReadElemLength(2U);
        }
    };

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    Field field;
    REQUIRE(field.valid());
    static_assert(Field::minLength() == 3U, "Min length is incorrect");
    static_assert(3U < Field::maxLength(), "Max length is incorrect");

    static const char Buf[] = {
        0x1, 0x0, 0x2, 0x0, 0x3, 0x0, 0x4, 0x0, 0x5, 0x0, 0x6, 0x0, 0x7, 0x0, 0x8, 0x0
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto readIter = &Buf[0];
    auto es = field.read(readIter, BufSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(field.length() == 6U);
    REQUIRE(field.value().size() == 3U);
    REQUIRE(field.valid());
    REQUIRE(field.value()[0].value() == 0x1);
    REQUIRE(field.value()[1].value() == 0x2);
    REQUIRE(field.value()[2].value() == 0x3);
}

TEST_CASE("Test80", "test80") 
{
    typedef comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint16_t,
                comms::option::ValidNumValueRange<0, 10>,
                comms::option::DefaultNumValue<5>
            >,
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t,
                comms::option::ValidNumValueRange<100, 100>,
                comms::option::DefaultNumValue<100>,
                comms::option::EmptySerialization
            >,
            comms::field::EnumValue<
                comms::Field<BigEndianOpt>,
                Enum1,
                comms::option::FixedLength<1>,
                comms::option::ValidNumValueRange<0, Enum1_NumOfValues - 1>,
                comms::option::DefaultNumValue<Enum1_Value2>
            >
        >
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 3U, "Invalid minLength");
    static_assert(Field::maxLength() == 3U, "Invalid maxLength");
    static_assert(Field::minLengthFromUntil<1, 2>() == 0U, "Invalid minLength");
    static_assert(Field::maxLengthFromUntil<1, 2>() == 0U, "Invalid maxLength");
    static_assert(Field::minLengthFrom<1>() == 1U, "Invalid minLength");
    static_assert(Field::maxLengthFrom<1>() == 1U, "Invalid maxLength");

    Field field;
    REQUIRE(field.valid());
    auto& intValField = std::get<0>(field.value());
    auto& constValField = std::get<1>(field.value());
    auto& enumValField = std::get<2>(field.value());
    REQUIRE(intValField.value() == 5U);
    REQUIRE(constValField.value() == 100U);
    REQUIRE(enumValField.value() == Enum1_Value2);

    intValField.value() = 50U;
    REQUIRE(!field.valid());
    intValField.value() = 1U;
    REQUIRE(field.valid());
    enumValField.value() = Enum1_NumOfValues;
    REQUIRE(!field.valid());
    enumValField.value() = Enum1_Value1;
    REQUIRE(field.valid());
    constValField.value() = 10;
    REQUIRE(!field.valid());
    constValField.value() = 100;
    REQUIRE(field.valid());

    static const char Buf[] = {
        0x00, 0x3, Enum1_Value3, (char)0xff
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 3U);
    REQUIRE(field.valid());
    REQUIRE(intValField.value() == 3U);
    REQUIRE(constValField.value() == 100U);
    REQUIRE(enumValField.value() == Enum1_Value3);

    intValField.value() = 0xabcd;
    enumValField.value() = Enum1_Value1;

    static const char ExpectedBuf[] = {
        (char)0xab, (char)0xcd, (char)Enum1_Value1
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test81", "test81") 
{
    using Field = comms::field::IntValue<
            comms::Field<comms::option::BigEndian>,
            std::uint64_t,
            comms::option::ValidBigUnsignedNumValueRange<0xffffffff, std::numeric_limits<std::uintmax_t>::max() - 1>,
            comms::option::DefaultBigUnsignedNumValue<std::numeric_limits<std::uintmax_t>::max()>
        >;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(!field.valid());
    REQUIRE(field.value() == std::numeric_limits<std::uintmax_t>::max());
}

TEST_CASE("Test82", "test82") 
{

    typedef comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint16_t,
                comms::option::ValidNumValueRange<0, 10>,
                comms::option::DefaultNumValue<5>
            >
        >
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 2U, "Invalid minLength");
    static_assert(Field::minLengthFrom<0>() == 2U, "Invalid minLength");
    static_assert(Field::minLengthUntil<1>() == 2U, "Invalid minLength");
    static_assert(Field::maxLength() == 2U, "Invalid maxLength");
    static_assert(Field::maxLengthFrom<0>() == 2U, "Invalid minLength");
    static_assert(Field::maxLengthUntil<1>() == 2U, "Invalid minLength");

    Field field;
    REQUIRE(field.length() == 2U);
    REQUIRE(field.lengthFrom<0>() == 2U);
    REQUIRE(field.lengthUntil<1>() == 2U);
    REQUIRE(field.valid());
    auto& intValField = std::get<0>(field.value());
    REQUIRE(intValField.value() == 5U);

    intValField.value() = 50U;
    REQUIRE(!field.valid());
    intValField.value() = 1U;
    REQUIRE(field.valid());
    static const char Buf[] = {
        0x00, 0x3, (char)0xff
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 2U);
    REQUIRE(field.valid());
    REQUIRE(intValField.value() == 3U);

    intValField.value() = 0xabcd;

    static const char ExpectedBuf[] = {
        (char)0xab, (char)0xcd
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    Field fieldTmp;
    auto readIter = &ExpectedBuf[0];
    auto es = fieldTmp.readFromUntil<0, 1>(readIter, ExpectedBufSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(fieldTmp == field);
}

TEST_CASE("Test83", "test83") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::SequenceFixedSize<5>,
        comms::option::SequenceFixedSizeUseFixedSizeStorage
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 5U, "Invalid min length");
    static_assert(Field::maxLength() == 5U, "Invalid max length");

    static_assert(comms::util::isStaticVector<Field::ValueType>(), "The storage typ is incorrect");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(Field::minLength() == 5U);
    REQUIRE(Field::maxLength() == 5U);

    static const char Buf[] = {
        0x0, 0x1, 0x2, 0x3, 0x4
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == BufSize);
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == BufSize);
}

TEST_CASE("Test84", "test84") 
{
    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceFixedSize<5>,
        comms::option::SequenceFixedSizeUseFixedSizeStorage
    > Field;

    static_assert(Field::minLength() == 5U, "Invalid min length");
    static_assert(Field::maxLength() == 5U, "Invalid max length");
    static_assert(comms::util::isStaticString<Field::ValueType>(), "Invalid storage type");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.length() == 5U);

    static const char* HelloStr = "hello";
    field.value() = HelloStr;
    REQUIRE(field.value().size() == 5U);
    REQUIRE(field.length() == 5U);
    REQUIRE(std::string(&(*field.value().begin())) == HelloStr);

    static const char ExpectedBuf[] = {
        'h', 'e', 'l', 'l', 'o'
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    field.value() = "foo";
    REQUIRE(field.value().size() == 3U);
    REQUIRE(std::string(field.value().data()) == "foo");
    REQUIRE(field.length() == 5U);

    static const char ExpectedBuf2[] = {
        'f', 'o', 'o', 0x0, 0x0
    };
    static const std::size_t ExpectedBufSize2 = std::extent<decltype(ExpectedBuf2)>::value;
    writeReadField(field, ExpectedBuf2, ExpectedBufSize2);

    field = readWriteField<Field>(&ExpectedBuf2[0], ExpectedBufSize2);
    REQUIRE(field.value() == "foo");
}

TEST_CASE("Test85", "test85") 
{
    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceFixedSize<5>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 5U, "Invalid min length");
    static_assert(Field::maxLength() == 5U, "Invalid max length");

    Field field;

    field.value() = "blabla";
    REQUIRE(field.value().size() == 6U);
    REQUIRE(field.length() == 5U);

    static const char ExpectedBuf[] = {
        'b', 'l', 'a', 'b', 'l'
    };
    static const std::size_t ExpectedBufSize3 = std::extent<decltype(ExpectedBuf)>::value;

    std::vector<std::uint8_t> outBuf;
    auto writeIter = std::back_inserter(outBuf);
    auto es = field.write(writeIter, outBuf.max_size());
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(outBuf.size() == ExpectedBufSize3);
    REQUIRE(std::equal(outBuf.begin(), outBuf.end(), std::begin(ExpectedBuf)));
}

TEST_CASE("Test86", "test86") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ValidNumValueRangeOverride<0, 10>,
        comms::option::ValidNumValueRange<20, 30>,
        comms::option::DefaultNumValue<20>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.value() == 20);
    REQUIRE(!field.valid());
    field.value() = 5U;
    REQUIRE(field.valid());
}

TEST_CASE("Test87", "test87") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t,
            comms::option::ValidNumValueRange<0, 5>
        >,
        comms::option::SequenceSizeFieldPrefix<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint16_t
            >
        >,
        comms::option::SequenceElemSerLengthFieldPrefix<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t
            >
        >
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    REQUIRE(Field::minLength() == sizeof(std::uint16_t));

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == 0U);

    static const char Buf[] = {
        0x0, 0x4, 0x1, 0x0, 0x1, 0x1, 0x1, 0x2, 0x1, 0x3
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == BufSize);
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == 4U);

    field.value().resize(5);
    static const char ExpectedBuf[] = {
        0x0, 0x5, 0x1, 0x0, 0x1, 0x1, 0x1, 0x2, 0x1, 0x3, 0x1, 0x0
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    REQUIRE(field.valid());
    writeReadField(field, ExpectedBuf, ExpectedBufSize);


    static const char Buf2[] = {
        0x0, 0x4, 0x2, 0x0, 0x1, 0x2, 0x3, 0x4, 0x2, 0x5, 0x6, 0x2, 0x7, 0x8
    };
    static const std::size_t Buf2Size = std::extent<decltype(Buf2)>::value;
    auto readIter = &Buf2[0];
    auto es = field.read(readIter, Buf2Size);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(field.length() == Buf2Size - 4U);
    REQUIRE(!field.valid());
    REQUIRE(field.value().size() == 4U);
    REQUIRE(field.value()[0].value() == 0x0);
    REQUIRE(field.value()[1].value() == 0x3);
    REQUIRE(field.value()[2].value() == 0x5);
    REQUIRE(field.value()[3].value() == 0x7);
}

TEST_CASE("Test88", "test88") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::Bundle<
            comms::Field<BigEndianOpt>,
            std::tuple<
                comms::field::IntValue<
                    comms::Field<BigEndianOpt>,
                    std::uint8_t
                >,
                comms::field::String<
                    comms::Field<BigEndianOpt>,
                    comms::option::SequenceSizeFieldPrefix<
                        comms::field::IntValue<
                            comms::Field<BigEndianOpt>,
                            std::uint8_t
                        >
                    >
                >
            >
        >,
        comms::option::SequenceSizeFieldPrefix<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t
            >
        >,
        comms::option::SequenceElemSerLengthFieldPrefix<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint32_t,
                comms::option::VarLength<1, 4>
            >
        >
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    REQUIRE(Field::minLength() == sizeof(std::uint8_t));

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().size() ==  0U);

    static const char Buf[] = {
        0x2,
        0x9, 0x1, 0x5, 'h', 'e', 'l', 'l', 'o', 0xa, 0xb,
        0x7, 0x2, 0x3, 'b', 'l', 'a', 0xc, 0xd
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto readIter = &Buf[0];
    auto es = field.read(readIter, BufSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    auto& vec = field.value();
    REQUIRE(vec.size() == 2U);
    auto& bundle0 = vec[0];
    auto& bundle1 = vec[1];
    REQUIRE(std::get<0>(bundle0.value()).value() == 1U);
    REQUIRE(std::get<1>(bundle0.value()).value() == "hello");
    REQUIRE(std::get<0>(bundle1.value()).value() == 2U);
    REQUIRE(std::get<1>(bundle1.value()).value() == "bla");
    REQUIRE(readIter == &Buf[0] + BufSize);

    static const char ExpectedBuf[] = {
        0x2,
        0x7, 0x1, 0x5, 'h', 'e', 'l', 'l', 'o',
        0x5, 0x2, 0x3, 'b', 'l', 'a'
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    field.value().resize(1);
    auto& intField = std::get<0>(field.value()[0].value());
    intField.value() = 4U;
    auto& stringField = std::get<1>(field.value()[0].value());
    stringField.value().clear();
    for (auto idx = 0; idx < 128; ++idx) {
        stringField.value().push_back('a');
    }

    std::vector<char> expBuf;
    expBuf.push_back(0x1); // count
    expBuf.push_back((char)0x81); // high byte of length
    expBuf.push_back(0x02); // low byte of length
    expBuf.push_back(0x4); // value of first integral byte
    expBuf.push_back((char)128); // length of string
    for (auto idx = 0; idx < 128; ++idx) {
        expBuf.push_back('a'); // string itself
    }
    writeReadField(field, &expBuf[0], expBuf.size());
}

TEST_CASE("Test89", "test89") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::Bundle<
            comms::Field<LittleEndianOpt>,
            std::tuple<
                comms::field::IntValue<
                    comms::Field<LittleEndianOpt>,
                    std::uint32_t,
                    comms::option::VarLength<1, 4>
                >,
                comms::field::String<
                    comms::Field<LittleEndianOpt>,
                    comms::option::SequenceSizeFieldPrefix<
                        comms::field::IntValue<
                            comms::Field<LittleEndianOpt>,
                            std::uint16_t,
                            comms::option::VarLength<1, 2>
                        >
                    >
                >
            >
        >,
        comms::option::SequenceSerLengthFieldPrefix<
            comms::field::IntValue<
                comms::Field<LittleEndianOpt>,
                std::uint32_t,
                comms::option::VarLength<1, 4>
            >
        >,
        comms::option::SequenceElemSerLengthFieldPrefix<
            comms::field::IntValue<
                comms::Field<LittleEndianOpt>,
                std::uint32_t,
                comms::option::VarLength<1, 4>
            >
        >
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    REQUIRE(Field::minLength() == sizeof(std::uint8_t));

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == 0U);

    static const char Buf[] = {
        18,
        0x9, 0x1, 0x5, 'h', 'e', 'l', 'l', 'o', 0xa, 0xb,
        0x7, 0x2, 0x3, 'b', 'l', 'a', 0xc, 0xd
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto readIter = &Buf[0];
    auto es = field.read(readIter, BufSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    auto& vec = field.value();
    REQUIRE(vec.size() == 2U);
    auto& bundle0 = vec[0];
    auto& bundle1 = vec[1];
    REQUIRE(std::get<0>(bundle0.value()).value() == 1U);
    REQUIRE(std::get<1>(bundle0.value()).value() == "hello");
    REQUIRE(std::get<0>(bundle1.value()).value() == 2U);
    REQUIRE(std::get<1>(bundle1.value()).value() == "bla");

    static const char ExpectedBuf[] = {
        14,
        0x7, 0x1, 0x5, 'h', 'e', 'l', 'l', 'o',
        0x5, 0x2, 0x3, 'b', 'l', 'a'
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    field.value().resize(1);
    auto& intField = std::get<0>(field.value()[0].value());
    intField.value() = 0x4000;
    auto& stringField = std::get<1>(field.value()[0].value());
    stringField.value().clear();
    for (auto idx = 0; idx < 128; ++idx) {
        stringField.value().push_back('a');
    }

    auto expTotalLength = 2U + 2 + 3 + 2 + 128;
    REQUIRE(field.length() == expTotalLength);

    std::vector<char> expBuf;
    static const std::vector<char> totalLenEncoding = { (char)0x87, 0x1 };
    static const std::vector<char> elemLenEncoding = { (char)0x85, 0x1 };
    static const std::vector<char> intEncoding = { (char)0x80, (char)0x80, 0x1 };
    static const std::vector<char> stringLenEncoding = { (char)0x80, 0x1 };


    expBuf.insert(expBuf.end(), totalLenEncoding.begin(), totalLenEncoding.end());
    expBuf.insert(expBuf.end(), elemLenEncoding.begin(), elemLenEncoding.end());
    expBuf.insert(expBuf.end(), intEncoding.begin(), intEncoding.end());
    expBuf.insert(expBuf.end(), stringLenEncoding.begin(), stringLenEncoding.end());

    for (auto idx = 0; idx < 128; ++idx) {
        expBuf.push_back('a'); // string itself
    }
    writeReadField(field, &expBuf[0], expBuf.size());
}

TEST_CASE("Test90", "test90") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::Bundle<
            comms::Field<BigEndianOpt>,
            std::tuple<
                comms::field::IntValue<
                    comms::Field<BigEndianOpt>,
                    std::uint8_t
                >,
                comms::field::IntValue<
                    comms::Field<BigEndianOpt>,
                    std::uint16_t
                >
            >
        >,
        comms::option::SequenceSizeFieldPrefix<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t
            >
        >,
        comms::option::SequenceElemFixedSerLengthFieldPrefix<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint32_t,
                comms::option::VarLength<1, 4>
            >
        >
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 1U, "Invalid min length");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == 0U);

    static const char Buf[] = {
        0x2, 0x4,
        0x1, 0x2, 0x3, 0x4,
        0x5, 0x6, 0x7, 0x8
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto readIter = &Buf[0];
    auto es = field.read(readIter, BufSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    auto& vec = field.value();
    REQUIRE(vec.size() == 2U);
    auto& bundle0 = vec[0];
    auto& bundle1 = vec[1];
    REQUIRE(std::get<0>(bundle0.value()).value() == 0x1);
    REQUIRE(std::get<1>(bundle0.value()).value() == 0x0203);
    REQUIRE(std::get<0>(bundle1.value()).value() == 0x5);
    REQUIRE(std::get<1>(bundle1.value()).value() == 0x0607);

    static const char ExpectedBuf[] = {
        0x2, 0x3,
        0x1, 0x2, 0x3,
        0x5, 0x6, 0x7
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);

    field.value().clear();
    static const char EmptyExpectedBuf[] = {0x0};
    static const std::size_t EmptyExpectedBufSize = std::extent<decltype(EmptyExpectedBuf)>::value;

    writeReadField(field, EmptyExpectedBuf, EmptyExpectedBufSize);
    REQUIRE(field.length() == 1U);
}

TEST_CASE("Test91", "test91") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::Bundle<
            comms::Field<BigEndianOpt>,
            std::tuple<
                comms::field::IntValue<
                    comms::Field<BigEndianOpt>,
                    std::uint8_t
                >,
                comms::field::IntValue<
                    comms::Field<BigEndianOpt>,
                    std::uint16_t
                >
            >
        >,
        comms::option::SequenceFixedSize<2>,
        comms::option::SequenceElemFixedSerLengthFieldPrefix<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint32_t,
                comms::option::VarLength<1, 4>
            >
        >
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 7U, "Invalid min length");

    Field field;
    REQUIRE(field.valid());
    REQUIRE(field.value().size() == 0U);

    static const char Buf[] = {
        0x4,
        0x1, 0x2, 0x3, 0x4,
        0x5, 0x6, 0x7, 0x8
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto readIter = &Buf[0];
    auto es = field.read(readIter, BufSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    auto& vec = field.value();
    REQUIRE(vec.size() == 2U);
    auto& bundle0 = vec[0];
    auto& bundle1 = vec[1];
    REQUIRE(std::get<0>(bundle0.value()).value() == 0x1);
    REQUIRE(std::get<1>(bundle0.value()).value() == 0x0203);
    REQUIRE(std::get<0>(bundle1.value()).value() == 0x5);
    REQUIRE(std::get<1>(bundle1.value()).value() == 0x0607);

    static const char ExpectedBuf[] = {
        0x3,
        0x1, 0x2, 0x3,
        0x5, 0x6, 0x7
    };
    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test92", "test92") 
{
    typedef std::tuple<
        comms::field::IntValue<
            comms::Field<LittleEndianOpt>,
            std::uint8_t
        >,
        comms::field::IntValue<
                comms::Field<LittleEndianOpt>,
                std::uint8_t
        >,
        comms::field::IntValue<
            comms::Field<LittleEndianOpt>,
            std::uint8_t
        >
    > BitfileMembers;

    typedef comms::field::Bitfield<
        comms::Field<LittleEndianOpt>,
        BitfileMembers
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(!field.setVersion(5U));
    static_cast<void>(field);
    REQUIRE(field.length() == 3U);
    REQUIRE(field.memberBitLength<0>() == 8U);
    REQUIRE(field.memberBitLength<1>() == 8U);
    REQUIRE(field.memberBitLength<2>() == 8U);

    static const char Buf[] = {
        (char)0x1, (char)0x2, (char)0x3
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize);
    auto& members = field.value();
    auto& mem1 = std::get<0>(members);
    REQUIRE(mem1.value() == 0x1);

    auto& mem2 = std::get<1>(members);
    REQUIRE(mem2.value() == 0x2);

    auto& mem3 = std::get<2>(members);
    REQUIRE(mem3.value() == 0x3);
}

TEST_CASE("Test93", "test93") 
{
    typedef std::tuple<
        comms::field::IntValue<
            comms::Field<LittleEndianOpt>,
            std::uint8_t,
            comms::option::FixedBitLength<4>,
            comms::option::DefaultNumValue<0xf>
        >,
        comms::field::IntValue<
            comms::Field<LittleEndianOpt>,
            std::int16_t,
            comms::option::DefaultNumValue<2016>,
            comms::option::NumValueSerOffset<-2000>,
            comms::option::FixedBitLength<8>
        >,
        comms::field::IntValue<
            comms::Field<LittleEndianOpt>,
            std::uint16_t,
            comms::option::FixedBitLength<12>,
            comms::option::DefaultNumValue<0x801>
        >
    > BitfileMembers;

    typedef comms::field::Bitfield<
        comms::Field<LittleEndianOpt>,
        BitfileMembers
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    static_cast<void>(field);
    REQUIRE(field.length() == 3U);
    REQUIRE(field.memberBitLength<0>() == 4U);
    REQUIRE(field.memberBitLength<1>() == 8U);
    REQUIRE(field.memberBitLength<2>() == 12U);

    auto& members = field.value();
    auto& mem1 = std::get<0>(members);
    REQUIRE(mem1.value() == 0xf);
    auto& mem2 = std::get<1>(members);
    REQUIRE(mem2.value() == 2016);
    auto& mem3 = std::get<2>(members);
    REQUIRE(mem3.value() == 0x801);

    static const char ExpectedBuf[] = {
        0x0f, 0x11, (char)0x80
    };

    const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

TEST_CASE("Test94", "test94") 
{
    using Mem1 =
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint16_t
        >;

    struct Mem2 : public
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint16_t,
            comms::option::HasCustomVersionUpdate
        >
    {
        bool setVersion(unsigned)
        {
            return true;
        }
    };

    typedef comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            Mem1,
            Mem2
        >
    > Field;

    static_assert(Field::isVersionDependent(),
        "Invalid version dependency assumption");
    Field field;
    REQUIRE(field.setVersion(5U));
}

TEST_CASE("Test95", "test95") 
{
    using Mem1 =
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint16_t
        >;

    using Mem2 =
        comms::field::Optional<
            Mem1,
            comms::option::ExistsSinceVersion<5>,
            comms::option::ExistsByDefault
        >;

    typedef comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            Mem1,
            Mem2
        >
    > Field;

    static_assert(Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    REQUIRE(field.length() == 4U);
    REQUIRE(!field.setVersion(5U));
    REQUIRE(field.length() == 4U);
    REQUIRE(field.setVersion(4U));
    REQUIRE(field.length() == 2U);
    REQUIRE(field.setVersion(15U));
    REQUIRE(field.length() == 4U);
}

TEST_CASE("Test96", "test96") 
{
    using Mem1 =
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t,
            comms::option::FixedBitLength<4>
        >;

    struct Mem2 : public
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t,
            comms::option::HasCustomVersionUpdate,
            comms::option::FixedBitLength<4>
        >
    {
        bool setVersion(unsigned)
        {
            return true;
        }
    };

    typedef comms::field::Bitfield<
        comms::Field<BigEndianOpt>,
        std::tuple<
            Mem1,
            Mem2
        >
    > Field;

    static_assert(Field::isVersionDependent(),
        "Invalid version dependency assumption");
    Field field;
    REQUIRE(field.setVersion(5U));
}

TEST_CASE("Test97", "test97") 
{
    using Mem1 =
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint16_t
        >;

    using Mem2 =
        comms::field::Optional<
            Mem1,
            comms::option::ExistsSinceVersion<5>,
            comms::option::ExistsByDefault
        >;

    using ListElem = comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            Mem1,
            Mem2
        >
    >;

    static_assert(ListElem::isVersionDependent(),
        "Invalid version dependency assumption");

    using Field =
        comms::field::ArrayList<
            comms::Field<BigEndianOpt>,
            ListElem
        >;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    static_assert(Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    field.value().resize(1);
    REQUIRE(field.length() == 4U);
    REQUIRE(field.setVersion(1U));
    REQUIRE(field.length() == 2U);

    do {
        static const char Buf1[] = {
            (char)0x01, (char)0x02
        };
        static const std::size_t Buf1Size = std::extent<decltype(Buf1)>::value;

        auto readIter = &Buf1[0];
        auto es = field.read(readIter, Buf1Size);
        REQUIRE(es == comms::ErrorStatus::Success);
        REQUIRE(field.value().size() == 1U);
        auto& members = field.value()[0].value();
        auto& mem1 = std::get<0>(members);
        auto& mem2 = std::get<1>(members);
        REQUIRE(mem1.value() == 0x102);
        REQUIRE(mem2.isMissing());

        REQUIRE(field.setVersion(15U));
        REQUIRE(mem2.doesExist());
        REQUIRE(field.length() == 4U);
    } while (false);

    do {
        static const char Buf2[] = {
            (char)0x03, (char)0x04, (char)0x05, (char)0x06
        };
        static const std::size_t Buf2Size = std::extent<decltype(Buf2)>::value;
        auto readIter = &Buf2[0];
        auto es = field.read(readIter, Buf2Size);
        REQUIRE(es == comms::ErrorStatus::Success);
        REQUIRE(field.value().size() == 1U);
        auto& members = field.value()[0].value();
        auto& mem1 = std::get<0>(members);
        auto& mem2 = std::get<1>(members);
        REQUIRE(field.length() == 4U);
        REQUIRE(mem2.doesExist());
        REQUIRE(mem1.value() == 0x304);
        REQUIRE(mem2.field().value() == 0x506);
    } while (false);
}

TEST_CASE("Test98", "test98") 
{
    using Field =
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t,
            comms::option::InvalidByDefault,
            comms::option::VersionStorage
        >;

    Field field;
    REQUIRE(!field.valid());
    REQUIRE(field.getVersion() == 0U);
    REQUIRE(field.setVersion(5U));
    REQUIRE(field.getVersion() == 5U);

    using Field2 =
        comms::field::BitmaskValue<
            comms::Field<BigEndianOpt>,
            comms::option::FixedLength<1U>,
            comms::option::DefaultNumValue<0x6U>,
            comms::option::VersionStorage,
            comms::option::BitmaskReservedBits<0xc2U, 0x2U>
        >;

    Field2 field2;
    REQUIRE(field2.getVersion() == 0U);
    REQUIRE(field2.setVersion(5U));
    REQUIRE(field2.getVersion() == 5U);
}

TEST_CASE("Test99", "test99") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::SequenceLengthForcingEnabled
    > Field1;

    static_assert(comms::field::isArrayList<Field1>(), "Bad field type detection");
    static_assert(!Field1::isVersionDependent(),
        "Invalid version dependency assumption");

    Field1 field1;
    REQUIRE(field1.valid());

    field1.forceReadLength(4U);

    static const char Buf[] = {
        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto readIter = &Buf[0];
    auto es = field1.read(readIter, BufSize);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(field1.value().size() == 4U);
    REQUIRE(field1.length() == 4U);
    REQUIRE(field1.valid());
    field1.clearReadLengthForcing();

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceLengthForcingEnabled
    > Field2;

    static_assert(!Field2::isVersionDependent(),
        "Invalid version dependency assumption");

    Field2 field2;
    REQUIRE(field2.valid());

    field2.forceReadLength(5U);

    static const char Buf2[] = {
        'h', 'e', 'l', 'l', 'o', 'a', 'b', 'c', 'd'
    };
    static const std::size_t Buf2Size = std::extent<decltype(Buf)>::value;
    auto readIter2 = &Buf2[0];
    es = field2.read(readIter2, Buf2Size);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(field2.value() == "hello");
    REQUIRE(field2.valid());
    field2.clearReadLengthForcing();
}

TEST_CASE("Test100", "test100") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::int64_t,
        comms::option::FixedLength<5U, false>,
        comms::option::NumValueSerOffset<0x492559f64fLL>,
        comms::option::ScalingRatio<1, 0x174878e800LL>
    > Field;

    Field field;

    static const char Buf[] = {
        (char)0x87, (char)0x54, (char)0xa2, (char)0x03, (char)0xb9
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    field = readWriteField<Field>(Buf, BufSize);

    REQUIRE(std::abs(field.getScaled<double>() - 2.67) <= 0.1);
}

TEST_CASE("Test101", "test101") 
{
    typedef comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            comms::field::IntValue<comms::Field<BigEndianOpt>, std::uint16_t>,
            comms::field::IntValue<comms::Field<BigEndianOpt>, std::uint32_t, comms::option::FixedLength<3> >,
            comms::field::IntValue<comms::Field<BigEndianOpt>, std::uint8_t>
        >
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 6U, "Invalid minLength");
    static_assert(Field::minLengthFrom<2>() == 1U, "Invalid minLength");
    static_assert(Field::minLengthUntil<1>() == 2U, "Invalid minLength");
    static_assert(Field::minLengthFromUntil<1, 2>() == 3U, "Invalid minLength");
    static_assert(Field::maxLength() == 6U, "Invalid maxLength");
    static_assert(Field::maxLengthFrom<2>() == 1U, "Invalid maxLength");
    static_assert(Field::maxLengthUntil<1>() == 2U, "Invalid maxLength");
    static_assert(Field::maxLengthFromUntil<1, 2>() == 3U, "Invalid maxLength");

    Field field;
    static const char Buf[] = {
        0x1, 0x2, 0x3, 0x4, 0x5, 0x6
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

    auto iter = &Buf[0];
    auto len = BufSize;

    REQUIRE(len == 6U);

    auto es = field.readUntilAndUpdateLen<1>(iter, len);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(std::get<0>(field.value()).value() == 0x102);
    REQUIRE(len == 4U);

    es = field.readFromUntilAndUpdateLen<1, 2>(iter, len);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(std::get<1>(field.value()).value() == 0x30405);
    REQUIRE(len == 1U);    

    es = field.readFromAndUpdateLen<2>(iter, len);
    REQUIRE(es == comms::ErrorStatus::Success);
    REQUIRE(std::get<2>(field.value()).value() == 0x6);
    REQUIRE(len == 0U);    
}

TEST_CASE("Test102", "test102") 
{
    enum class EnumVal : std::uint32_t
    {
        V1 = 0,
        V2 = 128
    };

    typedef comms::field::EnumValue<
        comms::Field<LittleEndianOpt>,
        EnumVal,
        comms::option::VarLength<1, 2>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static const char Buf[] = {
        (char)0x80, 0x01
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    auto field = readWriteField<Field>(Buf, BufSize);
    REQUIRE(field.length() == 2U);
    REQUIRE(field.value() == EnumVal::V2);
    REQUIRE(field.valid());

    field.value() = EnumVal::V1;
    REQUIRE(field.length() == 1U);
    static const char ExpectedBuf[] = {
        (char)0x0
    };

    static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
    writeReadField(field, ExpectedBuf, ExpectedBufSize);
}

template <typename... TExtraOpts>
class Test103_Field : public
    comms::field::BitmaskValue<
        comms::Field<comms::option::BigEndian>,
        comms::option::FixedLength<1>,
        TExtraOpts...
    >
{
    using Base =
        comms::field::BitmaskValue<
            comms::Field<comms::option::BigEndian>,
            comms::option::FixedLength<1>,
            TExtraOpts...
        >;
public:
    COMMS_BITMASK_BITS_SEQ(B0, B1, B2);
};


TEST_CASE("Test103", "test103") 
{
    using Field = Test103_Field<>;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    Field field;
    field.setBitValue_B1(true);
    REQUIRE(field.value() == 0x2);
    field.setBitValue_B0(true);
    REQUIRE(field.value() == 0x3);
}

TEST_CASE("Test104", "test104") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::uint8_t,
        comms::option::ValidNumValueRange<0, 0>
    > TermField;

    static_assert(!TermField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceTerminationFieldSuffix<TermField>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static const char Buf[] = {
        'h', 'e', 'l', 'l', 'o'
    };

    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    readWriteField<Field>(&Buf[0], BufSize, comms::ErrorStatus::NotEnoughData);
}

TEST_CASE("Test105", "test105") 
{
    struct LenField : public
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t
        >
    {
    };

    static_assert(!LenField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::IntValue<comms::Field<BigEndianOpt>, std::uint16_t>,
        comms::option::SequenceSerLengthFieldPrefix<LenField>
    > Field;

    static_assert(comms::field::isArrayList<Field>(), "Bad field type detection");
    
    static const char Buf[] = {
        0x8, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    readWriteField<Field>(&Buf[0], BufSize, comms::ErrorStatus::InvalidMsgData);
}

TEST_CASE("Test106", "test106") 
{
    typedef comms::field::IntValue<
        comms::Field<BigEndianOpt>,
        std::int32_t,
        comms::option::VarLength<1U, 4U>
    > Field;

    Field field;
    do {
        field.value() = 0x5f;
        REQUIRE(field.length() == 2U);

        static const char Buf[] = {
            0x5f
        };
        static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
        field = readWriteField<Field>(&Buf[0], BufSize);
        REQUIRE(field.length() == 1U);
        REQUIRE(field.value() == -33);
    } while (false);

    do {
        field.value() = -1;
        REQUIRE(field.length() == 1U);

        static const char ExpectedBuf[] = {
            0x7f
        };
        static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
        writeReadField(field, &ExpectedBuf[0], ExpectedBufSize);
    } while(false);

    do {
        field.value() = 0;
        REQUIRE(field.length() == 1U);

        static const char ExpectedBuf[] = {
            0x0
        };
        static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
        writeReadField(field, &ExpectedBuf[0], ExpectedBufSize);
    } while(false);
}

TEST_CASE("Test107", "test107") 
{
    struct LenField : public
        comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint8_t
        >
    {
    };

    static_assert(!LenField::isVersionDependent(),
        "Invalid version dependency assumption");

    typedef comms::field::String<
        comms::Field<BigEndianOpt>,
        comms::option::SequenceSerLengthFieldPrefix<LenField>,
        comms::option::FixedSizeStorage<32>
    > Field;

    static_assert(comms::field::isString<Field>(), "Bad field type detection");
    
    static const char Buf[] = {
        0x5, 'h', 'e', 'l', 'l', 'o'
    };
    static const std::size_t BufSize = std::extent<decltype(Buf)>::value;
    Field field = readWriteField<Field>(&Buf[0], BufSize);
    REQUIRE(field.value() == "hello");
}

TEST_CASE("Test108", "test108") 
{

    typedef comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t
            >,
            comms::field::String<comms::Field<BigEndianOpt> >
        >,
        comms::option::RemLengthMemberField<0>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 1U, "Invalid minLength");

    do {
        Field field;
        REQUIRE(field.length() == 1U);
        REQUIRE(std::get<0>(field.value()).value() == 0U);

        auto& strMem = std::get<1>(field.value());
        strMem.value() = "aaa";
        REQUIRE(strMem.length() == 3U);
        REQUIRE(field.refresh());

        static const char ExpectedBuf[] = {
            0x3, 'a', 'a', 'a'
        };
        static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
        writeReadField(field, ExpectedBuf, ExpectedBufSize);   
    } while (false); 

    do {
        static const char Buf[] = {
            0x05, 'h', 'e', 'l', 'l', 'o', '0', '1'
        };

        static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

        auto field = readWriteField<Field>(Buf, BufSize);
        auto& strMem = std::get<1>(field.value());
        REQUIRE(field.length() == 6U);
        REQUIRE(strMem.value() == "hello");
        REQUIRE(field.valid());
    } while (false);

    do {
        static const char Buf[] = {
            0x05, 'h', 'e', 'l', 'l'
        };

        static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

        readWriteField<Field>(Buf, BufSize, comms::ErrorStatus::NotEnoughData);
    } while (false);    
}

TEST_CASE("Test109", "test109") 
{

    typedef comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t,
                comms::option::DefaultNumValue<9>
            >,
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t
            >,
            comms::field::String<comms::Field<BigEndianOpt> >
        >,
        comms::option::RemLengthMemberField<1>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");

    static_assert(Field::minLength() == 2U, "Invalid minLength");

    do {
        Field field;
        REQUIRE(field.length() == 2U);
        REQUIRE(std::get<1>(field.value()).value() == 0U);
        REQUIRE(!field.refresh());

        auto& strMem = std::get<2>(field.value());
        strMem.value() = "abc";
        REQUIRE(strMem.length() == 3U);
        REQUIRE(field.refresh());

        static const char ExpectedBuf[] = {
            0x9, 0x3, 'a', 'b', 'c'
        };
        static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
        writeReadField(field, ExpectedBuf, ExpectedBufSize);   
    } while (false); 

    do {
        static const char Buf[] = {
            0x01, 0x05, 'h', 'e', 'l', 'l', 'o', '0', '1'
        };

        static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

        auto field = readWriteField<Field>(Buf, BufSize);
        auto& strMem = std::get<2>(field.value());
        REQUIRE(field.length() == 7U);
        REQUIRE(strMem.value() == "hello");
        REQUIRE(field.valid());
    } while (false);

    do {
        static const char Buf[] = {
            0x02, 0x05, 'h', 'e', 'l', 'l'
        };

        static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

        readWriteField<Field>(Buf, BufSize, comms::ErrorStatus::NotEnoughData);
    } while (false);    
}

TEST_CASE("Test110", "test110") 
{
    typedef comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t,
                comms::option::DefaultNumValue<9>
            >,
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t
            >
        >,
        comms::option::RemLengthMemberField<1>
    > Field;

    static_assert(!Field::isVersionDependent(),
        "Invalid version dependency assumption");
    static_assert(Field::hasNonDefaultRefresh(),
        "Invalid refresh assumption");

    static_assert(Field::minLength() == 2U, "Invalid minLength");

    do {
        Field field;
        REQUIRE(field.length() == 2U);
        REQUIRE(std::get<1>(field.value()).value() == 0U);
        REQUIRE(!field.refresh());

        static const char ExpectedBuf[] = {
            0x9, 0x0
        };
        static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
        writeReadField(field, ExpectedBuf, ExpectedBufSize);   
    } while (false); 

    do {
        static const char Buf[] = {
            0x01, 0x03, 'h', 'e', 'l', 'l', 'o', '0', '1'
        };

        static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

        Field field;
        auto iter = &Buf[0];
        auto status = field.read(iter, BufSize);
        REQUIRE(status == comms::ErrorStatus::Success);
        auto diff = static_cast<std::size_t>(std::distance(&Buf[0], iter));
        REQUIRE(diff == 5U);
    } while (false);

    do {
        static const char Buf[] = {
            0x02, 0x05, 'h', 'e', 'l', 'l'
        };

        static const std::size_t BufSize = std::extent<decltype(Buf)>::value;

        readWriteField<Field>(Buf, BufSize, comms::ErrorStatus::NotEnoughData);
    } while (false);    
}

