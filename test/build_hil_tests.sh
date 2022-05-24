#!/bin/bash
set -e

FRAMEWORK_IO_ROOT=`git rev-parse --show-toplevel`

source ${FRAMEWORK_IO_ROOT}/tools/ci/helper_functions.sh

# perform builds
## TODO replace with build and running tests
path="${FRAMEWORK_IO_ROOT}/test"

echo '******************************************************'
echo '* Building HIL Tests'
echo '******************************************************'

(cd ${path}; ./build_lib_i2c_tests.sh)
(cd ${path}; ./build_lib_i2s_tests.sh)
(cd ${path}; ./build_lib_spi_tests.sh)
(cd ${path}; ./build_lib_uart_tests.sh)
