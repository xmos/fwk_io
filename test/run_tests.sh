#!/bin/bash
set -e

if [ -f /.dockerenv ]; then
    # Docker workaround for: "fatal: detected dubious ownership in repository"
    git config --global --add safe.directory /fwk_io
fi

if [ -z "$1" ] || [ "$1" == "all" ]
then
    declare -a hil_test_libs=(
        "lib_uart"
        "lib_i2c"
        "lib_i2s"
        "lib_spi"
        )
else
    declare -a hil_test_libs=(
        "$1"
        )
fi

#****************************
# Run tests and copy results
#****************************
tests_start=`date +%s`
tools_ver=`xcc --version`
echo "Using tools_ver ${tools_ver}"

for lib in ${hil_test_libs[@]}; do
    pushd .
    echo "************************"
    echo "* Building ${lib} tests *"
    echo "************************"
    ./build_${lib}_tests.sh
    echo "************************"
    echo "* Running ${lib} tests *"
    echo "************************"
    #https://docs.github.com/en/actions/using-github-hosted-runners/about-github-hosted-runners
    #Looks like runners are 2 cores (maybe 2 HT) so run 4 at a time for speedup
    pytest -n 4 --junitxml="test_results_${lib}.xml" ${lib}/test_*.py
    popd
done

tests_end=`date +%s`

#****************************
# Check results
#****************************
for lib in ${hil_test_libs[@]}; do
    pytest test_verify_results.py --lib_name ${lib}
done

#****************************
# Display time results
#****************************
echo "************************"
echo "Test runtime: $((tests_end-tests_start))s"
echo "************************"
