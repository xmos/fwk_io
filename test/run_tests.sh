#!/bin/bash
set -e

git config http.https://github.com/.extraheader AUTHORIZATION: basic ***
git config --unset-all url.https://github.com/.insteadOf
git config --add url.https://github.com/.insteadOf git@github.com:
git config --add url.https://github.com/.insteadOf org-292931@github.com:

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
    cd ${lib} && pytest -n 4 --junitxml="test_results.xml"
    popd
done

tests_end=`date +%s`

#****************************
# Check results
#****************************
pytest test_verify_results.py ${hil_test_libs[*]}

#****************************
# Display time results
#****************************
echo "************************"
echo "Test runtime: $((tests_end-tests_start))s"
echo "************************"
