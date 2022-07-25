# Copyright 2015-2021 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
from i2s_master_checker import I2SMasterChecker, Clock
from pathlib import Path
import Pyxsim as px
import pytest

num_in_out_args = {"4ch_in,4ch_out": (4, 4),
                   "1ch_in,1ch_out": (1, 1),
                   "4ch_in,0ch_out": (4, 0),
                   "0ch_in,4ch_out": (0, 4)}

@pytest.mark.parametrize(("num_in", "num_out"), num_in_out_args.values(), ids=num_in_out_args.keys())
def test_i2s_basic_master_4b(build, capfd, nightly, request, num_in, num_out):
    test_level = "0" if nightly else "1"
    id_string = f"{test_level}_{num_in}_{num_out}"
    cwd = Path(request.fspath).parent
    binary = f'{cwd}/i2s_master_test/bin/test_hil_i2s_master_test_4b_{id_string}.xe'

    clk = Clock("tile[0]:XS1_PORT_1A")

    checker = I2SMasterChecker(
        "tile[0]:XS1_PORT_1B",
        "tile[0]:XS1_PORT_1C",
        ["tile[0]:XS1_PORT_4F.3","tile[0]:XS1_PORT_4F.2","tile[0]:XS1_PORT_4F.1", "tile[0]:XS1_PORT_4F.0"],
        ["tile[0]:XS1_PORT_4E.3","tile[0]:XS1_PORT_4E.2","tile[0]:XS1_PORT_4E.1", "tile[0]:XS1_PORT_4E.0"],
        "tile[0]:XS1_PORT_1L",
        "tile[0]:XS1_PORT_16A",
        "tile[0]:XS1_PORT_1M",
         clk,
         False) # Don't check the bclk stops precisely as the hardware can't do that

    tester = px.testers.PytestComparisonTester(f'{cwd}/expected/master_test.expect',
                                            regexp = True,
                                            ordered = True,
                                            ignore = ["CONFIG:.*?"])

    ## Temporarily building externally, see hil/build_lib_i2s_tests.sh
    # build(directory = binary,
    #         env = {"NUMS_IN_OUT":f'{num_in};{num_out}', "TEST_LEVEL":f'{test_level}'},
    #         bin_child = id_string)

    px.run_with_pyxsim(binary,
                        simthreads = [clk, checker])

    tester.run(capfd.readouterr().out)
