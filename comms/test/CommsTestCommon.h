//
// Copyright 2014 - 2017 (C). Alex Robenko. All rights reserved.
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

#include <tuple>
#include <vector>
#include <iterator>
#include <iostream>

#include "comms/comms.h"

enum MessageType {
    MessageType1,
    UnusedValue1,
    MessageType2,
    UnusedValue2,
    UnusedValue3,
    MessageType3,
    MessageType4,
    MessageType5,
};

template <typename TTraits>
class TestMessageBase : public comms::Message<TTraits>
{
    using Base = comms::Message<TTraits>;
public:

    virtual ~TestMessageBase() {}

    const std::string& getName() const
    {
        return this->getNameImpl();
    }

private:
    virtual const std::string& getNameImpl() const = 0;
};


template <typename TField>
using FieldsMessage1 =
    std::tuple<
        comms::field::IntValue<TField, std::uint16_t>
    >;

template <typename TMessage>
class Message1 : public
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType1>,
            comms::option::FieldsImpl<FieldsMessage1<typename TMessage::Field> >,
            comms::option::MsgType<Message1<TMessage> >
        >
{
public:

    COMMS_MSG_FIELDS_ACCESS(value1);

    Message1() = default;

    virtual ~Message1() = default;

protected:

    virtual const std::string& getNameImpl() const
    {
        static const std::string str("Message1");
        return str;
    }
};

template <typename... TArgs>
bool operator==(
    const Message1<TArgs...>& msg1,
    const Message1<TArgs...>& msg2)
{
    return msg1.fields() == msg2.fields();
}

template <typename TMessage>
class Message2 : public
    comms::MessageBase<
        TMessage,
        comms::option::StaticNumIdImpl<MessageType2>,
        comms::option::ZeroFieldsImpl,
        comms::option::MsgType<Message2<TMessage> >
    >
{
public:
    virtual ~Message2() = default;

protected:
    virtual const std::string& getNameImpl() const
    {
        static const std::string str("Message2");
        return str;
    }
};

template <typename... TArgs>
bool operator==(
    const Message2<TArgs...>& msg1,
    const Message2<TArgs...>& msg2)
{
    return msg1.fields() == msg2.fields();
}

template <typename TField>
using Message3Fields =
    std::tuple<
        comms::field::IntValue<TField, std::uint32_t>,
        comms::field::IntValue<
            TField,
            std::int16_t,
            comms::option::FixedLength<1>,
            comms::option::ValidNumValueRange<-120, 120>,
            comms::option::DefaultNumValue<127> >, // invalid upon creation
        comms::field::BitmaskValue<TField, comms::option::FixedLength<2> >,
        comms::field::BitmaskValue<TField, comms::option::FixedLength<3> >
    >;

template <typename TMessage>
class Message3 : public
    comms::MessageBase<
        TMessage,
        comms::option::StaticNumIdImpl<MessageType3>,
        comms::option::FieldsImpl<Message3Fields<typename TMessage::Field> >,
        comms::option::MsgType<Message3<TMessage> >
    >
{
public:
    COMMS_MSG_FIELDS_ACCESS(value1, value2, value3, value4);

    Message3() = default;

    virtual ~Message3() = default;

protected:

    virtual const std::string& getNameImpl() const
    {
        static const std::string str("Message3");
        return str;
    }
};

template <typename... TArgs>
bool operator==(
    const Message3<TArgs...>& msg1,
    const Message3<TArgs...>& msg2)
{
    return msg1.fields() == msg2.fields();
}

template <typename TField>
using Message4Fields =
    std::tuple<
        comms::field::BitmaskValue<
            TField,
            comms::option::FixedLength<1> >,
        comms::field::Optional<
            comms::field::IntValue<
                TField,
                std::uint16_t
            >
        >
    >;

template <typename TMessage>
class Message4 : public
    comms::MessageBase<
        TMessage,
        comms::option::StaticNumIdImpl<MessageType4>,
        comms::option::FieldsImpl<Message4Fields<typename TMessage::Field> >,
        comms::option::MsgType<Message4<TMessage> >,
        comms::option::HasDoRefresh
    >
{
public:
    COMMS_MSG_FIELDS_ACCESS(value1, value2);

    Message4()
    {
        auto& optField = field_value2();
        optField.setMissing();
    }

    virtual ~Message4() = default;

    bool doRefresh()
    {
        auto& mask = field_value1();
        auto expectedNextFieldMode = comms::field::OptionalMode::Missing;
        if ((mask.value() & 0x1) != 0) {
            expectedNextFieldMode = comms::field::OptionalMode::Exists;
        }

        auto& optField = field_value2();
        if (optField.getMode() == expectedNextFieldMode) {
            return false;
        }

        optField.setMode(expectedNextFieldMode);
        return true;
    }

protected:
    virtual const std::string& getNameImpl() const
    {
        static const std::string str("Message4");
        return str;
    }
};

template <typename... TArgs>
bool operator==(
    const Message4<TArgs...>& msg1,
    const Message4<TArgs...>& msg2)
{
    return msg1.fields() == msg2.fields();
}


template <typename TField>
using FieldsMessage5 =
    std::tuple<
        comms::field::IntValue<TField, std::uint16_t>,
        comms::field::IntValue<TField, std::int8_t>
    >;

template <typename TMessage>
class Message5 : public
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType5>,
            comms::option::FieldsImpl<FieldsMessage5<comms::Field<comms::option::BigEndian> > >,
            comms::option::MsgType<Message5<TMessage> >
        >
{
public:

    COMMS_MSG_FIELDS_ACCESS(value1, value2);

    Message5() = default;

    virtual ~Message5() = default;

protected:

    virtual const std::string& getNameImpl() const
    {
        static const std::string str("Message5");
        return str;
    }
};

template <typename... TArgs>
bool operator==(
    const Message5<TArgs...>& msg1,
    const Message5<TArgs...>& msg2)
{
    return msg1.fields() == msg2.fields();
}

template <typename TMessage>
using AllMessages =
    std::tuple<
        Message1<TMessage>,
        Message2<TMessage>,
        Message3<TMessage>
    >;


template <typename TProtStack>
typename TProtStack::MsgPtr commonReadWriteMsgTest(
    TProtStack& stack,
    const char* const buf,
    std::size_t bufSize,
    comms::ErrorStatus expectedEs = comms::ErrorStatus::Success)
{
    using MsgPtr = typename TProtStack::MsgPtr;

    MsgPtr msg;
    auto readIter = buf;
    auto es = stack.read(msg, readIter, bufSize);
    TS_ASSERT_EQUALS(es, expectedEs);
    if (es != comms::ErrorStatus::Success) {
        return std::move(msg);
    }

    TS_ASSERT(msg);

    auto actualBufSize = static_cast<std::size_t>(std::distance(buf, readIter));
    TS_ASSERT_EQUALS(actualBufSize, stack.length(*msg));
    std::unique_ptr<char []> outCheckBuf(new char[actualBufSize]);
    auto writeIter = &outCheckBuf[0];
    es = stack.write(*msg, writeIter, actualBufSize);
    TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    TS_ASSERT(std::equal(buf, buf + actualBufSize, static_cast<const char*>(&outCheckBuf[0])));
    return std::move(msg);
}

template <typename TProtStack>
typename TProtStack::MsgPtr commonReadWriteMsgTest(
    TProtStack& stack,
    typename TProtStack::AllFields& fields,
    const char* const buf,
    std::size_t bufSize,
    comms::ErrorStatus expectedEs = comms::ErrorStatus::Success)
{
    using MsgPtr = typename TProtStack::MsgPtr;

    MsgPtr msg;
    auto readIter = buf;
    auto es = stack.template readFieldsCached<0>(fields, msg, readIter, bufSize);
    TS_ASSERT_EQUALS(es, expectedEs);
    if (es != comms::ErrorStatus::Success) {
        return std::move(msg);
    }

    TS_ASSERT(msg);

    auto actualBufSize = static_cast<std::size_t>(std::distance(buf, readIter));
    TS_ASSERT_EQUALS(actualBufSize, stack.length(*msg));
    std::unique_ptr<char []> outCheckBuf(new char[actualBufSize]);
    auto writeIter = &outCheckBuf[0];
    typename TProtStack::AllFields writtenFields;
    es = stack.template writeFieldsCached<0>(writtenFields, *msg, writeIter, actualBufSize);
    TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    TS_ASSERT(std::equal(buf, buf + actualBufSize, static_cast<const char*>(&outCheckBuf[0])));
    TS_ASSERT_EQUALS(fields, writtenFields);
    return std::move(msg);
}

template <typename TProtStack>
typename TProtStack::MsgPtr vectorBackInsertReadWriteMsgTest(
    TProtStack& stack,
    const char* const buf,
    std::size_t bufSize,
    comms::ErrorStatus expectedEs = comms::ErrorStatus::Success)
{
    using MsgPtr = typename TProtStack::MsgPtr;

    MsgPtr msg;
    auto readIter = buf;
    auto es = stack.read(msg, readIter, bufSize);
    TS_ASSERT_EQUALS(es, expectedEs);
    if (es != comms::ErrorStatus::Success) {
        return std::move(msg);
    }

    TS_ASSERT(msg);

    auto actualBufSize = static_cast<std::size_t>(std::distance(buf, readIter));
    TS_ASSERT_EQUALS(actualBufSize, stack.length(*msg));
    std::vector<char> outCheckBuf;
    auto writeIter = std::back_inserter(outCheckBuf);
    es = stack.write(*msg, writeIter, actualBufSize);
    if (es == comms::ErrorStatus::UpdateRequired) {
        assert(!outCheckBuf.empty());
        auto updateIter = &outCheckBuf[0];
        es = stack.update(updateIter, actualBufSize);
    }
    TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    TS_ASSERT_EQUALS(outCheckBuf.size(), actualBufSize);
    TS_ASSERT_EQUALS(outCheckBuf.size(), stack.length(*msg));
    bool resultAsExpected = std::equal(buf, buf + actualBufSize, outCheckBuf.cbegin());
    if (!resultAsExpected) {
        std::cout << "Original buffer:\n\t" << std::hex;
        std::copy_n(buf, actualBufSize, std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << "\n\nWritten buffer:\n\t";
        std::copy_n(&outCheckBuf[0], actualBufSize, std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << std::dec << std::endl;
    }
    TS_ASSERT(resultAsExpected);
    return std::move(msg);
}


template <typename TProtStack, typename TMessage>
void commonWriteReadMsgTest(
    TProtStack& stack,
    TMessage msg,
    char* buf,
    std::size_t bufSize,
    const char* expectedBuf,
    comms::ErrorStatus expectedEs = comms::ErrorStatus::Success)
{
    auto writeIter = buf;
    auto es = stack.write(msg, writeIter, bufSize);
    TS_ASSERT_EQUALS(es, expectedEs);
    if (es != comms::ErrorStatus::Success) {
        return;
    }

    assert(expectedBuf != nullptr);
    auto constBuf = static_cast<const char*>(buf);
    TS_ASSERT(std::equal(constBuf, constBuf + bufSize, &expectedBuf[0]));

    using MsgPtr = typename TProtStack::MsgPtr;
    MsgPtr msgPtr;
    auto readIter = expectedBuf;
    es = stack.read(msgPtr, readIter, bufSize);
    TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    TS_ASSERT(msgPtr);
    TS_ASSERT_EQUALS(msgPtr->getId(), msg.getId());
    auto* castedMsg = dynamic_cast<TMessage*>(msgPtr.get());
    TS_ASSERT(castedMsg != nullptr);
    TS_ASSERT_EQUALS(*castedMsg, msg);
}

template <typename TProtStack, typename TMessage>
void vectorBackInsertWriteReadMsgTest(
    TProtStack& stack,
    TMessage msg,
    const char* expectedBuf,
    std::size_t bufSize,
    comms::ErrorStatus expectedEs = comms::ErrorStatus::Success)
{
    std::vector<char> buf;
    auto writeIter = std::back_inserter(buf);
    auto es = stack.write(msg, writeIter, buf.max_size());
    if (expectedEs != comms::ErrorStatus::Success) {
        TS_ASSERT_EQUALS(es, expectedEs);
        return;
    }

    if (es == comms::ErrorStatus::UpdateRequired) {
        auto updateIter = &buf[0];
        es = stack.update(updateIter, buf.size());
        TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    }

    TS_ASSERT_EQUALS(es, expectedEs);
    if (es != comms::ErrorStatus::Success) {
        return;
    }

    assert(expectedBuf != nullptr);
    TS_ASSERT_EQUALS(buf.size(), bufSize);
    bool bufEquals = std::equal(buf.cbegin(), buf.cend(), &expectedBuf[0]);
    if (!bufEquals) {
        std::cout << "ERROR: Buffers are not equal:\nexpected: " << std::hex;
        std::copy_n(&expectedBuf[0], bufSize, std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << "\nwritten: ";
        std::copy(buf.cbegin(), buf.cend(), std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << std::dec << std::endl;
    }
    TS_ASSERT(bufEquals);

    using MsgPtr = typename TProtStack::MsgPtr;
    MsgPtr msgPtr;
    const char* readIter = &buf[0];
    es = stack.read(msgPtr, readIter, buf.size());
    TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    TS_ASSERT(msgPtr);
    TS_ASSERT_EQUALS(msgPtr->getId(), msg.getId());
    auto* castedMsg = dynamic_cast<TMessage*>(msgPtr.get());
    TS_ASSERT(castedMsg != nullptr);
    TS_ASSERT_EQUALS(*castedMsg, msg);
}

