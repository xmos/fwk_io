#!/bin/bash
set -e

# Build tests in parallel.
# This will only spawn 1 build per logical processor,
# but never more than max_parallel_procs
max_parallel_procs=64
parallel=1

# Set this for debugging, allowing builds without performing each build
# in a separate cmake configuration
debug=1

FRAMEWORK_IO_ROOT=`git rev-parse --show-toplevel`

source ${FRAMEWORK_IO_ROOT}/tools/ci/helper_functions.sh

# setup configurations
if [ -z "$1" ] || [ "$1" == "all" ]
then
    # row format is: "make_target BOARD"
    applications=(
        "test_hil_spi_master_sync_multi_device_1_0_1_4_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_4_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_4_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_4_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_8_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_8_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_8_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_8_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_80_0   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_80_1   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_80_2   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_80_3   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_4_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_4_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_4_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_4_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_8_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_8_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_8_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_8_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_80_0   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_80_1   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_80_2   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_80_3   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_4_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_4_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_4_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_4_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_8_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_8_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_8_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_8_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_80_0   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_80_1   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_80_2   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_80_3   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_4_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_4_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_4_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_4_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_8_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_8_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_8_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_8_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_80_0   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_80_1   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_80_2   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_80_3   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_4_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_4_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_4_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_4_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_8_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_8_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_8_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_8_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_80_0   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_80_1   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_80_2   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_80_3   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_4_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_4_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_4_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_4_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_8_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_8_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_8_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_8_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_80_0   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_80_1   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_80_2   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_80_3   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_4_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_4_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_4_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_4_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_8_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_8_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_8_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_8_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_80_0          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_80_1          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_80_2          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_80_3          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_4_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_4_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_4_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_4_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_8_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_8_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_8_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_8_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_80_0          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_80_1          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_80_2          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_80_3          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_4_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_4_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_4_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_4_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_8_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_8_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_8_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_8_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_80_0          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_80_1          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_80_2          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_80_3          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_4_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_4_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_4_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_4_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_8_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_8_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_8_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_8_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_80_0          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_80_1          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_80_2          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_80_3          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_4_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_4_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_4_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_4_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_8_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_8_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_8_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_8_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_80_0          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_80_1          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_80_2          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_80_3          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_4_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_4_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_4_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_4_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_8_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_8_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_8_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_8_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_80_0          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_80_1          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_80_2          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_80_3          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_0_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_0_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_1_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_1_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_2_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_2_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_3_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_3_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_0_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_0_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_1_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_1_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_2_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_2_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_3_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_3_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_0_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_0_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_1_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_1_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_2_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_2_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_3_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_3_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_0_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_0_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_1_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_1_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_2_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_2_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_3_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_3_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_0_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_0_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_1_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_1_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_2_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_2_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_3_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_3_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_0_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_0_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_1_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_1_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_2_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_2_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_3_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_3_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
    )
elif [ "$1" == "master" ]
then
    # row format is: "make_target BOARD"
    applications=(
        "test_hil_spi_master_sync_multi_device_1_0_1_4_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_4_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_4_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_4_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_8_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_8_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_8_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_8_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_80_0   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_80_1   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_80_2   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_0_1_80_3   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_4_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_4_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_4_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_4_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_8_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_8_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_8_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_8_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_80_0   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_80_1   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_80_2   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_0_80_3   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_4_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_4_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_4_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_4_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_8_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_8_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_8_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_8_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_80_0   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_80_1   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_80_2   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_1_1_1_80_3   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_4_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_4_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_4_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_4_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_8_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_8_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_8_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_8_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_80_0   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_80_1   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_80_2   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_0_1_80_3   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_4_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_4_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_4_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_4_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_8_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_8_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_8_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_8_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_80_0   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_80_1   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_80_2   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_0_80_3   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_4_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_4_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_4_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_4_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_8_0    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_8_1    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_8_2    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_8_3    XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_80_0   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_80_1   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_80_2   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_multi_device_0_1_1_80_3   XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_4_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_4_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_4_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_4_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_8_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_8_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_8_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_8_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_80_0          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_80_1          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_80_2          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_0_1_80_3          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_4_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_4_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_4_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_4_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_8_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_8_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_8_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_8_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_80_0          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_80_1          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_80_2          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_0_80_3          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_4_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_4_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_4_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_4_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_8_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_8_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_8_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_8_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_80_0          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_80_1          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_80_2          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_1_1_1_80_3          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_4_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_4_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_4_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_4_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_8_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_8_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_8_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_8_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_80_0          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_80_1          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_80_2          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_0_1_80_3          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_4_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_4_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_4_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_4_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_8_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_8_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_8_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_8_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_80_0          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_80_1          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_80_2          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_0_80_3          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_4_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_4_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_4_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_4_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_8_0           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_8_1           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_8_2           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_8_3           XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_80_0          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_80_1          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_80_2          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_master_sync_rx_tx_0_1_1_80_3          XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
    )
elif [ "$1" == "slave" ]
then
    # row format is: "make_target BOARD"
    applications=(
        "test_hil_spi_slave_rx_tx_1_0_1_0_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_0_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_1_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_1_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_2_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_2_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_3_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_0_1_3_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_0_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_0_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_1_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_1_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_2_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_2_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_3_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_0_3_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_0_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_0_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_1_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_1_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_2_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_2_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_3_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_1_1_1_3_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_0_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_0_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_1_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_1_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_2_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_2_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_3_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_0_1_3_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_0_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_0_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_1_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_1_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_2_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_2_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_3_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_0_3_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_0_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_0_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_1_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_1_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_2_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_2_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_3_0                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
        "test_hil_spi_slave_rx_tx_0_1_1_3_1                 XCORE-AI-EXPLORER  xmos_cmake_toolchain/xs3a.cmake"
    )
else
    echo "Argument $1 not a supported configuration!"
    exit
fi

# helper to perform builds
do_build () {
    read -ra FIELDS <<< ${applications[i]}
    application="${FIELDS[0]}"
    board="${FIELDS[1]}"
    toolchain_file="${FRAMEWORK_IO_ROOT}/${FIELDS[2]}"
    path="${FRAMEWORK_IO_ROOT}"
    echo '******************************************************'
    echo '* Building' ${application} 'for' ${board} 
    echo '******************************************************'

    if [ "$debug" == "0" ]
    then
        (cd ${path}; rm -rf build_ci_${application}_${board})
        (cd ${path}; mkdir -p build_ci_${application}_${board})
        if [ "$parallel" == "0" ]
        then
            (cd ${path}/build_ci_${application}_${board}; log_errors cmake ../ -DCMAKE_TOOLCHAIN_FILE=${toolchain_file} -DBOARD=${board} -DFWK_IO_TESTS=ON; log_errors make ${application} -j)
        else
            (cd ${path}/build_ci_${application}_${board}; log_errors cmake ../ -DCMAKE_TOOLCHAIN_FILE=${toolchain_file} -DBOARD=${board} -DFWK_IO_TESTS=ON; log_errors make ${application})
        fi
    else
        (cd ${path}; mkdir -p build_ci_${application}_${board})
        if [ "$parallel" == "0" ]
        then
            (cd ${path}/build_ci_${application}_${board}; log_errors cmake ../ -DCMAKE_TOOLCHAIN_FILE=${toolchain_file} -DBOARD=${board} -DFWK_IO_TESTS=ON; log_errors make ${application} -j)
        else
            (cd ${path}/build_ci_${application}_${board}; log_errors cmake ../ -DCMAKE_TOOLCHAIN_FILE=${toolchain_file} -DBOARD=${board} -DFWK_IO_TESTS=ON; log_errors make ${application})
        fi
    fi
}

# perform builds
if [ "$parallel" == "0" ]
then
    for ((i = 0; i < ${#applications[@]}; i += 1)); do
        do_build "$i"
    done
else
    cpu_cnt=$([ $(uname) = 'Darwin' ] && 
            sysctl -n hw.logicalcpu_max || 
            lscpu -p | egrep -v '^#' | wc -l)
    if (($cpu_cnt > $max_parallel_procs));
    then
        num_par_procs=$max_parallel_procs
    else
        num_par_procs=$cpu_cnt
    fi
    if (($num_par_procs == 0));
    then
        num_par_procs=1
    fi
    
    running=0
    built_cnt=0
    # run build processes and store pids in array
    for ((i = 0; i < ${#applications[@]}; i += 1)); do
        do_build "$i" &
        pids[${i}]=$!
        running=$((running+1))
        if (((($running == $num_par_procs)) || ((i+1 == ${#applications[@]}))));
        then
            for pid in ${pids[*]}; do
                wait $pid
            done
            built_cnt=$((built_cnt+running))
            echo $built_cnt 'of' ${#applications[@]} 'builds complete'
            running=0
        fi
    done
fi
