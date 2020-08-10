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

using Test1_FieldBase = comms::Field<comms::option::BigEndian>;
template <std::uint8_t TVal>
using Test1_IntKeyField =
    comms::field::IntValue<
        Test1_FieldBase,
        std::uint8_t,
        comms::option::DefaultNumValue<TVal>,
        comms::option::ValidNumValueRange<TVal, TVal>,
        comms::option::FailOnInvalid<>
>;

TEST_CASE("Test1", "test1") 
{
    using Mem1 =
        comms::field::Bundle<
            Test1_FieldBase,
            std::tuple<
                Test1_IntKeyField<1>,
                comms::field::IntValue<Test1_FieldBase, std::uint16_t>
            >
        >;

    using Mem2 =
        comms::field::Bundle<
            Test1_FieldBase,
            std::tuple<
                Test1_IntKeyField<2>,
                comms::field::IntValue<Test1_FieldBase, std::uint32_t>,
                comms::field::Optional<
                    comms::field::IntValue<Test1_FieldBase, std::uint8_t>,
                    comms::option::ExistsSinceVersion<5>,
                    comms::option::ExistsByDefault
                >
            >
        >;

    using Field =
        comms::field::Variant<
            Test1_FieldBase,
            std::tuple<
                Mem1,
                Mem2
            >
        >;

    static_assert(Field::isVersionDependent(), "Must be version dependent");
    Field field;
    REQUIRE(field.currentField() == std::tuple_size<Field::Members>::value);

    do {
        field.setVersion(5);
        static const char Buf1[] = {
            0x2, 0x1, 0x2, 0x3, 0x4, 0x5
        };
        static const std::size_t Buf1Size = std::extent<decltype(Buf1)>::value;

        auto readIter = &Buf1[0];
        auto es = field.read(readIter, Buf1Size);
        REQUIRE(es == comms::ErrorStatus::Success);
        REQUIRE(field.currentField() == 1U);
        auto& m = field.accessField<1>();
        REQUIRE(std::get<0>(m.value()).value() == 2U);
        REQUIRE(std::get<1>(m.value()).value() == 0x01020304);
        REQUIRE(std::get<2>(m.value()).doesExist());
        REQUIRE(std::get<2>(m.value()).field().value() == 0x05);
        REQUIRE(field.length() == Buf1Size);
    } while (false);

    do {
        REQUIRE(field.setVersion(4));
        static const char Buf1[] = {
            0x2, 0x1, 0x2, 0x3, 0x4, 0x5
        };
        static const std::size_t Buf1Size = std::extent<decltype(Buf1)>::value;

        auto readIter = &Buf1[0];
        auto es = field.read(readIter, Buf1Size);
        REQUIRE(es == comms::ErrorStatus::Success);
        REQUIRE(field.currentField() == 1U);
        auto& m = field.accessField<1>();
        REQUIRE(std::get<0>(m.value()).value() == 2U);
        REQUIRE(std::get<1>(m.value()).value() == 0x01020304);
        REQUIRE(std::get<2>(m.value()).isMissing());
        REQUIRE(field.length() == Buf1Size - 1U);
    } while (false);

    do {
        field.reset();
        auto& m = field.initField<1>();
        REQUIRE(field.currentField() == 1U);
        field.setVersion(5);
        REQUIRE(std::get<2>(m.value()).doesExist());
        std::get<1>(m.value()).value() = 0x04030201;
        std::get<2>(m.value()).field().value() = 0xab;

        static const char Buf1[] = {
            0x2, 0x4, 0x3, 0x2, 0x1, (char)0xab
        };
        static const std::size_t Buf1Size = std::extent<decltype(Buf1)>::value;

        std::vector<char> outBuf;
        auto writeIter = std::back_inserter(outBuf);
        auto es = field.write(writeIter, outBuf.max_size());
        REQUIRE(es == comms::ErrorStatus::Success);
        REQUIRE(outBuf.size() == Buf1Size);
        REQUIRE(std::equal(outBuf.begin(), outBuf.end(), std::begin(Buf1)));
    } while (false);
}

using Test2_FieldBase = comms::Field<comms::option::BigEndian>;

template <std::uint8_t TVal>
using Test2_IntKeyField = Test1_IntKeyField<TVal>;

template <std::uint8_t TKey, typename TValue>
class Test2_PropField : public
    comms::field::Bundle<
        Test2_FieldBase,
        std::tuple<
            Test2_IntKeyField<TKey>,
            comms::field::IntValue<Test2_FieldBase, std::uint8_t>,
            TValue
        >,
        comms::option::RemLengthMemberField<1>
    >
{
    using Base =
        comms::field::Bundle<
            Test2_FieldBase,
            std::tuple<
                Test2_IntKeyField<TKey>,
                comms::field::IntValue<Test2_FieldBase, std::uint8_t>,
                TValue
            >,
            comms::option::RemLengthMemberField<1>
        >;
public:
    COMMS_FIELD_MEMBERS_NAMES(key, length, value);
};

TEST_CASE("Test2", "test2") 
{
    using Prop1 = Test2_PropField<1, comms::field::IntValue<Test1_FieldBase, std::uint16_t> >;
    using Prop2 =
        Test2_PropField<
            2,
            comms::field::Bundle<
                Test2_FieldBase,
                std::tuple<
                    comms::field::IntValue<Test1_FieldBase, std::uint32_t>,
                    comms::field::Optional<
                        comms::field::IntValue<Test1_FieldBase, std::uint8_t>,
                        comms::option::ExistsSinceVersion<5>,
                        comms::option::ExistsByDefault
                    >
                >
            >
        >;

    using Field =
        comms::field::Variant<
            Test2_FieldBase,
            std::tuple<
                Prop1,
                Prop2
            >
        >;

    static_assert(Field::isVersionDependent(), "Must be version dependent");
    Field field;
    REQUIRE(field.currentField() == std::tuple_size<Field::Members>::value);

    do {
        field.setVersion(5);
        static const char Buf1[] = {
            0x2, 0x5, 0x1, 0x2, 0x3, 0x4, 0x5
        };
        static const std::size_t Buf1Size = std::extent<decltype(Buf1)>::value;

        auto readIter = &Buf1[0];
        auto es = field.read(readIter, Buf1Size);
        REQUIRE(es == comms::ErrorStatus::Success);
        REQUIRE(field.currentField() == 1U);
        auto& m = field.accessField<1>();
        REQUIRE(m.field_key().value() == 2U);
        REQUIRE(std::get<0>(m.field_value().value()).value() == 0x01020304);
        REQUIRE(std::get<1>(m.field_value().value()).doesExist());
        REQUIRE(std::get<1>(m.field_value().value()).field().value() == 0x05);
        REQUIRE(field.length() == Buf1Size);
    } while (false);

    do {
        REQUIRE(field.setVersion(4));
        static const char Buf1[] = {
            0x2, 0x5, 0x1, 0x2, 0x3, 0x4, 0x5
        };
        static const std::size_t Buf1Size = std::extent<decltype(Buf1)>::value;

        auto readIter = &Buf1[0];
        auto es = field.read(readIter, Buf1Size);
        REQUIRE(es == comms::ErrorStatus::Success);
        REQUIRE(field.currentField() == 1U);
        auto& m = field.accessField<1>();
        REQUIRE(m.field_key().value() == 2U);
        REQUIRE(std::get<0>(m.field_value().value()).value() == 0x01020304);
        REQUIRE(std::get<1>(m.field_value().value()).isMissing());
        REQUIRE(field.length() == Buf1Size - 1U);
    } while (false);

    do {
        field.reset();
        REQUIRE(!field.currentFieldValid());
        field.setVersion(4);
        auto& m = field.initField<1>();
        REQUIRE(field.currentField() == 1U);
        REQUIRE(std::get<1>(m.field_value().value()).isMissing());
        REQUIRE(m.field_length().value() == 4U);
        field.setVersion(5);
        REQUIRE(m.field_length().value() == 5U);
        REQUIRE(std::get<1>(m.field_value().value()).doesExist());
        std::get<0>(m.field_value().value()).value() = 0x04030201;
        std::get<1>(m.field_value().value()).field().value() = 0xab;

        static const char Buf1[] = {
            0x2, 0x5, 0x4, 0x3, 0x2, 0x1, (char)0xab
        };
        static const std::size_t Buf1Size = std::extent<decltype(Buf1)>::value;

        std::vector<char> outBuf;
        auto writeIter = std::back_inserter(outBuf);
        auto es = field.write(writeIter, outBuf.max_size());
        REQUIRE(es == comms::ErrorStatus::Success);
        REQUIRE(outBuf.size() == Buf1Size);
        REQUIRE(std::equal(outBuf.begin(), outBuf.end(), std::begin(Buf1)));
    } while (false);
}

TEST_CASE("Test3", "test3") 
{
    typedef comms::field::Bundle<
        comms::Field<BigEndianOpt>,
        std::tuple<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint16_t,
                comms::option::VarLength<1, 2>
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
        REQUIRE(field.length() == 4U);

        static const char ExpectedBuf[] = {
            0x3, 'a', 'a', 'a'
        };
        static const std::size_t ExpectedBufSize = std::extent<decltype(ExpectedBuf)>::value;
        writeReadField(field, ExpectedBuf, ExpectedBufSize);
    } while (false);

    do {
        Field field;
        std::string str(128, 'a');
        std::get<1>(field.value()).value() = str;
        REQUIRE(field.refresh());
        REQUIRE(field.length() == 130U);
    } while (false);
}

TEST_CASE("Test4", "test4") 
{
    enum class Field2Val : std::uint16_t
    {
        V0 = 0,
        V1 = 0xff
    };

    using FieldBase = comms::Field<BigEndianOpt>;
    using Field1 = comms::field::IntValue<FieldBase, uint8_t>;
    using Field2 = comms::field::EnumValue<FieldBase, Field2Val>;


    Field1 field1;
    Field2 field2;

    REQUIRE(field1.length() == 1U);
    REQUIRE(field2.length() == 2U);

    Field1 field1Tmp;
    field2.value() = Field2Val::V1;
    field1Tmp.value() = comms::cast_assign(field1.value()) = field2.value();
    REQUIRE(field1.value() == 0xff);
    REQUIRE(field1 == field1Tmp);
}

TEST_CASE("Test5", "test5") 
{
    typedef comms::field::ArrayList<
        comms::Field<BigEndianOpt>,
        comms::field::String<
            comms::Field<BigEndianOpt>,
            comms::option::SequenceSizeFieldPrefix<
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
        comms::option::SequenceElemSerLengthFieldPrefix<
            comms::field::IntValue<
                comms::Field<BigEndianOpt>,
                std::uint8_t
            >
        >
    > Field;

    Field field;
    REQUIRE(field.valid());
    field.value().resize(1);
    REQUIRE(field.valid());
    REQUIRE(field.canWrite());
    field.value()[0].value().assign(260, 'a');
    REQUIRE(!field.valid());
    REQUIRE(!field.canWrite());
    std::vector<std::uint8_t> outBuf;
    auto writeIter = std::back_inserter(outBuf);
    auto es = field.write(writeIter, outBuf.max_size());
    REQUIRE(es == comms::ErrorStatus::InvalidMsgData);
    field.value()[0].value().assign(5, 'b');
    REQUIRE(field.valid());
}

TEST_CASE("Test6", "test6") 
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
        strMem.value().assign(260, 'a');
        REQUIRE(!field.valid());
        REQUIRE(!field.canWrite());

        std::vector<std::uint8_t> outBuf;
        auto writeIter = std::back_inserter(outBuf);
        auto es = field.write(writeIter, outBuf.max_size());
        REQUIRE(es == comms::ErrorStatus::InvalidMsgData);
    } while (false);

}

TEST_CASE("Test7", "test7") 
{
    do {
        typedef comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint64_t,
            comms::option::UnitsBytes
        > Field;

        Field field;
        static_assert(comms::units::isBytes(field), "Invalid units");

        const unsigned Num = 7;

        comms::units::setBytes(field, Num);
        REQUIRE(comms::units::getBytes<unsigned>(field) == Num);
        REQUIRE(fpEquals(comms::units::getKilobytes<double>(field), (double(Num) / 1024)));
        REQUIRE(fpEquals(comms::units::getMegabytes<double>(field), (double(Num) / 1024 / 1024)));
        REQUIRE(fpEquals(comms::units::getGigabytes<double>(field), (double(Num) / 1024 / 1024 / 1024)));
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), (double(Num) / 1024 / 1024 / 1024 / 1024)));

        comms::units::setKilobytes(field, Num);
        REQUIRE(comms::units::getBytes<unsigned>(field) == Num * 1024);
        REQUIRE(comms::units::getKilobytes<unsigned>(field) == Num);
        REQUIRE(fpEquals(comms::units::getMegabytes<double>(field), (double(Num) / 1024 )));
        REQUIRE(fpEquals(comms::units::getGigabytes<double>(field), (double(Num) / 1024 / 1024)));
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), (double(Num) / 1024 / 1024 / 1024)));

        comms::units::setMegabytes(field, Num);
        REQUIRE(comms::units::getBytes<unsigned long>(field) == Num * 1024UL * 1024);
        REQUIRE(comms::units::getKilobytes<unsigned>(field) == Num * 1024);
        REQUIRE(comms::units::getMegabytes<unsigned>(field) == Num);
        REQUIRE(fpEquals(comms::units::getGigabytes<double>(field), (double(Num) / 1024)));
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), (double(Num) / 1024 / 1024)));

        comms::units::setGigabytes(field, Num);
        REQUIRE(comms::units::getBytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024);
        REQUIRE(comms::units::getKilobytes<std::uintmax_t>(field) == Num * 1024ULL * 1024);
        REQUIRE(comms::units::getMegabytes<std::uintmax_t>(field) == Num * 1024ULL);
        REQUIRE(comms::units::getGigabytes<std::uintmax_t>(field) == Num);
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), (double(Num) / 1024)));

        comms::units::setTerabytes(field, Num);
        REQUIRE(comms::units::getBytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024 * 1024);
        REQUIRE(comms::units::getKilobytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024);
        REQUIRE(comms::units::getMegabytes<std::uintmax_t>(field) == Num * 1024ULL * 1024);
        REQUIRE(comms::units::getGigabytes<std::uintmax_t>(field) == Num * 1024ULL);
        REQUIRE(comms::units::getTerabytes<std::uintmax_t>(field) == Num);
    } while (false);

    do {
        typedef comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint64_t,
            comms::option::UnitsKilobytes
        > Field;

        Field field;
        static_assert(comms::units::isKilobytes(field), "Invalid units");

        const unsigned Num = 7;

        comms::units::setKilobytes(field, Num);
        REQUIRE(comms::units::getBytes<unsigned>(field) == Num * 1024);
        REQUIRE(comms::units::getKilobytes<unsigned>(field) == Num);
        REQUIRE(fpEquals(comms::units::getMegabytes<double>(field), (double(Num) / 1024 )));
        REQUIRE(fpEquals(comms::units::getGigabytes<double>(field), (double(Num) / 1024 / 1024)));
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), (double(Num) / 1024 / 1024 / 1024)));

        comms::units::setMegabytes(field, Num);
        REQUIRE(comms::units::getBytes<unsigned long>(field) == Num * 1024UL * 1024);
        REQUIRE(comms::units::getKilobytes<unsigned>(field) == Num * 1024);
        REQUIRE(comms::units::getMegabytes<unsigned>(field) == Num);
        REQUIRE(fpEquals(comms::units::getGigabytes<double>(field), (double(Num) / 1024)));
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), (double(Num) / 1024 / 1024)));

        comms::units::setGigabytes(field, Num);
        REQUIRE(comms::units::getBytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024);
        REQUIRE(comms::units::getKilobytes<std::uintmax_t>(field) == Num * 1024ULL * 1024);
        REQUIRE(comms::units::getMegabytes<std::uintmax_t>(field) == Num * 1024ULL);
        REQUIRE(comms::units::getGigabytes<std::uintmax_t>(field) == Num);
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), (double(Num) / 1024)));

        comms::units::setTerabytes(field, Num);
        REQUIRE(comms::units::getBytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024 * 1024);
        REQUIRE(comms::units::getKilobytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024);
        REQUIRE(comms::units::getMegabytes<std::uintmax_t>(field) == Num * 1024ULL * 1024);
        REQUIRE(comms::units::getGigabytes<std::uintmax_t>(field) == Num * 1024ULL);
        REQUIRE(comms::units::getTerabytes<std::uintmax_t>(field) == Num);
    } while (false);    

    do {
        typedef comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint64_t,
            comms::option::UnitsMegabytes
        > Field;

        Field field;
        static_assert(comms::units::isMegabytes(field), "Invalid units");

        const unsigned Num = 7;

        comms::units::setMegabytes(field, Num);
        REQUIRE(comms::units::getBytes<unsigned long>(field) == Num * 1024UL * 1024);
        REQUIRE(comms::units::getKilobytes<unsigned>(field) == Num * 1024);
        REQUIRE(comms::units::getMegabytes<unsigned>(field) == Num);
        REQUIRE(fpEquals(comms::units::getGigabytes<double>(field), (double(Num) / 1024)));
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), (double(Num) / 1024 / 1024)));

        comms::units::setGigabytes(field, Num);
        REQUIRE(comms::units::getBytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024);
        REQUIRE(comms::units::getKilobytes<std::uintmax_t>(field) == Num * 1024ULL * 1024);
        REQUIRE(comms::units::getMegabytes<std::uintmax_t>(field) == Num * 1024ULL);
        REQUIRE(comms::units::getGigabytes<std::uintmax_t>(field) == Num);
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), (double(Num) / 1024)));

        comms::units::setTerabytes(field, Num);
        REQUIRE(comms::units::getBytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024 * 1024);
        REQUIRE(comms::units::getKilobytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024);
        REQUIRE(comms::units::getMegabytes<std::uintmax_t>(field) == Num * 1024ULL * 1024);
        REQUIRE(comms::units::getGigabytes<std::uintmax_t>(field) == Num * 1024ULL);
        REQUIRE(comms::units::getTerabytes<std::uintmax_t>(field) == Num);
    } while (false);  

    do {
        typedef comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint64_t,
            comms::option::UnitsGigabytes
        > Field;

        Field field;
        static_assert(comms::units::isGigabytes(field), "Invalid units");

        const unsigned Num = 7;

        comms::units::setGigabytes(field, Num);
        REQUIRE(comms::units::getBytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024);
        REQUIRE(comms::units::getKilobytes<std::uintmax_t>(field) == Num * 1024ULL * 1024);
        REQUIRE(comms::units::getMegabytes<std::uintmax_t>(field) == Num * 1024ULL);
        REQUIRE(comms::units::getGigabytes<std::uintmax_t>(field) == Num);
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), (double(Num) / 1024)));

        comms::units::setTerabytes(field, Num);
        REQUIRE(comms::units::getBytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024 * 1024);
        REQUIRE(comms::units::getKilobytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024);
        REQUIRE(comms::units::getMegabytes<std::uintmax_t>(field) == Num * 1024ULL * 1024);
        REQUIRE(comms::units::getGigabytes<std::uintmax_t>(field) == Num * 1024ULL);
        REQUIRE(comms::units::getTerabytes<std::uintmax_t>(field) == Num);
    } while (false);        

    do {
        typedef comms::field::IntValue<
            comms::Field<BigEndianOpt>,
            std::uint64_t,
            comms::option::UnitsTerabytes
        > Field;

        Field field;
        static_assert(comms::units::isTerabytes(field), "Invalid units");

        const unsigned Num = 7;

        comms::units::setTerabytes(field, Num);
        REQUIRE(comms::units::getBytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024 * 1024);
        REQUIRE(comms::units::getKilobytes<std::uintmax_t>(field) == Num * 1024ULL * 1024 * 1024);
        REQUIRE(comms::units::getMegabytes<std::uintmax_t>(field) == Num * 1024ULL * 1024);
        REQUIRE(comms::units::getGigabytes<std::uintmax_t>(field) == Num * 1024ULL);
        REQUIRE(comms::units::getTerabytes<std::uintmax_t>(field) == Num);
    } while (false);                    

}

struct Test8_Helper
{
    template <typename TField>
    static void test(TField& field)
    {
        static const double Num = 7;

        comms::units::setBytes(field, Num);
        REQUIRE(fpEquals(comms::units::getBytes<double>(field), Num));
        REQUIRE(fpEquals(comms::units::getKilobytes<double>(field), Num / 1024));
        REQUIRE(fpEquals(comms::units::getMegabytes<double>(field), Num / 1024 / 1024));
        REQUIRE(fpEquals(comms::units::getGigabytes<double>(field), Num / 1024 / 1024 / 1024));
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), Num / 1024 / 1024 / 1024 / 1024));

        comms::units::setKilobytes(field, Num);
        REQUIRE(fpEquals(comms::units::getBytes<double>(field), Num * 1024));
        REQUIRE(fpEquals(comms::units::getKilobytes<double>(field), Num));        
        REQUIRE(fpEquals(comms::units::getMegabytes<double>(field), Num / 1024 ));
        REQUIRE(fpEquals(comms::units::getGigabytes<double>(field), Num / 1024 / 1024));
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), Num / 1024 / 1024 / 1024));

        comms::units::setMegabytes(field, Num);
        REQUIRE(fpEquals(comms::units::getBytes<double>(field), Num * 1024 * 1024));
        REQUIRE(fpEquals(comms::units::getKilobytes<double>(field), Num * 1024));        
        REQUIRE(fpEquals(comms::units::getMegabytes<double>(field), Num));        
        REQUIRE(fpEquals(comms::units::getGigabytes<double>(field), Num / 1024));
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), Num / 1024 / 1024));

        comms::units::setGigabytes(field, Num);
        REQUIRE(fpEquals(comms::units::getBytes<double>(field), Num * 1024 * 1024 * 1024));
        REQUIRE(fpEquals(comms::units::getKilobytes<double>(field), Num * 1024 * 1024));        
        REQUIRE(fpEquals(comms::units::getMegabytes<double>(field), Num * 1024));        
        REQUIRE(fpEquals(comms::units::getGigabytes<double>(field), Num));
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), Num / 1024));

        comms::units::setTerabytes(field, Num);
        REQUIRE(fpEquals(comms::units::getBytes<double>(field), Num * 1024 * 1024 * 1024 * 1024));
        REQUIRE(fpEquals(comms::units::getKilobytes<double>(field), Num * 1024 * 1024 * 1024));        
        REQUIRE(fpEquals(comms::units::getMegabytes<double>(field), Num * 1024 * 1024));        
        REQUIRE(fpEquals(comms::units::getGigabytes<double>(field), Num * 1024));
        REQUIRE(fpEquals(comms::units::getTerabytes<double>(field), Num));        
    }
};

TEST_CASE("Test8", "test8") 
{
    do {
        typedef comms::field::FloatValue<
            comms::Field<BigEndianOpt>,
            double,
            comms::option::UnitsBytes
        > Field;

        Field field;
        static_assert(comms::units::isBytes(field), "Invalid units");
        Test8_Helper::test(field);
    } while (false);

    do {
        typedef comms::field::FloatValue<
            comms::Field<BigEndianOpt>,
            double,
            comms::option::UnitsKilobytes
        > Field;

        Field field;
        static_assert(comms::units::isKilobytes(field), "Invalid units");
        Test8_Helper::test(field);
    } while (false);    

    do {
        typedef comms::field::FloatValue<
            comms::Field<BigEndianOpt>,
            double,
            comms::option::UnitsMegabytes
        > Field;

        Field field;
        static_assert(comms::units::isMegabytes(field), "Invalid units");
        Test8_Helper::test(field);
    } while (false);        

    do {
        typedef comms::field::FloatValue<
            comms::Field<BigEndianOpt>,
            double,
            comms::option::UnitsGigabytes
        > Field;

        Field field;
        static_assert(comms::units::isGigabytes(field), "Invalid units");
        Test8_Helper::test(field);
    } while (false);     

    do {
        typedef comms::field::FloatValue<
            comms::Field<BigEndianOpt>,
            double,
            comms::option::UnitsTerabytes
        > Field;

        Field field;
        static_assert(comms::units::isTerabytes(field), "Invalid units");
        Test8_Helper::test(field);
    } while (false);           
}

TEST_CASE("Test9", "test9") 
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
        comms::option::CustomStorageType<comms::util::StringView>
    > Field;

    static_assert(std::is_same<Field::ValueType, comms::util::StringView>::value, 
        "comms::util::StringView is expected to be used for this field");     

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
    REQUIRE(field.value() == comms::util::StringView("foo"));
    REQUIRE(field.length() == 6U);

    static const char ExpectedBuf2[] = {
        'f', 'o', 'o', 0x0, 0x0, 0x0
    };
    static const std::size_t ExpectedBufSize2 = std::extent<decltype(ExpectedBuf2)>::value;
    writeReadField(field, ExpectedBuf2, ExpectedBufSize2);

    field = readWriteField<Field>(&ExpectedBuf2[0], ExpectedBufSize2);
    REQUIRE(field.value() == "foo");
}
