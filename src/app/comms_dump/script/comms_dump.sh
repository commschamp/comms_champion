#!/bin/bash

BIN_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CC_DIR=$( dirname ${BIN_DIR} )
LIB_DIR="${CC_DIR}/lib"

LD_LIBRARY_PATH=${LIB_DIR} ${BIN_DIR}/comms_dump $@

