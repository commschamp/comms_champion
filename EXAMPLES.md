# COMMS Library Quick Examples
Below are quick examples on how to define fields as well as messages using
[COMMS Library](https://github.com/arobenko/comms_champion#comms-library). 
For proper and full tutorial please refer to proper documentation
(Download **doc_comms.zip** archive from 
[release artefacts](https://github.com/arobenko/comms_champion/releases)).

## Defining Fields
Almost every field definition class receives its base class as a template
parameter. This base class is expected to be a variant of **comms::Field** with
endian information.
```cpp
using MyFieldBase = comms::Field<comms::option::BigEndian>;
```

Now the definition of simple 2 byte unsigned integer value field looks like this:
```cpp
using MyIntField = comms::field::IntValue<MyFieldBase, std::uint16_t>;
```

The definition of unsigned integer with 3 bytes serialisation length:
```cpp
using My3ByteIntField = 
    comms::field::IntValue<
        MyFieldBase, // big endian serialisation 
        std::uint32_t, // store as 4 byte unsigned integer
        comms::option::FixedLength<3> // serialise using only 3 bytes
    >;
```

Definition of the year value, serialised using only 1 byte as an offset from
year 2000, and default constructed as year 2017:
```cpp
using MyYearField = 
    comms::field::IntValue<
        MyFieldBase, // big endian serialisation
        std::int16_t, // store as 2 byte value
        comms::option::FixedLength<1>, // serialise using only 1 byte
        comms::option::NumValueSerOffset<-2000> // add (-2000) before serialisation and 
                                                // subtruct (-2000) after deserialisation
        comms::option::DefaultNumValue<2017> // construct with default value 2017
    >;
```

Variant length (Base-128) integer value:
```cpp
using MyVarLengthField = 
    comms::field::IntValue<
        MyFieldBase, // big endian serialisation
        std::uint32_t, // store as 4 bytes value
        comms::option::VarLength<1, 4> // 1 to 4 bytes serialisation length.
    >;
```

Enum values are similar to integer ones:
```cpp
enum class MyEnum : std::uint8_t // Serialise using 1 byte
{
    Value1,
    Value2,
    Value3,
    NumOfValues
};

using MyEnumField = 
    comms::field::EnumValue<
        MyFieldBase, // big endian serialisation
        MyEnum, // use MyEnum as storage type
        comms::option::ValidNumValueRange<0, (int)MyEnum::NumOfValues - 1> // provide range of valid values
    >;
```

2 bytes bitmask value:
```cpp
struct MyBitmaskField : public 
    comms::field::BitmaskValue<
        MyFieldBase, // big endian serialisation
        comms::option::FixedLength<2> // serialise using 2 bytes
        comms::option::BitmaskReservedBits<0xfff0> // Specify reserved bits 
    >
{
    COMMS_BITMASK_BITS_SEQ(name1, name2, name3, name4); // provide names for bits for convenient access
}
```

Bitfields:
```cpp
struct MyBitfield : public
    comms::field::Bitfield<
        MyFieldBase,
        std::tuple<
            comms::field::IntValue<MyFieldBase, std::uint8_t, comms::option::FixedBitLength<2> >, // 2 bits value
            comms::field::BitmaskValue<MyFieldBase, comms::option::FixedBitLength<3> >, // 3 bits value
            comms::field::EnumValue<MyFieldBase, MyEnum, comms::option::FixedBitLength<3> > // 3 bits value
        >
    >
{
    COMMS_FIELD_MEMBERS_ACCESS(value1, value2, value3); // names for member fields for convenient access
};
```

Simple raw data list:
```cpp
using MyRawDataList = 
    comms::field::ArrayList<
        MyFieldBase,
        std::uint8_t
    >;
```

Raw data list with 2 byte size prefix:
```cpp
using MyRawDataList2 = 
    comms::field::ArrayList<
        MyFieldBase,
        std::uint8_t,
        comms::option::SequenceSizeFieldPrefix<
            comms::field::IntValue<MyFieldBase, std::uint16_t>
        >
    >;
```

Size prefixed list of complex (bundle) elements:
```cpp
using MyComplexList = 
    comms::field::ArrayList<
        MyFieldBase,
        comms::field::Bundle<
            MyFieldBase,
            std::tuple<
                comms::field::IntValue<MyFieldBase, std::uint16_t>, // 2 bytes int
                comms::field::EnumValue<MyFieldBase, MyEnum> // 1 byte enum
            >
        >,
        comms::option::SequenceSizeFieldPrefix<
            comms::field::IntValue<MyFieldBase, std::uint16_t>
        >
    >;
```

String with 1 byte size prefix:
```cpp
using MyString = 
    comms::field::String<
        MyFieldBase,
        comms::option::SequenceSizeFieldPrefix<
            comms::field::IntValue<MyFieldBase, std::uint8_t>
        >        
    >
```

Optional 2 byte integer, default constructed as "missing".:
```cpp
using MyOptInt = 
    comms::field::Optional<
        comms::field::IntValue<MyFieldBase, std::uint16_t>,
        comms::option::DefaultOptionalMode<comms::field::OptionalMode::Missing>        
    >
```

# Defining Messages
Usually the message IDs are numeric values specified using an enum
```cpp
// Message ID
enum MsgId : std::uint16_t
{
    MsgId_Msg1,
    MsgId_Msg2,
    MsgId_Msg3,
    ...
};
```

The message message definition will usually like like this:
```cpp
// Fields used by Message1 (defined below)
using Message1Fields = 
    std::tuple<
        MyIntField,  
        MyBitmaskField
        MyRawDataList2
    >;
    
// The definition of Message1 message
template <typename TMsgBase> // Interface class passed as a template parameter
class Message1 : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Msg1>, // numeric message ID
        comms::option::FieldsImpl<Message1Fields>, // provide message fields
        comms::option::MsgType<Message1<TMsgBase> > // specify exact type of the message
    >
{
    // Provide names of the fields for convenient access
    COMMS_MSG_FIELDS_ACCESS(field1, field2, field3);
};
```

The definition of the message contents is common for any application. The 
generated code depends on the used message interface classes.

# Defining Interface
The interface definition is application specific. Every application defines
what polymorphic interface every message needs to define and implement:
```cpp
using App1Interface =
    comms::Message<
        comms::option::BigEndian, // Use big endian for serialisation
        comms::option::MsgType<MsgId>, // Provide type used for message ID
        comms::option::IdInfoInterface, // Support polymorphic retreival of message ID
        comms::option::ReadIterator<const std::uint8_t*>, // Support polymorphic read using "const std::uint8_t*" as iterator
        comms::option::WriteIterator<std::uint8_t*>, // Support polymorphic write using "std::uint8_t*" as iterator
        comms::option::LengthInfoInterface, // Support polymorphic retrieval of serialisation length
        comms::option::ValidCheckInterface, // Support polymorphic contents validity check
        comms::option::Handler<MyHandler> // Support dispatch to handling object of "MyHandler" type
    >;
```

Some other application may define different interface:
```cpp
using App2Interface =
    comms::Message<
        comms::option::BigEndian, // Use big endian for serialisation
        comms::option::MsgType<MsgId>, // Provide type used for message ID
        comms::option::IdInfoInterface, // Support polymorphic retreival of message ID
        comms::option::ReadIterator<const std::uint8_t*>, // Support polymorphic read using "const std::uint8_t*" as iterator
        comms::option::WriteIterator<std::back_insert_itetrator<std::vector<std::uint8_t> > >, 
                                                          // Support polymorphic write using
                                                          // "std::back_insert_itetrator<std::vector<std::uint8_t> > >" as iterator
        comms::option::Handler<MyOtherHandler> // Support dispatch to handling object of "MyOtherHandler" type
    >;
```

Note that definition of **Message1** class remains unchanged, every application
passes its chosen interface to implement the required functionality.

In app1:
```cpp
using Msg1 = Message1<App1Interface>; // will implement all the virtual functions required by app1
```

In app2:
```cpp
using Msg1 = Message1<App2Interface>; // will implement all the virtual functions required by app2
```

## Defining Transport Frames
The transport frames definition is also flexible and assembled out of layers.
For example, simple frame of just 2 bytes size followed by 2 byte message ID
will look like this:
```cpp
// Define field used to (de)serialise message id (see definition of MsgId enum earlier)
using MsgIdField = comms::field::EnumValue<MyFieldBase, MsgId>

// Define field used to (de)serialise remaining length of the message:
using MsgSizeField = comms::field::IntValue<MyFieldBase, std::uint16_t>

// Define transport frame by wrapping "layers"
using Frame = 
    comms::protocol::MsgSizeLayer< // The SIZE 
        MsgSizeField,
        comms::option::MsgIdLayer< // The ID 
            MsgIdField,
            comms::protocol::MsgDataLayer<> // The PAYLOAD
        >
    >;
```

The more complex transport consisting of SYNC, SIZE, ID, PAYLOAD, and CHECKSUM
may look like this:
```cpp

// Define field used to (de)serialise message id (see definition of MsgId enum earlier)
using MsgIdField = comms::field::EnumValue<MyFieldBase, MsgId>

// Define field used to (de)serialise remaining length of the message:
using MsgSizeField = comms::field::IntValue<MyFieldBase, std::uint16_t>

// Define checksum value field
using ChecksumField =
    comms::field::IntValue<
        MyFieldBase,
        std::uint16_t
    >;

// Define field used as synchronisation prefix
using SyncField =
    comms::field::IntValue<
        MyFieldBase,
        std::uint16_t,
        comms::option::DefaultNumValue<0xabcd>,
        comms::option::ValidNumValueRange<0xabcd, 0xabcd>
    >;

// Define transport frame by wrapping "layers"
using Frame = 
    comms::protocol::SyncPrefixLayer< // The SYNC
        SyncField,
        comms::protocol::ChecksumLayer // The CHECKSUM
            ChecksumField,
            comms::protocol::checksum::Crc_CCITT, // Use CRC-CCITT calculation
            comms::protocol::MsgSizeLayer< // The SIZE 
                MsgSizeField,
                comms::option::MsgIdLayer< // The ID 
                    MsgIdField,
                    comms::protocol::MsgDataLayer<> // The PAYLOAD
                >
            >
        >
    >;
```

