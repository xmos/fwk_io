#!/bin/bash
set -e

FRAMEWORK_IO_ROOT=`git rev-parse --show-toplevel`

source ${FRAMEWORK_IO_ROOT}/tools/ci/helper_functions.sh

# row format is: "make_target BOARD toolchain"
applications=(
    "test_hil_i2c_master_test_400_stop_0        XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_400_stop_1        XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_400_stop_2        XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_400_stop_3        XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_400_stop_4        XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_400_no_stop_0     XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_400_no_stop_1     XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_400_no_stop_2     XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_400_no_stop_3     XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_400_no_stop_4     XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_100_stop_0        XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_100_stop_1        XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_100_stop_2        XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_100_stop_3        XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_100_stop_4        XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_100_no_stop_0     XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_100_no_stop_1     XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_100_no_stop_2     XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_100_no_stop_3     XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_100_no_stop_4     XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_10_stop_0         XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_10_stop_1         XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_10_stop_2         XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_10_stop_3         XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_10_stop_4         XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_10_no_stop_0      XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_10_no_stop_1      XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_10_no_stop_2      XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_10_no_stop_3      XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_10_no_stop_4      XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_slave_test                    XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_test_locks                    XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_reg_test               XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_tx_only_stop      XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_master_test_tx_only_no_stop   XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
    "test_hil_i2c_test_repeated_start           XCORE-AI-EXPLORER    xmos_cmake_toolchain/xs3a.cmake"
)

# perform builds
for ((i = 0; i < ${#applications[@]}; i += 1)); do
    read -ra FIELDS <<< ${applications[i]}
    application="${FIELDS[0]}"
    board="${FIELDS[1]}"
    toolchain_file="${FRAMEWORK_IO_ROOT}/${FIELDS[2]}"
    path="${FRAMEWORK_IO_ROOT}"
    echo '******************************************************'
    echo '* Building' ${application} 'for' ${board}
    echo '******************************************************'

    (cd ${path}; rm -rf build_ci_${board})
    (cd ${path}; mkdir -p build_ci_${board})
    (cd ${path}/build_ci_${board}; log_errors cmake ../ -DCMAKE_TOOLCHAIN_FILE=${toolchain_file} -DBOARD=${board} -DFWK_IO_TESTS=ON; log_errors make ${application} -j)
done
