# Copyright 2023 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
from tdm_slave_checker import TDMSlaveTX16Checker
from pathlib import Path
import Pyxsim
import pytest

DEBUG = True
# DEBUG = False

tx_offset_args = {"0": (0),
                  "1": (1),
                  "2": (2)}

@pytest.mark.parametrize(("tx_offset"), tx_offset_args.values(), ids=tx_offset_args.keys())
def test_tdm_slavetx16_cb(capfd, request, nightly, tx_offset):
    id_string = f"{tx_offset}"

    cwd = Path(request.fspath).parent
    binary = f'{cwd}/tdm_slave_tx16_cb_test/bin/test_hil_i2s_tdm_tx16_slave_test_{id_string}.xe'

    checker = TDMSlaveTX16Checker(
        "tile[0]:XS1_PORT_1A",
        "tile[0]:XS1_PORT_1C",
        "tile[0]:XS1_PORT_1D",
        "tile[0]:XS1_PORT_1E",
        "tile[0]:XS1_PORT_16B",
        "tile[0]:XS1_PORT_1F")

    ## Temporarily building externally, see hil/build_lib_i2s_tests.sh

    if DEBUG:
        tester = Pyxsim.testers.AssertiveComparisonTester(
            f'{cwd}/expected/tdm_slave_tx16_cb_test.expect',
            regexp = True,
            ordered = True
        )

        with capfd.disabled():
            Pyxsim.run_with_pyxsim(binary,
                            simthreads = [checker],
                            simargs = [],
                            vcdTracing = True,
                            timeout = 100)
            
        tester.run(capfd.readouterr().out)
    else:
        tester = Pyxsim.testers.AssertiveComparisonTester(
            f'{cwd}/expected/tdm_slave_tx16_cb_test.expect',
            regexp = True,
            ordered = True,
            suppress_multidrive_messages=True,
            ignore=["CONFIG:.*"]
        )

        Pyxsim.run_with_pyxsim(binary,
                        simthreads = [checker],
                        simargs = [],
                        timeout = 100)

        tester.run(capfd.readouterr().out)
