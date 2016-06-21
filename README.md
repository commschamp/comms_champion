![Image: CommsChampion Icon](src/app/comms_champion/src/image/app_icon.png)

# What It's All About?
Almost every electronic device/component nowadays has to be able to communicate
to other devices, components, or outside world over some I/O link. Such communication
is implemented using various communication protocols. The implementation of
these protocols can be a tedious, time consuming and error-prone process.

As a whole, most of the communication protocols are very similar, they define
various messages with their internal fields, define serialisation rules
for all the fields and wrap them in some kind of transport information to ensure
safe delivery of the message over the I/O link. However, it is very difficult
to generalise development of the communication protocols and provide any 
library and/or tools that can be used in most of the development cases
and runtime environments. The protocols may use different serialisation rules 
(endian), different incoming message logic, even the same protocol may have
different rules for wrapping message data based on the I/O link used to communicate
this data. That's why most developers don't even try to develop something 
generic up front and reuse it with different communication protocols.
As the result they experience a *deja-vu* feeling, that they have done it before, every time they
have to implement a new protocol. 

With this project I take up the challenge to create a library as well as
multiple analysis tools that will make the development of any communication
protocol much easier and quicker process. The development is done using C++
programming language with usage of features and/or STL components introduced
in **C++11** standard.

# What's Inside?
This repository provides the [COMMS Library](#comms-library), which can be used to 
develop custom communication protocols. It also provides a set of various applications and 
utilities, called [CommsChampion](#commschampion-tools), that can help in 
protocol development, visualisation and analysis.

# COMMS Library
**COMMS** is the headers only, platform independent library, 
which makes the implementation of a communication
protocol an easy and relatively quick process. It provides all the necessary
types and classes to make the definition of the custom messages, as well as
wrapping transport data fields, to be simple declarative statements of type and
class definitions. These statements will specify **WHAT** needs to be implemented. 
The **COMMS** library internals handle the **HOW** part.

The internals of the **COMMS** library is mostly template classes which use 
multiple meta-programming techniques. As the result, only the functionality,
required by the protocol being developed, gets compiled in, providing the best code size and
speed performance possible. The down side is that compilation process may
take a significant amount of time and consume a lot of memory.

The **COMMS** library was specifically developed to be used in **embedded** systems
including **bare-metal** ones. It doesn't use exceptions and/or RTTI. It also
minimises usage of dynamic memory allocation and provides an ability to exclude
it altogether if required, which may be needed when developing **bare-metal**
embedded systems. 

Core ideas and architecture of the **COMMS** library is described in
[Guide to Implementing Communication Protocols in C++](https://www.gitbook.com/book/arobenko/comms-protocols-cpp/details) free e-book.

Full [doxygen](www.doxygen.org) generated documentation with the full tutorial inside can be
downloaded as [zip archive](https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/doc_comms.zip)
or browsed online [here](https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/index.html).

# CommsChampion Tools
**CommsChampion** is a name for set of tool applications, which can be used to 
develop, monitor and debug custom binary communication protocols. 
All the applications are plug-in based, i.e. plug-ins are used to define 
I/O socket, data filters, and the custom protocol itself. The tools
use [QT5](http://www.qt.io/) framework for GUI interfaces as well as loading
and managing plug-ins.

The current list of available applications is:

- **comms_champion** is the main generic GUI application for visualisation and analysis of the
communication protocols, that were developed using [COMMS Library](#comms-library)
mentioned above.  

- **comms_dump** is a command line utility, that recognises all the received
custom binary protocol messages and dumps them all in CSV format to standard output.
It can also record the incoming message into the file, which can be opened
lately for visual analysis using **comms_champion** GUI application. 
The tool has an ability to receive a file with definition of outgoing messages, 
created using **comms_champion** GUI application, and send them one by one in
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
- **raw_data_protocol** - Protocol definition that defines only a single message
type with one field of unlimited length data. It can be used to review the
raw data being received from I/O socket.

# Demo Protocol
**Demo** is a simple binary protocol which is implemented using 
the [COMMS Library](#comms-library). The protocol definition classes are
also extended to implement the protocol plugin for
the [CommsChampion](#commschampion-tools) application. The plugin is used for testing
and demostration purposes.

The [doxygen](www.doxygen.org) generated documentation, which includes the protocol
definition inside, can be
downloaded as [zip archive](https://dl.dropboxusercontent.com/u/46999418/comms_champion/demo/doc_demo.zip)
or browsed online [here](https://dl.dropboxusercontent.com/u/46999418/comms_champion/demo/html/index.html).

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

# Licence
The [COMMS Library](#comms-library) from this repository is licensed under
**GPLv3**. It can be used in any product as long as its sources remain open and
have the same or other compatible license. As the author and full copyright
owner of this product I can provide a commercial license as well, please refer
to [Contact Information](#contact-information) below and get in touch with
me if you need one.

The [CommsChampion](#commschampion-tools) tools and 
[Demo Protocol](#demo-protocol) are licensed under **LGPLv3**, which
allows usage of open source QT5 libraries. 

Some icons, used in [CommsChampion](#commschampion-tools) tools, were taken from
[Fat Cow](http://www.fatcow.com/free-icons) and the license of the latter
applies.

The [application icon](src/app/comms_champion/src/image/app_icon.png) of the
[CommsChampion](#commschampion-tools) tool must
be replaced in any diravative work to differentiate between the original and
the forked versions.

# How to Build
Details instructions on how to build and install all the components can be
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
On Windows platforms just run the executable binary (**comms_champion.exe**
or **comms_dump.exe**), which resides in 
**install/bin** subdirectory. Make sure that the directory of your QT5 dlls can 
be found in your %PATH%.

On Linux platforms use the appropriate shell script 
(**comms_champion.sh** or **comms_dump.sh**), which also resides in
**install/bin** subdirectory.
 
Please note that available plugins must reside in the **../plugin** subdirectory relative
to the location of the binaries.

The tools support multiple command line options, please use "-h" or "--help" for
the full list.

>$> ./install/bin/comms_champion.sh -h

>$> ./install/bin/comms_dump.sh -h

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


