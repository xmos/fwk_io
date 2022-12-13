#!/usr/bin/env python
# Copyright 2015-2022 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
from spi_slave_checker_intertransaction import SPISlaveIntertransactionChecker
from pathlib import Path
import Pyxsim as px
import pytest

# This test is intended to quantify the minimum spi slave
# intertransaction timing parameter.
#
# This test is always supposed to "fail", and will output
# the last successful intertransaction duration
#
# For developers, run
# pytest -n 4 --junitxml="test_results.xml"

DEBUG_MODE = 0

mode_args = {"mode_0": 0,
             "mode_1": 1,
             "mode_2": 2,
             "mode_3": 3}

in_place_args = {"in_place": 1}

mosi_enabled_args = {"mosi_enabled": 1}

miso_enabled_args = {"miso_disabled": 0,
                    "miso_enabled": 1}

full_load_args = {"fully_loaded": 1}

# If mosi disabled, deselect the test
def uncollect_if(mode, full_load, mosi_enabled, miso_enabled, in_place):
    if not (mosi_enabled):
        return True

@pytest.mark.uncollect_if(func=uncollect_if)
@pytest.mark.parametrize("mode", mode_args.values(), ids=mode_args.keys())
@pytest.mark.parametrize("in_place", in_place_args.values(), ids=in_place_args.keys())
@pytest.mark.parametrize("mosi_enabled", mosi_enabled_args.values(), ids=mosi_enabled_args.keys())
@pytest.mark.parametrize("miso_enabled", miso_enabled_args.values(), ids=miso_enabled_args.keys())
@pytest.mark.parametrize("full_load", full_load_args.values(), ids=full_load_args.keys())
def test_spi_slave_intertransaction(build, capfd, request, full_load, miso_enabled, mosi_enabled, in_place, mode):
    id_string = f"{full_load}_{miso_enabled}_{mosi_enabled}_{mode}_{in_place}"

    cwd = Path(request.fspath).parent

    binary = f"{cwd}/spi_slave_intertransaction/bin/test_hil_spi_slave_intertransaction_{id_string}.xe"

    checker = SPISlaveIntertransactionChecker("tile[0]:XS1_PORT_1C",
                              "tile[0]:XS1_PORT_1D",
                              "tile[0]:XS1_PORT_1A",
                              "tile[0]:XS1_PORT_1B",
                              "tile[0]:XS1_PORT_1E",
                              "tile[0]:XS1_PORT_16B",
                              "tile[0]:XS1_PORT_1F",
                              "tile[0]:XS1_PORT_1G",
                              3000000,
                              10000)

    tester = px.testers.PytestComparisonTester(f'{cwd}/expected/slave_intertransaction.expect',
                                            regexp = True,
                                            ordered = True,
                                            suppress_multidrive_messages = False)

    ## Temporarily building externally, see hil/build_lib_spi_tests.sh
    # build(directory = binary,
    #         env = {"FULL_LOAD":f'{full_load}',
    #                "MISO_ENABLED":f'{miso_enabled}',
    #                "MOSI_ENABLED":f'{mosi_enabled}',
    #                "SPI_MODE":f'{mode}',
    #                "IN_PLACE":f'{in_place}'},
    #         bin_child = id_string)

    if DEBUG_MODE:
        with capfd.disabled():
            px.run_with_pyxsim(binary,
                               simthreads = [checker]
                               )
    else:
        px.run_with_pyxsim(binary,
                           simthreads = [checker]
                           )
        out, err = capfd.readouterr()
        f_out = open(f'spi_slave_intertransaction_results_{id_string}.txt','w')
        f_out.write(out)
        f_out.close()

        tester.run(out)
