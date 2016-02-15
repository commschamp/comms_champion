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
develop custom communication protocols and one generic protocol visualisation and analysis GUI tool
called [CommsChampion](#commschampion-tool)

# COMMS Library
**COMMS** is the headers only platform independent library that makes the implementation of a communication
protocol an easy and relatively quick process. It provides all the necessary
types and classes to make the definition of the custom messages as well as
wrapping transport data fields to be simple declarative statements of type and
class definitions, which specify **WHAT** needs to be implemented. The **COMMS**
library internals handle the **HOW** part. 

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

Full [doxygen](www.doxygen.org) generated documentation with the full tutorial inside can be
downloaded as [zip archive](https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/doc_comms.zip)
or browsed online [here](https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/index.html).

# CommsChampion Tool
**CommsChampion** is a generic GUI tool for visualisation and analysis of the
communication protocols that were developed using [COMMS Library](#comms-library)
mentioned above. It is plug-in based, i.e. plug-ins are used to
define I/O socket, data filters, and the custom protocol itself. The tool
uses [QT5](http://www.qt.io/) framework for its interface as well as loading
and managing plug-ins.

At this moment the tool is in its alpha and being extensively developed. No 
documentation on how to use and/or develop plug-ins is currently available,
will be provided in the future once the API stabilises.

# Available Protocols
The [COMMS Library](#comms-library) just provides an infrastructure for
implementation of various communication protocols and 
the [CommsChampion](#commschampion-tool) tool just provides consistent UI 
environment to be able to visually analyse communication protocols that were
developed using the [COMMS Library](#comms-library).

There is [comms_all_protocols](https://github.com/arobenko/comms_all_protocols)
project that serves as a bundle to compile all the communication protocols, that
where developed using the [COMMS Library](#comms-library), and relevant plugins
for [CommsChampion](#commschampion-tool) tool, all at once. The README file
of this project contains the updated list of all the protocols that have been
implemented and can be used as reference.

# Licence
The [COMMS Library](#comms-library) from this repository is licensed under
GPLv3. It can be used in any product as long as its sources remain open and
have the same or other compatible license. As the author and full copyright
owner of this product I can provide a commercial license as well, please refer
to [Contact Information](#contact-information) below and get in touch with
me if you need one.

The [CommsChampion](#commschampion-tool) tool is licensed under LGPLv3, which
allows usage of open source QT5 libraries. 

# How to Build
This project uses [CMake](https://cmake.org) cross-platform build system to
generate required build files native to the platform.

- Create build directory somewhere and navigate there.

>$> mkdir /some/build/dir && cd /some/build/dir

- Generate Makefiles (or any other build environment) for **Release** build.

>$> cmake -DCMAKE_BUILD_TYPE=Release /path/to/comms_champion/sources

- Build and install.

>$> make install

After the build is complete, all the binaries, headers, libraries will reside
in **install** subdirectory of the directory chosen for build (/some/build/dir) .

In addition to built-in options/variables of CMake, such as **CMAKE_BUILD_TYPE** or
**CMAKE_TOOLCHAIN_FILE**, the following ones can be used:

- **CC_COMMS_LIB_ONLY**=ON/OFF - Exclude compilation of all the tools, install only
**COMMS** Library. Default value is **OFF**, i.e. other tools get built.

- **CC_COMMS_UNIT_TESTS**=ON/OFF - Include/Exclude unittests of the **COMMS** library.
Default value is **OFF**

- **CC_COMMS_CHAMPION**=ON/OFF - Enable/Disable build of CommsChampion tool. Default value
is **ON**.

- **CC_STATIC_RUNTIME**=ON/OFF - Enable/Disable static link to C++ runtime. Default value is **OFF**

- **CC_QT_DIR**=dir - Directory of QT5 installation. Can be used to provide path to QT5 if
differs from system default installation path.

For example, discard all other tools, just install the **COMMS** library:

>$> cd /path/to/comms_champion/sources

>$> mkdir build && cd build

>$> cmake -DCMAKE_BUILD_TYPE=Release -DCC_COMMS_LIB_ONLY=ON ..

>$> make install 

The example above will skip build of any tool available, it will just install 
the **COMMS** library headers in **install/include** subdirectory

Another example is to build all the available tools and unit-tests:

>$> cd /path/to/comms_champion/sources

>$> mkdir build && cd build

>$> cmake -DCMAKE_BUILD_TYPE=Release -DCC_COMMS_UNIT_TESTS=ON ..

>$> make install 

The built **CommsChampion** tool binary will reside in
**install/bin** subdirectory and available
plugins for it will reside in **install/plugin** subdirectory.

**NOTE**, that **master** branch will always point to the latest release, the
development is performed on **develop** branch. As the result it is safe
to just clone the sources of this repository and use it without
any extra manipulations of looking for latest version among the tags and
checking it out.

## Building Documentation
The documentation is not created during normal build process. The documentation of
the [COMMS Library](#comms-library) can be generated by building **doc_comms**
target:
 
>$> make doc_comms

The HTML documentation will reside in **install/doc/comms/html** subdirectory.

**NOTE**, that you must have 
[Doxygen](www.doxygen.org) 
and [Dia](https://wiki.gnome.org/Apps/Dia) 
being installed on your system in order to be able to properly generate the 
documentation.

# How to Use COMMS Library
As was mentioned earlier, the **COMMS** library is a headers only one, just
have /path/to/comms_champion/install/dir/**include** directory among your
include paths and use the following statement in your sources:

```
#include "comms/comms.h"
```
Nothing else is required.

# How to Run CommsChampion
On windows platforms just run **comms_champion.exe** binary which resides in 
**install/bin** subdirectory.

On Linux platforms use **comms_champion.sh** script which also resides in
**install/bin** subdirectory.
 
Please note that available plugins must reside in the **../plugin** subdirectory relative
to the location of the **comms_champion** binary.

The tool supports multiple command line options, please use "-h" or "--help" for
the full list.

>$> ./install/bin/comms_champion.sh -h

# Contact Information
For bug reports, feature requests, or any other question you may open an issue
here in **github** or e-mail me directly to: **arobenko@gmail.com**


