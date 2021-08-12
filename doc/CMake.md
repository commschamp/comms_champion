# Integration with Other CMake Project
There are multiple ways to use COMMS library in other projects.

## Pre-built COMMS library
In case the comms_champion was already built as an external project
it is recommended to use the following construct.
```
list (APPEND CMAKE_PREFIX_PATH /path/to/comms_champion/install/dir)
find_package(LibComms NO_MODULE)

if (NOT TARGET cc::comms)
    message (FATAL_ERROR "COMMS library is not found")
endif ()

# For libraries that use COMMS headers in their headers
target_link_libraries(my_protocol_library INTERFACE cc::comms)

# For binaries that use COMMS library
target_link_libraries(my_binary PRIVATE cc::comms)
```
If the package was successfully found, the target `cc::comms` needs 
to be defined. Linking to `cc::comms` target will add all the relevant
include paths.

## When external COMMS Library is not available
In case the comms_champion project is not available, the 
[cmake/CC_External.cmake](../cmake/CC_External.cmake) script provides
multiple macros / functions which could be used for easy 
integration. Please open [it](../cmake/CC_External.cmake) for 
the documentation on available macros / functions.

If the [comms_champion](https://github.com/commschamp/comms_champion) 
sources are not attached to the 
project being developed as a submodule, it is recommended to 
prefetch the sources to get an access to the 
[cmake/CC_External.cmake](../cmake/CC_External.cmake) script during
the cmake execution.

Please copy the [cmake/CC_Prefetch.cmake](../cmake/CC_Prefetch.cmake)
file to your project (or use it as an example) and then 
prefetch the 
[comms_champion](https://github.com/commschamp/comms_champion) 
sources using following (or similar) 
cmake code.
```
include (${PROJECT_SOURCE_DIR}/cmake/CC_Prefetch.cmake)
cc_prefetch(SRC_DIR /path/to/place/for/comms_champion)
```
After the prefetching, the provided 
[cmake/CC_External.cmake](../cmake/CC_External.cmake) becomes available.

### Build and install COMMS library during CMake configuration
When only COMMS library is required (neither unittests nor CommsChampion Tools are built), the 
build / install process is very light (just copying headers and cmake files to the destination directory), 
it is possible to perform it during CMake execution using `cc_build_during_config()` function.
```
include(/path/to/comms_champion/cmake/CC_External.cmake)

set (cc_install_dir ${PROJECT_BINARY_DIR}/comms_champion/install)
cc_build_during_config(
    SRC_DIR /path/to/comms_champion
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${cc_install_dir}
        -DCC_BUILD_UNIT_TESTS=OFF -DCC_BUILD_TOOLS_LIBRARY=OFF
)
```
The code above will install the COMMS library and all the relevant cmake 
scripts to the directory specified in `cc_install_dir` variable. Please 
remember to use default `CMAKE_INSTALL_PREFIX` variable to specify the destination.

After that it is possible to treat the COMMS library as being externally 
available and use `find_package()` described above.
```
list (APPEND CMAKE_PREFIX_PATH ${cc_install_dir})
find_package(LibComms NO_MODULE REQUIRED)
target_link_libraries(my_binary PRIVATE cc::comms)
```
### Build and install as external project
The CMake provides 
[ExternalProject_Add()](https://cmake.org/cmake/help/v3.0/module/ExternalProject.html)
function which can be used to build and install the comms_champion contents during the 
build process. The same [cmake/CC_External.cmake](../cmake/CC_External.cmake)
script provides different `cc_build_as_external_project()` function,
which provides a convenient interface to build the comms_champion is such way.
It is recommended to be used when the build process of the comms_champion is 
a bit heavy, i.e. building CommsChampion Tools as well.
```
include(/path/to/comms_champion/cmake/CC_External.cmake)
cc_build_as_external_project(
    SRC_DIR /path/to/comms_champion/sources
    BUILD_DIR ${PROJECT_BINARY_DIR}/comms_champion
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    CMAKE_ARGS 
        -DCC_BUILD_UNIT_TESTS=OFF
)
```
The CMake code above will create CMake target (default hidden name of which can be updated
using TGT parameter), which checks out this project from 
[github](https://github.com/commschamp/comms_champion) (the repo url can be updated
using REPO parameter). If this project sources are already checked out, as git submodule
for example, just pass NO_REPO parameter.
```
include(/path/to/comms_champion/cmake/CC_External.cmake)
cc_build_as_external_project(
    SRC_DIR /path/to/comms_champion/sources
    BUILD_DIR ${PROJECT_BINARY_DIR}/comms_champion
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    NO REPO                                    # No checkout, assume sources are in SRC_DIR
    CMAKE_ARGS 
        -DCC_BUILD_UNIT_TESTS=OFF
)
```

The invocation of the function above will also define `cc::comms` and `cc::comms_champion`
cmake targets, which can be used as link libraries.
```
target_link_libraries(my_app PRIVATE cc::comms)
target_link_libraries(my_cc_tools_plugin PRIVATE cc::comms cc::comms_champion)
```
