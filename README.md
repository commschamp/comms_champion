![Image: CommsChampion Icon](comms_champion/app/cc_view/src/image/app_icon.png)

# What It's All About?
Almost every electronic device/component nowadays has to be able to communicate
to other devices, components, or outside world over some I/O link. Such communication
is implemented using various communication protocols. The implementation of
these protocols can be a tedious, time consuming and error-prone process.
Therefore, there is a growing tendency among developers to use third party code 
generators for data (de)serialisation. Usually such tools receive description
of the protocol data layout in separate source file(s) with custom grammar, 
and generate appropriate (de)serialisation code and necessary abstractions to 
access the data. 

There are so many of them: 
[ProtoBuf](https://developers.google.com/protocol-buffers/), 
[Cap'n Proto](https://capnproto.org/), [MessagePack](http://msgpack.org/index.html),
[Thrift](https://thrift.apache.org/), [Kaitai Struct](http://kaitai.io/),
[Protlr](https://www.protlr.com/), you-name-it...
Most (or all) of such tools focus on being able to generate the
necessary code for as many programming languages as possible. 
However, focusing on the amount of supported programming languages rather than
on configuration variants of a specific language creates a situation where
the description grammar is quite poor without an ability to add extra conditional 
logic on how the data fields need to be serialised. For example, having a value
(such as single bit in some bitmask field) which determines whether some other
optional field exists or not. 

All of the mentioned above tools are capable of generating **C++** code. However,
the generated code quite often is not good enough be used in embedded systems, especially
bare-metal ones. Either the produced **C++** code or the tool itself has 
**at least** one of the following deficiencies:

- Inability to specify binary data layout. Many of the tools use their own
serialisation format without an ability to provide custom one. It makes them
impossible to use to implement already defined and used binary communication
protocol.
- Inability to customise underlying types. Most (or all) of the mentioned code 
generating tools, which do allow customisation of binary data layout,
choose to use **std::string** for string fields and/or 
**std::vector** for lists, as well as (de)serialisation code is generated to use 
standard streams (**std::istream** and **std::ostream**). Even if such ability
is provided, it is usually "global" one and do not allow substitution of types only for
specific messages / fields.
- Small number of supported data fields or limited number of their serialisation options.
For example, strings can be serialised by being prefixed with their size
(which in turn can have different lengths), or being terminated with '\0', or
having fixed size with '\0' padding if the string is too short. There are 
protocols that use all three variants of strings.
- Lack of polymorphic interface to allow implementation of the common code for all the 
defined messages.
- When polymorphic interface with virtual functions is provided, there is no
way to exclude generation of unnecessary virtual functions for a particular embedded application.
All the provided virtual functions will probably remain in the final image even
if they are not used.
- Lack of efficient built-in way of dispatching the deserialised message object into 
its appropriate handling function. There is a need to provide a separate 
dispatch table or map from message ID to some callback function or object.
- Lack of ability to override or complement the generated serialisation code with the manually
written one where extra logic is required.

This project comes to resolve all the problems listed above, but focusing on
**embedded systems** with limited resources (including bare-metal ones). It
keeps the idea of having "single source of truth" (i.e. single implementation) for
all the applications, but approaches the problem from a different angle. Instead,
of having separate message definition file(s) with a custom grammar, the messages
are defined using **C++** programming language, which is widely used in
embedded systems development. 

The idea is to have a library (see [COMMS Library](#comms-library) below), that
provide all the necessary, highly configurable C++ classes. The messages 
themselves and their fields are defined using simple declarative types and 
class definition statements which specify **WHAT** needs to be implemented. 
The **COMMS** library internals handle the **HOW** part. Thanks to the heavy
use of templates and multiple meta-programming techniques, only the needed code
gets generated and compiled. The polymorphic common interfaces are highly 
configurable. The functionality they need to provide is defined using
template parameters. As the result, the C++ compiler itself becomes a code
generating tool.

This project also provides a set of plug-in based applications, 
(see [CommsChampion Tools](#commschampion-tools) below), that come to help to
visualise and analyse protocols defined using provided 
[COMMS Library](#comms-library). The developed protocol plugins 
reuse the same message definitions code that was initially developed for the
embedded application itself.

# What's Inside?
This repository provides the [COMMS Library](#comms-library), which can be used to 
develop custom communication protocols. It also provides a set of various applications and 
utilities, called [CommsChampion](#commschampion-tools), that can help in 
protocol development, visualisation and analysis.

# COMMS Library
**COMMS** is the **C++(11)** headers only, platform independent library, 
which makes the implementation of a communication
protocol to be an easy and relatively quick process. It provides all the necessary
types and classes to make the definition of the custom messages, as well as
wrapping transport data fields, to be simple declarative statements of type and
class definitions. These statements will specify **WHAT** needs to be implemented. 
The **COMMS** library internals handle the **HOW** part.

The internals of the **COMMS** library is mostly template classes which use 
multiple meta-programming techniques. As the result, only the functionality,
required by the protocol being developed, gets compiled in, providing the best code size and
speed performance possible. The down side is that compilation process may
take a significant amount of time and consume a lot of memory.

The **COMMS** library allows having **single implementation** of the binary 
protocol messages, which can be re-compiled and used for any possible application:
bare-metal with constrained resources, Linux based embedded systems, even 
independent GUI analysis tools.

The **COMMS** library was specifically developed to be used in **embedded** systems
including **bare-metal** ones. It doesn't use exceptions and/or RTTI. It also
minimises usage of dynamic memory allocation and provides an ability to exclude
it altogether if required, which may be needed when developing **bare-metal**
embedded systems. 

Core ideas and architecture of the **COMMS** library is described in
[Guide to Implementing Communication Protocols in C++](https://www.gitbook.com/book/arobenko/comms-protocols-cpp/details) free e-book.

Full [doxygen](www.doxygen.org) generated documentation with the full tutorial inside can be
downloaded as [zip archive](https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/doc_comms.zip).

# CommsChampion Tools
**CommsChampion** is a name for set of tool applications, which can be used to 
develop, monitor and debug custom binary communication protocols. 
All the applications are plug-in based, i.e. plug-ins are used to define 
I/O socket, data filters, and the custom protocol itself. The tools
use [QT5](http://www.qt.io/) framework for GUI interfaces as well as loading
and managing plug-ins.

The current list of available applications is:

- **cc_view** is the main generic GUI application for visualisation and analysis of the
communication protocols, that were developed using [COMMS Library](#comms-library)
mentioned above.  

- **cc_dump** is a command line utility, that recognises all the received
custom binary protocol messages and dumps them all in CSV format to standard output.
It can also record the incoming message into the file, which can be opened
later for visual analysis using **cc_view** GUI application. 
The tool has an ability to receive a file with definition of outgoing messages, 
created using **cc_view** GUI application, and send them one by one 
in parallel to dumping/recording the incoming messages.

The [CommsChampion Tools](#commschampion-tools) package provides the following
plugins that can be used with any application:

- **null_socket** - NULL socket, that doesn't produce any incoming data and
discards any outgoing data.
- **echo_socket** - Echo socket, all the data being sent immediatelly reports
as incoming data.
- **serial_socket** - Low level socket that sends and receives data over serial
(RS-232) I/O link.
- **tcp_client_socket** - Client TCP/IP socket, that connects to remote 
server, sends and receives data over TCP/IP network link.
- **tcp_server_socket** - Server TCP/IP socket, waits for and accepts all
connections from TCP/IP clients, sends and receives data to/from them.
- **tcp_proxy_socket** - Proxy server TCP/IP socket, combines Server and Client
side of TCP/IP connection, can be used to monitor traffic of the messages between
remote a client and a server.
- **udp_socket** - Generic (client/server) UDP/IP socket.
- **raw_data_protocol** - Protocol definition that defines only a single message
type with one field of unlimited length data. It can be used to review the
raw data being received from I/O socket.

# Demo Protocol
**Demo** is a simple binary protocol which is implemented using 
the [COMMS Library](#comms-library). The protocol definition classes are
also extended to implement the protocol plugin for
the [CommsChampion](#commschampion-tools) application. The plugin is used for testing
and demostration purposes.

The demo protocol as well as [Other Available Protocols](#other-available-protocols) 
may serve as reference on how to implement any other custom binary protocol.

The [doxygen](www.doxygen.org) generated documentation, which includes the protocol
definition inside, can be
downloaded from [here](https://dl.dropboxusercontent.com/u/46999418/comms_champion/demo/doc_demo.zip).

# Other Available Protocols
The [COMMS Library](#comms-library) just provides an infrastructure for
implementation of various communication protocols and 
the [CommsChampion](#commschampion-tools) tools just provide consistent 
environment to be able to analyse and debug communication protocols, that were
developed using the [COMMS Library](#comms-library).

There is [comms_all_protocols](https://github.com/arobenko/comms_all_protocols)
project that serves as a bundle to compile all the communication protocols, that
where developed using the [COMMS Library](#comms-library), and relevant plugins
for [CommsChampion](#commschampion-tools) tools, all at once. The README file
of this project contains the updated list of all the protocols that have been
implemented and can be used as reference.

# Developing Custom Socket/Filter/Protocol Plugin
At this moment the [CommsChampion](#commschampion-tools) tools are in their alpha 
and being extensively developed. No 
documentation on how to use and/or develop plug-ins is currently available,
will be provided in the future once the API stabilises.

However, if you have tried [CommsChampion Tools](#commschampion-tools) with
any other binary protocol and would like to develop a plugin for your own,one
developed internaly by your complany, please get in touch 
(see [Contact Information](#contact-information) below). I will be able to help.

# Licence
The [COMMS Library](#comms-library) from this repository is licensed under
the classic **GPLv3 / Commercial** dual licensing scheme. The
source code is available for anyone to use as long as the derivative work
remains open source with compatible licence. Download it, try it! If it works
as expected and commercial closed source licence is required for the final
product, please send me an e-mail. As the author and full copyright owner I 
will be able to provide one. Please refer
to [Contact Information](#contact-information) below and get in touch with
me if you need one.

The [CommsChampion](#commschampion-tools) tools and 
[Demo Protocol](#demo-protocol) are licensed under **LGPLv3**, which
allows usage of open source QT5 libraries. 

Some icons, used in [CommsChampion](#commschampion-tools) tools, were taken from
[Fat Cow](http://www.fatcow.com/free-icons) and the license of the latter
applies.

The [application icon](src/app/cc_view/src/image/app_icon.png) of the
[CommsChampion](#commschampion-tools) tool must
be replaced in any derivative work to differentiate between the original and
the forked versions.

# How to Build
Detailed instructions on how to build and install all the components can be
found in [BUILD.md](BUILD.md) file.

# How to Use COMMS Library
As was mentioned earlier, the **COMMS** library is a headers only one, just
have /path/to/comms_champion/install/dir/**include** directory among your
include paths and use the following statement in your sources:

```
#include "comms/comms.h"
```
Nothing else is required.

# How to Run CommsChampion applications
On Windows platforms try run the executable binary (**cc_view.exe**
or **cc_dump.exe**), which resides in 
**install/bin** subdirectory. If the execution fails due to missing QT5 dlls,
either set your %PATH% variable accordingly or try to execute generated **.bat**
files (**cc_view.bat** or **cc_dump.bat**) residing in the same directory.

On Linux platforms use the appropriate shell script 
(**cc_view.sh** or **cc_dump.sh**), which also resides in
**install/bin** subdirectory.
 
Please note that available plugins must reside in the **../plugin** subdirectory relative
to the location of the binaries.

The tools support multiple command line options, please use "-h" or "--help" for
the full list.

>$> ./install/bin/cc_view.sh -h

>$> ./install/bin/cc_dump.sh -h

# Branching Model
This repository will follow the 
[Successful Git Branching Model](http://nvie.com/posts/a-successful-git-branching-model/).

The **master** branch will always point to the latest release, the
development is performed on **develop** branch. As the result it is safe
to just clone the sources of this repository and use it without
any extra manipulations of looking for the latest stable version among the tags and
checking it out.

# Contact Information
For bug reports, feature requests, or any other question you may open an issue
here in **github** or e-mail me directly to: **arobenko@gmail.com**


