This directory contains tests that were ported to use
[catch2](https://github.com/catchorg/Catch2) as its testing
infrastructure. Unfortunatelly the **catch2** turns out to 
contains some inner bugs which result in SegFaults when 
compiled by the MSVC for x86 (32 bit) architecture. 

The porting work has been moved here for storage maybe for 
future use when the problems are fixed. Once the 
[catch2](https://github.com/catchorg/Catch2) is reliable enough
they will be moved up.
