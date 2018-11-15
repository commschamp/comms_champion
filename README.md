![Image: CommsChampion Icon](comms_champion/app/cc_view/src/image/app_icon.png)

# What It's All About?
Almost every electronic device/component nowadays has to be able to communicate
to other devices, components, or outside world over some I/O link. Such communication
is implemented using various communication protocols. The implementation of
these protocols can be a tedious, time consuming and error-prone process.
Therefore, there is a growing tendency among developers to use third party code 
generators for data (de)serialisation. Usually such tools receive description
of the protocol data structures in separate source file(s) with a custom grammar, 
and generate appropriate (de)serialisation code and necessary abstractions to 
access the data. The main problem with all these tools is that their major purpose
is data structures serialisation and/or facilitation of remote procedure calls (RPC).
The binary data layout and how the transferred data is going to be used is of much
lesser importance. The binary protocols, on the other hand, require different approach.
Their specification puts major emphasis on binary data layout, what values are
being transferred (units and/or scaling factor) and how the other end is expected 
to behave on certain values (what values are considered to be valid and how to
behave on reception of invalid values). It requires having some extra meta-information
attached to described data structures. The existing tools either don't have means
to specify such meta-information (other than in comments), or don't know what to
do with it when provided. As the result the developer still has to write a 
significant amount of boilerplate code in order to integrate the generated 
serialisation focused code to be used in binary communication protocol handling.

Also all of the schema based serialisation solutions have
**at least** one of the following limitations:

- Inability to specify binary data layout. Many of the tools use their own
serialisation format without an ability to provide custom one. It makes them
impossible to use to implement already defined and used binary communication
protocol.
- Inability to customise underlying types. Most (or all) of the available code 
generating tools, which do allow customisation of binary data layout, don't allow
usage of custom data structures. They usually
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
- Poor or weak description grammar without an ability to support conditional
(de)serialisation. For example, having a value
(such as single bit in some bitmask field) which determines whether some other
optional field exists or not. 
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

The generalisation is hard. As the result many embedded C++ developers still have 
to manually implement required communication protocol 
rather than relying on the existing tools for code generation.

This project is a core of **CommsChampion ecosystem", which comes to help in 
developing binary communication protocols, with main focus on
**embedded systems** with limited resources (including 
bare-metal ones) and choosing **C++(11)** programming language to do so. 
This project has two major parts: [COMMS Library](#comms-library) and 
[CommsChampion Tools](#commschampion-tools). The library's components
can be used to define protocol messages as well as its transport
framing information using mostly declarative statements of classes and types 
definitions. The tools can be used to visualise, analyse, and debug binary 
communication protocols, which were developed using [COMMS Library](#comms-library).

<span style="color:red">**IMPORTANT**</span>: Over the years the 
[COMMS Library](#comms-library) grew with features and accumulated
multiple nuances to be remembered when defining a new protocol. In order to
simplify protocol definition work, a separate toolset, called 
[commsdsl](https://github.com/arobenko/commsdsl) (hosted as separate repository), 
has been developed. It allows much easier and simpler definition of the protocol, 
using schema files written in XML based domain specific language, called 
[CommsDSL](https://github.com/arobenko/CommsDSL-Specification). The toolset
will generate a C++11 code that defines the protocol using appropriate
[COMMS Library](#comms-library) classes and functions, as well as generate extra code
required to implement a protocol definition plugin for 
[CommsChampion Tools](#commschampion-tools). Many binary protocols 
may have nuances that are difficult to express in existing schema language. 
In order to still allow usage of the schema files for the protocol definition, the
toolset allows injection of extra custom code to modify or extend the generated
one. The generated code itself is also highly compile time customisable. It
allows selection of custom data structures for data storage as well as polymorphic
interfaces relevant to the application being developed.

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
However, at this stage the library internals are much more advanced and sophisticated, than
examples in the e-book, please don't treat the latter as a guide to library's internals.

Full [doxygen](www.doxygen.org) generated documentation with the full tutorial inside can be
downloaded as **doc_comms.zip** archive from 
[release artefacts](https://github.com/arobenko/comms_champion/releases).

For quick usage examples please refer to [EXAMPLES.md](EXAMPLES.md).

# CommsChampion Tools
**CommsChampion** is a name for set of tool applications (in addition to the 
[COMMS Library](#comms-library)), which can be used to 
develop, monitor and debug custom binary communication protocols, that where
developed using the [COMMS Library](#comms-library). 
All the applications are plug-in based, i.e. plug-ins are used to define 
I/O socket, data filters, and the custom protocol itself. The tools
use [Qt5](http://www.qt.io/) framework for GUI interfaces as well as loading
and managing plug-ins.

The current list of available applications is below. Please refer to the
[wiki page](https://github.com/arobenko/comms_champion/wiki/How-to-Use-CommsChampion-Tools)
for tutorial on how to use them.

- **cc_view** is the main generic GUI application for visualisation and analysis of the
communication protocols.  

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
- **echo_socket** - Echo socket, all the data being sent is immediately reported
as an incoming data.
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
and demonstration purposes.

The demo protocol as well as [Other Available Protocols](#other-available-protocols) 
may serve as reference on how to implement any other custom binary protocol.

The [doxygen](www.doxygen.org) generated documentation, which includes the protocol
definition inside, can be
downloaded as **doc_demo.zip** archive
from [release artefacts](https://github.com/arobenko/comms_champion/releases).

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
The full tutorial as well as API documentation can be downloaded as
**doc_commschampion.zip** archive from
from [release artefacts](https://github.com/arobenko/comms_champion/releases).

# Licence
The [COMMS Library](#comms-library) is licensed under
the classic **GPLv3 / Commercial** dual licensing scheme. Please refer to 
[LICENSE.md](LICENSE.md) for details.

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

Instructions on how to include the **COMMS** library in other CMake projects 
can be found in 
[How to Use CommsChampion in CMake Projects](https://github.com/arobenko/comms_champion/wiki/How-to-Use-CommsChampion-in-CMake-Projects)
wiki page.

# How to Run CommsChampion Tools applications
On Windows platforms try to run the *.exe binary (**cc_view.exe**
or **cc_dump.exe**), which resides in 
**install/bin** subdirectory. If the execution fails due to missing **Qt5** dlls,
either set your **%PATH%** variable accordingly or try to execute generated **.bat**
files (**cc_view.bat** or **cc_dump.bat**) residing in the same directory.

On Linux platforms use the appropriate shell script 
(**cc_view.sh** or **cc_dump.sh**), which also resides in
**install/bin** subdirectory.
 
Please note that available plugins must reside in the **../lib/CommsChampion/plugin** subdirectory relative
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


